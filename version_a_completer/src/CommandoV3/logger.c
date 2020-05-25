//
// Created by gwendal on 25/05/2020.
//
#include "util.h"
#include "logger.h"
#include "robot.h"

/**
 * @def Name of the task. Each instance will have this name,
 * followed by the number of the instance
 */
#define NAME_TASK "LoggerTask%d"

static int loggerCounter = 0;


/**STATE
 * Liste des différents états possibles du logger
 * FORGET:aucune action n'est effectuée
 * IDLE: en attente du démarage du logger
 * POLLING: recolte les LogEvent
 */
ENUM_DECL(STATE,
          S_FORGET=0,
          S_IDLE,
          S_POLLING,
          S_DEATH        ///< Etat de mort du robot, utilisé lorsqu'on veut l'arrêter
)

/**
 * Liste des event possibles
 */

ENUM_DECL(EVENT,
          E_START_POLLING,      // Evenement pour commencer à scruter le robot
          E_STOP_POLLING,       // Evenement pour arrêter de scruter le robot
          E_TO_POLL,            // Evenement appelé à expiration du timer de 250ms
          E_KILL                  ///< Détruit la MaE
)

/**
 * Liste des différentes actions possibles
 */

ENUM_DECL(ACTION,
          A_NOP,                ///< Ne rien faire
          A_START_POLLING,      ///< On lance le timer
          A_STOP_POLLING,       ///< On reset le timer - sert d'action pour quitter
          A_POLL,               ///< On relève l'état du robot
          A_KILL
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
    EVENT event;
    LogEvent logEvent; ///< Paramètre event du message
} Msg;

/**
 * @brief Wrapper enum. Utilisé pour envoyer des Msg dans une BAL
 */
wrapperOf(Msg)

/**
 * @brief Structure de l'objet Logger
 */
struct Logger_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    SensorState sens;
    int speed;
    Mailbox * mailbox;
    Msg message; ///< Structure utilisée pour passer les donnés de la BAL aux pointeurs de fonction
    STATE myState; ///< Etat actuel de la MaE
    Liste * myEvents;

};

/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

static void Logger_polling(Logger *this);

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

static void ActionNop(Logger * this);

static void ActionStartPolling(Logger * this);
static void ActionStopPolling(Logger * this);

static void ActionPoll(Logger * this);

static void ActionKll(Logger * this);

/* ----------------------- EVENT FUNCTIONS ----------------------- */

static void Logger_EventTOPoll(Logger * this);


/*----------------------- OTHER FUNCTIONS PROTOTYPES -----------------------*/

/**
 * @brief Fonction run de la classe Logger
 */
static void Logger_Run(Logger * this);


/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Pointeur de fonction pour les fonction d'Action de la MaE
 */
typedef void (*ActionPtr)(Logger *);

/**
 * @brief Tableau de pointeur de fonctions pour stocker les fonctions correspondantes aux actions dans le même ordre qu'elles ont été définies
 */
static const ActionPtr actionPtrTab[NB_ACTION]={
        &ActionNop,
        &ActionPoll,
        &ActionStartPolling,
        &ActionStopPolling,
        &ActionKll
};

/**
 * Tableau des transitions
 * [etat_courant][type_event]
 */
static Transition stateMachine[NB_STATE][NB_EVENT] ={
        [S_IDLE][E_START_POLLING] = {S_POLLING, A_START_POLLING},
        [S_POLLING][E_STOP_POLLING] = {S_IDLE, A_STOP_POLLING},
        [S_POLLING][E_TO_POLL] = {S_POLLING, A_POLL},
        [S_IDLE][E_KILL] = {S_DEATH, A_STOP_POLLING},
        [S_POLLING][E_KILL] = {S_DEATH, A_STOP_POLLING}
};


/*----------------------- EVENT FUNCTIONS -----------------------*/

extern void Logger_startPolling(Logger * this) {

    Wrapper wrapper = {
            .data.event = E_START_POLLING
    };
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void Logger_EventStopPolling(Logger * this) {

    Wrapper wrapper = {
            .data.event = E_STOP_POLLING
    };
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void Logger_EventTOPoll(Logger * this) {

    Wrapper wrapper = {
            .data.event = E_TO_POLL
    };
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

/* ----------------------- RUN FUNCTION ----------------------- */

static void Logger_Run(Logger * this) {
    ACTION action;
    STATE state = S_IDLE;
    Wrapper wrapper;

    while (state != S_DEATH) {
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

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

static void ActionNop(Logger * this){
    //ne rien faire
}

static void ActionPoll(Logger * this) {
    /*Watchdog *watchdog;
    WatchdogCallback callback={watchdog, &Logger_polling};
    watchdog=WatchdogConstruct(250,*callback, this);
    WatchdogStart(watchdog);
    WatchdogCancel(watchdog);
    WatchdogDestroy(watchdog);
     */
    //TODO je comprends pas le watchdog donc à refaire
}

/* ----------------------- NEW START STOP FREE -----------------------*/

extern Logger *  Logger_new() {
    loggerCounter++;
    Logger * this = (Logger *) malloc(sizeof(Logger));
    this->mailbox = mailboxInit("Pilot",loggerCounter,sizeof(Msg));
    this->myEvents=initialisation();

    return this;
}

extern void Logger_start(Logger * this) {
    pthread_create(&(this->threadId),NULL,(void *)Logger_Run, this);

}

extern void Logger_stop(Logger * this) {
    pthread_join(this->threadId,NULL);

}

extern void Logger_free(Logger * this) {
    mailboxClose(this->mailbox);
    free(this);
}

/* ----------------------- STATIC FUNCTION DEFINITION -----------------------*/


static void Logger_polling(Logger *this){
    this->sens=Logger_getSensorState(this);
    this->speed=Logger_getRobotSpeed(this);
    LogEvent logEvent={this->sens,this->speed};
    insertion(this->myEvents, logEvent);
    Logger_Run(this);
}

/* ----------------------- FUNCTION DEFINITION -----------------------*/

extern SensorState Logger_getSensorState(Logger * this){
    SensorState sens=Robot_getSensorState();
    return sens;
}

extern int Logger_getRobotSpeed(Logger * this){
    int speed=Robot_getRobotSpeed();
    return speed;
}

extern void Logger_signalES(Logger *this){
    this->myState=S_DEATH;
    Logger_Run(this);
}

extern LogEvent* Logger_getEvents(int from, int to,LogEvent *logEventToReturn,Logger * this){
    int count=0;
    //LogEvent logEventToReturn[(to-from)+1];

    Element *actuel = this->myEvents->premier;

    while (actuel != NULL)
    {
        if(count>=from && count<=to){
            logEventToReturn[count]=actuel->logEvent;
            count++;

        }
        actuel = actuel->suivant;
    }
    return logEventToReturn;
}

extern int Logger_getEventsCount(Logger * this){
    int count=0;

    Element *actuel = this->myEvents->premier;

    while (actuel != NULL)
    {
        count++;
        actuel = actuel->suivant;
    }
    return count;
}


extern void Logger_clearEvents(Logger * this){
    suppression(this->myEvents);
}




