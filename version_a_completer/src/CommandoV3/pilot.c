
#include <stdio.h>
#include <assert.h>

#include "../../lib/include/util.h"
#include "../../lib/include/pilot.h"
#include "robot.h"
#include "../../lib/include/mailbox.h"
#include "../../lib/include/watchdog.h"

#define SIZE_TASK_NAME (20)
#define DEFAULT_SPEED (0)
#define BUMP_TEST_REFRESH_RATE (1000000)

/**
 * @def Name of the task. Each instance will have this name,
 * followed by the number of the instance
 */
#define NAME_TASK "PilotTask%d"

/**
 * @brief Example instances counter used to have a unique queuename per thread
 */
static int pilotCounter = 0;

/**
 * Vector velocité par défaut : arrêt
 */
static const VelocityVector DEFAULT_VELOCITY_VECTOR =
{
    .dir = STOP,
    .power = DEFAULT_SPEED
};

/**
 * @brief Liste des états du pilot
 */
ENUM_DECL(STATE,
        S_FORGET,       ///< Etat par défaut, utilisé pour consommer une action
        S_IDLE,         ///< Etat où le robot est arrêté
        S_RUNNING,      ///< Etat où le robot est en mouvement
        S_BUMP_CHECK,   ///< Etat pour vérifier si le robot a heurté un obstacle
        S_EMERGENCY,    ///< Etat d'arrêt d'urgence
        S_DEATH         ///< Etat de mort du robot, utilisé lorsqu'on veut l'arrêter
)

/**
 * @brief Liste des évènements du pilot
 */
ENUM_DECL(EVENT,
        E_SET_ROBOT_VELOCITY,   ///< Demande d'envoi d'une vitesse au robot
        E_STOP,                 ///< Met la vitesse du robot à 0
        E_TOGGLE_ES,            ///< Signal d'urgence reçu
        E_TO_BUMP,              ///< Watchdog arrivé à expiration pour vérifier la collision
        E_BUMPED,               ///< Le robot s'est cogné
        E_NOT_BUMPED,           ///< Le robot ne s'est pas cogné
        E_KILL                  ///< Détruit la MaE
)

/**
 * @brief Liste des actions du pilot
 */
ENUM_DECL(ACTION,
        A_NOP,                      ///< Ne rien faire
        A_SEND_MVT_0,               ///< Envoi d'une vitesse nulle au robot
        A_IDLE_TO_RUNNING,          ///< Action appellée quand on passe de l'état IDLE à l'état RUNNING
        A_RUNNING_TO_IDLE,          ///< Action appellée quand on passe de l'état RUNNING à l'état IDLE
        A_RUNNING_TO_RUNNING,       ///< Action appellée quand on passe de l'état RUNNING à l'état RUNNING
        A_RUNNING_TO_BUMP_CHECK,    ///< Action appellée quand on passe de l'état RUNNING à l'état BUMP_CHECK
        A_BUMP_CHECK_TO_RUNNING,    ///< Action appellée quand on passe de l'état BUMP_CHECK à l'état RUNNING
        A_KILL                      ///< Action pour détruire la MaE
)

/**
 * @brief Structure de transition de la MaE
 */
typedef struct {
    STATE nextState; ///< Prochain State de la MaE
    ACTION action;   ///< Action réalisée avant d'aller dans le prochain State
} Transition;


/**
 * @brief Structure d'un message ajouté à la BAL
 */
typedef struct {
    EVENT event; ///< Paramètre event du message
    VelocityVector vel; ///< Paramètre VelocityVector du message
} Msg;

/**
 * @brief Wrapper enum. Utilisé pour envoyer des Msg dans une BAL
 */
wrapperOf(Msg)

/**
 * @brief Structure de l'objet Pilot
 */
struct Pilot_t {
    VelocityVector currentVel;
    PilotState myPilotState;

    Msg message; ///< Structure utilisée pour passer les donnés de la BAL aux pointeurs de fonction
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    Mailbox * mailbox; ///< La boite aux lettres de la MaE de Pilot
    Watchdog * watchdogBump; ///< Le watchdog qui gère la scrutation de bump
    STATE myState; ///< Etat actuel de la MaE
    char nameTask[SIZE_TASK_NAME]; ///< Name of the task

};


/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/*------------- ACTION functions -------------*/

/**
 * @brief Fonction appellée quand il ne faut rien faire
 */
static void ActionNop(Pilot * this);

/**
 * @brief Fonction appellée quand il faut immobiliser le robot
 */
static void ActionSendMvt0(Pilot * this);

/**
 * @brief Fonction appellée quand il faut passer de l'état IDLE à l'état RUNNING
 */
static void ActionIdleToRunning(Pilot * this);

/**
 * @brief Fonction appellée quand il faut passer de l'état RUNNING à l'état IDLE
 */
static void ActionRunningToIdle(Pilot * this);

/**
 * @brief Fonction appellée quand il faut passer de l'état RUNNING à l'état RUNNING
 */
static void ActionRunningToRunning(Pilot * this);

