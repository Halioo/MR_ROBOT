#include "dispatcher_client.h"

/**
 * @brief Example instances counter used to have a unique queuename per thread
 */
static int dispatcherCounter = 0;

/* ----------------------- MAILBOX DEFINITIONS -----------------------*/

/**
 * @def Name of the task. Each instance will have this name,
 * followed by the number of the instance
 */
#define NAME_TASK "ExampleTask%d"

/**
 * @def Size of a task name
 */
#define SIZE_TASK_NAME 20


/**
 * @brief Enumeration of all the STATEs that can be taken by the STATE machine
 */
ENUM_DECL(STATE,
    S_FORGET,      ///< Nothing happens
    S_IDLE,        ///< Idle STATE
    S_POLLING,     ///< Running STATE
    S_DEATH        ///< Transition STATE for stopping the STATE machine
)


/**
 * @brief Enumaration of all the possible ACTIONs called by the STATE machine
 */
ENUM_DECL(ACTION,
    A_NOP,                      ///< Nothing happens
    A_STOP_RUNNING,             ///< ACTION called when passing from the RUNNING STATE to the IDLE STATE
    A_START_POLLING,            ///< ACTION called when passing from the IDLE STATE to the RUNNING STATE
    A_KILL                      ///< Kills the STATE machine
)


/**
 * @brief Enumeration of all the possible EVENTs that triggers the STATE machine
 */
ENUM_DECL(EVENT,
    E_NOP,      ///< Do nothing
    E_VEL_IS_NULL, ///< EVENT VEL IS NULL THAT STOP POLLING
    E_SET_VEL, ///< EVENT SET VEL THAT START POLLING
    E_KILL     ///< Kills the STATE machine
)


/**
 * @brief Transition structure of the STATE machine
 */
typedef struct {
    STATE nextState; ///< Next STATE of the STATE machine
    ACTION action;   ///< ACTION done before going in the next STATE
} Transition;

/**
 * @brief Structure of a message sent in the mailbox
 */
typedef struct {
    EVENT event;            ///< EVENT sent in the message
    int eventsCount;        ///< Example of an other parameter
    EVENT tabEvents[10];      ///< Example of a possible parameter
} Msg;


/**
 * @brief Structure of the Example object
 */
struct Dispatcher_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    STATE state;        ///< Actual STATE of the STATE machine
    Msg msg;            ///< Structure used to pass parameters to the functions pointer.
    char nameTask[SIZE_TASK_NAME]; ///< Name of the task
    Mailbox * mb;

    // TODO : add here the instance variables you need to use.
    //Watchdog * wd; ///< Example of a watchdog implementation
    //int b; ///< Instance example variable
};

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Function pointer used to call the ACTIONs of the STATE machine.
 */
typedef void (*ActionPtr)(Dispatcher*);

/**
 * @brief Function pointer array used to call the ACTIONs of the STATE machine.
 */
static const ActionPtr actionPtr[NB_ACTION] = { 
        &ActionNop,
        &ActionExample1FromPolling,
        &ActionExample1FromIdle,
        &ActionKill
};


/**
 * @brief STATE machine of the Example class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = { 
        [S_IDLE][E_SET_VEL]    = {S_POLLING,	A_START_POLLING},
        [S_POLLING][E_VEL_IS_NULL] = {S_IDLE, A_STOP_RUNNING},
        [S_POLLING][E_KILL] = {S_DEATH, A_KILL},
        [S_IDLE][E_KILL] = {S_DEATH, A_KILL}

};

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

// TODO : Write all the ACTION functions

static void ActionExample1FromPolling(Dispatcher * this) {
    TRACE("[ActionEx1FromRunning] - %d\n", this->msg.tabEvents)
}


static void ActionExample1FromIdle(Dispatcher * this) {
    TRACE("[ActionEx1FromIdle] - %d\n", this->msg.event)
}


static void ActionNop(Dispatcher * this) {
    TRACE("[ActionNOP]\n")
}

static void ActionKill(Dispatcher * this) {
    TRACE("[Action Kill]\n")
    this->state = S_DEATH;
}

extern Dispatcher* Dispatcher_new(){
    Dispatcher* this;
    this = (Dispatcher*)(malloc(sizeof(Dispatcher)));
    this->state = S_FORGET;

    dispatcherCounter ++;
    return this;
}

extern void Dispatcher_free(Dispatcher* this);

extern void setEvents() {
    TRACE("from : remoteUI // msg: setEvents \n")
}

extern void setEventsCount() {
    TRACE("to : remoteUI // msg: setEventsCount")
}