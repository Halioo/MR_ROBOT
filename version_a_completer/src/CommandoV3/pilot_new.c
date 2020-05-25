
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "util.h"

#include "pilot.h"
#include "robot.h"
#include "mailbox.h"
#include "watchdog.h"

#define DEFAULT_SPEED (0)

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
ENUM_DECL(State,
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
ENUM_DECL(Event,
        E_NOP,                  ///< Ne rien faire
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
ENUM_DECL(Action,
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
 * @brief Structire de transition de la MaE
 */
typedef struct {
    State nextState; ///< Prochain State de la MaE
    Action action;   ///< Action réalisée avant d'aller dans le prochain State
} Transition;


/**
 * @brief Structure d'un message ajouté à la BAL
 */
typedef struct {
    Event event; ///< Paramètre event du message
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
    // TODO : ajouter les paramètres statiques pour en faire un objet multi-instances
    Msg message; ///< Structure utilisée pour passer les donnés de la BAL aux pointeurs de fonction
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    Mailbox * mailbox;
    State myState; ///< Etat actuel de la MaE
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

/*--------------------Prototypes des fonctions event-------------------------*/


static void Pilot_EventToggleES(Pilot * this);

static void Pilot_EventTOBump(Pilot * this);

static void Pilot_EventBumped(Pilot * this);

static void Pilot_EventNotBumped(Pilot * this);

/**
 * @brief Fonction run de la classe Pilot
 */
static void Pilot_Run(Pilot * this);

/*----------------------- OTHER FUNCTIONS PROTOTYPES -----------------------*/

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

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

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Pointeur de fonction pour les fonction d'Action de la MaE
 */
typedef void (*ActionPtr)();

/**
 * @brief Tableau de pointeur de fonctions pour stocker les fonctions correspondantes aux actions dans le même ordre qu'elles ont été définies
 */
static const ActionPtr actionPtrTab[NB_Action]={
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
static const Transition stateMachine[NB_State][NB_Event] = {
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

// TODO : Write all the ACTION functions

static void ActionNop(Pilot * this){

}


static void ActionSendMvt0(Pilot * this){
    Pilot_SendMvt(this->message.vel);
}


static void ActionIdleToRunning(Pilot * this){
    Pilot_SendMvt(this->message.vel);
    Pilot_EvalVel(this);
    // TODO : setTO (waf)
}


static void ActionRunningToIdle(Pilot * this){

}


static void ActionRunningToRunning(Pilot * this){

}


static void ActionRunningToBumpCheck(Pilot * this){

}


static void ActionBumpCheckToRunning(Pilot * this){

}


static void ActionKill(){

}


/*----------------------- EVENT FUNCTIONS -----------------------*/
// TODO : write the events functions

extern void Pilot_EventSetRobotVelocity(Pilot * this, VelocityVector vel) {
    Msg msg = {
        .event = E_SET_ROBOT_VELOCITY,
        .vel = vel
    };

    Wrapper wrapper = {
        .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void Pilot_EventStop(Pilot * this) {
    Msg msg = {
        .event = E_STOP,
        .vel = DEFAULT_VELOCITY_VECTOR
    };

    Wrapper wrapper = {
        .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_EventToggleES(Pilot * this) {
    Msg msg = {
        .event = E_TOGGLE_ES
    };

    Wrapper wrapper = {
            .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_EventTOBump(Pilot * this){
    Msg msg = {
        .event = E_TO_BUMP
    };

    Wrapper wrapper = {
        .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_EventBumped(Pilot * this){
    Msg msg = {
        .event = E_BUMPED
    };

    Wrapper wrapper = {
        .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

static void Pilot_EventNotBumped(Pilot * this){
    Msg msg = {
        .event = E_NOT_BUMPED
    };

    Wrapper wrapper = {
        .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}


/* ----------------------- RUN FUNCTION ----------------------- */

static void Pilot_Run(Pilot * this) {
    Action action = A_SEND_MVT_0;
    State state = S_IDLE;
    Wrapper wrapper;

    while (state != S_DEATH) {
        mailboxReceive(this->mailbox,wrapper.toString); ///< On reçoit un message de la mailbox

        if(wrapper.data.event == E_KILL){
            this->myState = S_DEATH;
        }
        else{
            action = stateMachine[state][wrapper.data.event].action;
            TRACE("Action %s\n", Action_toString[action])

            state = stateMachine[state][wrapper.data.event].nextState;
            TRACE("State %s\n", State_toString[state])

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
    // TODO : gestion d'erreurs

    return this;
}


int Pilot_start(Pilot * this) {
    int err = pthread_create(this->threadId,NULL,(void *)Pilot_Run, this);
    // TODO : gestion d'erreurs

}


int Pilot_stop(Pilot * this) {
    int err = pthread_join(this->threadId,NULL);
    // TODO : gestion d'erreurs

}


int Pilot_free(Pilot * this) {
    mailboxClose(this->mailbox);
    // TODO : gestion d'erreurs

    free(this);
    return 0;
}

/* ----------------------- OTHER FUNCTIONS -----------------------*/


static void Pilot_SendMvt(VelocityVector vel) {
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


static void Pilot_EvalVel(Pilot * this){
    if(this->message.vel.dir == STOP || this->message.vel.power == 0){
        Msg msg = {
            .event = E_STOP
        };
        Wrapper wrapper = {
            .data = msg
        };

        mailboxSendMsg(this->mailbox,wrapper.toString);
    }
}