/**
 * @brief Fonction appellée quand il faut passer de l'état RUNNING à l'état BUMP_CHECK
 */
static void ActionRunningToBumpCheck(Pilot * this);

/**
 * @brief Fonction appellée quand il faut passer de l'état BUMP_CHECK à l'état RUNNING
 */
static void ActionBumpCheckToRunning(Pilot * this);

/**
 * @brief Fonction appellée quand il faut détruire la MaE
 */
static void ActionKill(Pilot * this);

/*------------- EVENT functions -------------*/


static void Pilot_EventTOBump(Pilot * this);

static void Pilot_EventBumped(Pilot * this);

static void Pilot_EventNotBumped(Pilot * this);


/*----------------------- OTHER FUNCTIONS PROTOTYPES -----------------------*/

/**
 * @brief Fonction run de la classe Pilot
 */
static void Pilot_Run(Pilot * this);

/**
 * @brief Envoie un ordre de vitesse au robot
 * @param vel
 */
static void Pilot_SendMvt(VelocityVector vel);

/**
 * @brief Evalue la vitesse et ajoute un message E_STOP si la vitesse est nulle
 * @param vel
 */
static void Pilot_EvalVel(Pilot * this);

/**
 * @brief Evalue si le robot a rencontré un obstacle
 * @param this
 */
static void Pilot_EvalBump(Pilot * this);

/**
 * @brief Initialise le timer du watchdog qui vérifie le bump toutes les secondes
 * @param this
 */
static void Pilot_SetTO(Pilot * this);

/**
 * @brief Reset le timer du watchdog qui vérifie le bump toutes les secondes
 * @param this
 */
static void Pilot_ResetTO(Pilot * this);

/**
 * @brief Fonction qui gère ce qu'il se passe quand le watchdog
 * @param this
 */
static void Pilot_TOHandle(void * this);


/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Pointeur de fonction pour les fonction d'Action de la MaE
 */
typedef void (*ActionPtr)(Pilot *);

/**
 * @brief Tableau de pointeur de fonctions pour stocker les fonctions correspondantes aux actions dans le même ordre qu'elles ont été définies
 */
static const ActionPtr actionPtrTab[NB_ACTION]={
        &ActionNop,
        &ActionSendMvt0,
        &ActionIdleToRunning,
        &ActionRunningToIdle,
        &ActionRunningToRunning,
        &ActionRunningToBumpCheck,
        &ActionBumpCheckToRunning,
        &ActionKill
};


/**
 * @brief MaE de la classe Pilot
 */
static const Transition stateMachine[NB_STATE][NB_EVENT] = {
        [S_IDLE][E_TOGGLE_ES] = {S_EMERGENCY,A_SEND_MVT_0},
        [S_IDLE][E_SET_ROBOT_VELOCITY] = {S_RUNNING,A_IDLE_TO_RUNNING},
        [S_RUNNING][E_STOP] = {S_IDLE,A_RUNNING_TO_IDLE},
        [S_RUNNING][E_TOGGLE_ES] = {S_EMERGENCY,A_SEND_MVT_0},
        [S_RUNNING][E_SET_ROBOT_VELOCITY] = {S_RUNNING,A_RUNNING_TO_RUNNING},
        [S_RUNNING][E_TO_BUMP] = {S_BUMP_CHECK,A_RUNNING_TO_BUMP_CHECK},
        [S_BUMP_CHECK][E_NOT_BUMPED] = {S_RUNNING,A_BUMP_CHECK_TO_RUNNING},
        [S_BUMP_CHECK][E_BUMPED] = {S_IDLE,A_SEND_MVT_0},
        [S_BUMP_CHECK][E_TOGGLE_ES] = {S_EMERGENCY,A_SEND_MVT_0},
        [S_EMERGENCY][E_TOGGLE_ES] = {S_IDLE,A_SEND_MVT_0}
        // TODO : gérer le kill ?
};

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */


static void ActionNop(Pilot * this){
    TRACE("[%s] ACTION - Nop\n",this->nameTask)

}


static void ActionSendMvt0(Pilot * this){
    TRACE("[%s] ACTION - Send mvt 0\n",this->nameTask)
    Pilot_SendMvt(this->message.vel);
}


static void ActionIdleToRunning(Pilot * this){
    TRACE("[%s] ACTION - Idle to running\n",this->nameTask)
    Pilot_SetTO(this);
    Pilot_EvalVel(this);
    Pilot_SendMvt(this->message.vel);
}


static void ActionRunningToIdle(Pilot * this){
    TRACE("[%s] ACTION - Running to idle\n",this->nameTask)
    Pilot_ResetTO(this);
    Pilot_SendMvt(DEFAULT_VELOCITY_VECTOR);
}


static void ActionRunningToRunning(Pilot * this){
    TRACE("[%s] ACTION - Running to running\n",this->nameTask)
    Pilot_EvalVel(this);
    Pilot_SendMvt(this->message.vel);
}


static void ActionRunningToBumpCheck(Pilot * this){
    TRACE("[%s] ACTION - Running to bump check\n",this->nameTask)
    Pilot_EvalBump(this);
}


static void ActionBumpCheckToRunning(Pilot * this){
    TRACE("[%s] ACTION - Bump check to running\n",this->nameTask)
    Pilot_SetTO(this);
}


static void ActionKill(Pilot * this){
    TRACE("[%s] ACTION - Kill\n",this->nameTask)
    // TODO
}


/*----------------------- EVENT FUNCTIONS -----------------------*/

extern void Pilot_setRobotVelocity(Pilot * this, VelocityVector vel) {
    Msg msg = {
        .event = E_SET_ROBOT_VELOCITY,
        .vel = vel
    };

    Wrapper wrapper = {
        .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void Pilot_ToggleES(Pilot * this) {

    Wrapper wrapper = {
        .data.event = E_TOGGLE_ES
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_EventTOBump(Pilot * this){

    Wrapper wrapper = {
        .data.event = E_TO_BUMP
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_EventBumped(Pilot * this){

    Wrapper wrapper = {
        .data.event = E_BUMPED
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_EventNotBumped(Pilot * this){

    Wrapper wrapper = {
        .data.event = E_NOT_BUMPED
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}


/* ----------------------- RUN FUNCTION ----------------------- */

static void Pilot_Run(Pilot * this) {
    ACTION action;
    STATE state;
    Wrapper wrapper;

    TRACE("[%s] RUN\n",this->nameTask)

    while (this->myState != S_DEATH) {
        mailboxReceive(this->mailbox,wrapper.toString); ///< On reçoit un message de la mailbox

        if(wrapper.data.event == E_KILL){
            this->myState = S_DEATH;
        }
        else{
            action = stateMachine[state][wrapper.data.event].action;
            TRACE("Action %s\n", ACTION_toString[action])

            state = stateMachine[state][wrapper.data.event].nextState;
            TRACE("State %s\n", STATE_toString[state])

            if(state != S_FORGET){
                this->message = wrapper.data;
                actionPtrTab[action](this);
                this->myState = state;
            }
        }
    }
}


/* ----------------------- NEW START STOP FREE -----------------------*/

Pilot *  Pilot_new() {
    pilotCounter++;
    Pilot * this = (Pilot *) malloc(sizeof(Pilot));
    this->mailbox = mailboxInit("Pilot",pilotCounter,sizeof(Msg));
    this->watchdogBump = WatchdogConstruct(BUMP_TEST_REFRESH_RATE,&Pilot_TOHandle,this);
    sprintf(this->nameTask, NAME_TASK, pilotCounter);
    this->myState = S_IDLE;
    // TODO : gestion d'erreurs

    return this;
}


int Pilot_Start(Pilot * this) {
    int err = pthread_create(&(this->threadId),NULL,(void *)Pilot_Run, this);
    // TODO : gestion d'erreurs

}


int Pilot_Stop(Pilot * this) {
    Wrapper wrapper;
    wrapper.data.event = E_KILL;
    mailboxSendMsg(this->mailbox,wrapper.toString);
    WatchdogCancel(this->watchdogBump);

    pthread_join(this->threadId,NULL);
    // TODO : gestion d'erreurs

}


int Pilot_Free(Pilot * this) {
    mailboxClose(this->mailbox);
    WatchdogDestroy(this->watchdogBump);
    // TODO : gestion d'erreurs

    free(this);
    return 0;
}

/* ----------------------- OTHER FUNCTIONS -----------------------*/

static void Pilot_EvalVel(Pilot * this){
    if(this->message.vel.dir == STOP || this->message.vel.power == 0){

        Wrapper wrapper = {
            .data.event = E_STOP
        };

        mailboxSendMsg(this->mailbox,wrapper.toString);
    }
}

static void Pilot_SetTO(Pilot * this){
    WatchdogStart(this->watchdogBump);
}

static void Pilot_ResetTO(Pilot * this){
    WatchdogCancel(this->watchdogBump);
}

static void Pilot_TOHandle(void * this){

    Wrapper wrapper = {
        .data.event = E_TO_BUMP
    };

    mailboxSendMsg(((Pilot*)this)->mailbox,wrapper.toString);
}

static void Pilot_EvalBump(Pilot * this){
    Wrapper wrapper;
    if(Robot_getSensorState().collision_f == UP){
        wrapper.data.event = E_BUMPED;
    } else{
        wrapper.data.event = E_NOT_BUMPED;
    }
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_SendMvt(VelocityVector vel) {
    vel = translateDir(vel.dir);
    int vel_r = DEFAULT_SPEED, vel_l = DEFAULT_SPEED;
    switch (vel.dir) {
        case FORWARD:
            vel_r = vel.power;
            vel_l = vel.power;
            break;
        case BACKWARD:
            vel_r = -vel.power;
            vel_l = -vel.power;
            break;
        case RIGHT:
            vel_r = -vel.power;
            vel_l = vel.power;
            break;
        case LEFT:
            vel_r = vel.power;
            vel_l = -vel.power;
            break;
        default: break;
    }
    Robot_setWheelsVelocity(vel_r, vel_l);
}

