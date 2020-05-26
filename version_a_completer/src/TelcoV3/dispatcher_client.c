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
#define NAME_TASK "DispatcherTask%d"

/**
 * @def Size of a task name
 */
#define SIZE_TASK_NAME 20

/**
 * @brief enumeration of the possible command that can send the client
 */

ENUM_DECL(COMMANDE,
	ASK_NOTHING, 
	ASK_DISCONNECT, 
	ASK_DIR_FORWARD, 
	ASK_DIR_BACKWARD, 
	ASK_DIR_RIGHT, 
	ASK_DIR_LEFT,
	ASK_DIR_STOP,
	ASK_MORE_SPEED,
	ASK_LESS_SPEED,
	ASK_LOG,
	ASK_STOP
 )

/**
 * @brief Enumeration of all the STATEs that can be taken by the STATE machine
 */
ENUM_DECL(STATE,
    S_FORGET,               ///< Nothing happens
    S_IDLE,        ///< Idle STATE
    S_LISTENING,            ///< Running STATE
    S_DEATH                 ///< Transition STATE for stopping the STATE machine
)


/**
 * @brief Enumaration of all the possible ACTIONs called by the STATE machine
 */
ENUM_DECL(ACTION,
    A_NOP,                         ///< Nothing happens
    A_START_THREAD_LISTENING,             ///< ACTION called when passing from the RUNNING STATE to the IDLE STATE
    A_PROCESS_DATA,
    A_STOP_THREAD_LISTENING,              ///< ACTION called when passing from the IDLE STATE to the RUNNING STATE                    
    A_KILL                         ///< Kills the STATE machine
)


/**
 * @brief Enumeration of all the possible EVENTs that triggers the STATE machine
 */
ENUM_DECL(EVENT,
    E_MSG_RECEIVED,         ///< Do nothing
    E_START_LISTENING,      ///< EVENT VEL IS NULL THAT STOP POLLING
    E_STOP_LISTENING,       /// 
    E_STOP,                 ///< EVENT SET VEL THAT START POLLING
    E_KILL                  ///< Kills the STATE machine
)


/**
 * @brief Transition structure of the STATE machine
 */
typedef struct {
    STATE nextState;        ///< Next STATE of the STATE machine
    ACTION action;          ///< ACTION done before going in the next STATE
} Transition;

/**
 * @brief Structure of a message sent in the mailbox
 */
typedef struct {
    EVENT event;                ///< EVENT sent in the message
    RQ_data dataReceived;       ///Data received when a msg arrived (RQ_TYPE, COMMAND)
} Msg;

/**
 * @brief Wrapper enum. It is used to send EVENTs and parameters in a mailBox.
 */
wrapperOf(Msg)


/**
 * @brief Structure of the Example object
 */
struct Dispatcher_t {
    pthread_t threadId;             ///< Pthread identifier for the active function of the class.
    pthread_t threadListening;      ///< Pthread identifier for the listening process
    STATE state;                    ///< Actual STATE of the STATE machine
    Msg msg;                        ///< Structure used to pass parameters to the functions pointer.
    char nameTask[SIZE_TASK_NAME];  ///< Name of the task
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
        &ActionStartThreadListening,
        &ActionProcessData,
        &ActionStopThreadListening,
        &ActionKill
};


/**
 * @brief STATE machine of the Example class
 */
static Transition stateMachine[NB_STATE - 1][NB_EVENT] = { 
        [S_IDLE][E_START_LISTENING]         =       {S_LISTENING,	A_START_THREAD_LISTENING},
        [S_LISTENING][E_MSG_RECEIVED]       =       {S_LISTENING, A_PROCESS_DATA},
        [S_LISTENING][E_STOP_LISTENING]     =       {S_IDLE, A_START_THREAD_LISTENING},
        [S_IDLE][E_KILL]                    =       {S_DEATH, A_KILL},
        [S_LISTENING][E_KILL]               =       {S_DEATH, A_KILL}
};

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

// TODO : Write all the ACTION functions

static void ActionStartThreadListening(Dispatcher * this) { 
    StartThreadListening(this); 
    TRACE("[ActionStartThreadListening]\n")
}

static void ActionStopThreadListening(Dispatcher * this) { 
    StopThreadListening(this);
    TRACE("[ActionStopThreadListening]\n")
}

static void ActionProcessData(Dispatcher * this) {  
    processData(this->msg);
    TRACE("[ActionProcessData]\n")
}

static void ActionNop(Dispatcher * this) {
    TRACE("[ActionNOP]\n")
}

static void ActionKill(Dispatcher * this) {
    TRACE("[Action Kill]\n")
    this->state = S_DEATH;
}

/*----------------------- EVENT FUNCTIONS -----------------------*/

/**
 * @brief Start the thread to listen
 */

void StartThreadListening(Dispatcher* this) {
    TRACE("Start Listening Dispatcher \n");

    int err = pthread_create(&(this->threadListening), NULL, (void *) processData, this);
    if(err <0){
        PERRNO("Error when creating the thread\n");
    }
    TRACE("Create dispatcher Thread");
}


/**
 * @brief Start the thread to listen
 */

void StopThreadListening(Dispatcher* this) {
    TRACE("Stop Listening Dispatcher\n");

    int err = pthread_cancel(this->threadListening);
    if(err <0){
        PERRNO("Error when canceling the dispatcher thread\n");
    }
    TRACE("Stop dispatcher Thread");
}



/**
 * @brief Proccess the data received
 * 
 * Commande possible :
 * 
 * C_GET_EVENTS = 0,
 * C_GET_EVENTS_COUNT
 * 
 */
void processData(Msg msgReceived){

    COMMANDE cmd = msgReceived.dataReceived.command;
    EVENT tabEvents[] = * msgReceived.dataReceived.logEvent;

    switch (cmd)
    {
    case C_GET_EVENTS:
        RemoteUI_setEvents(tabEvents);
        TRACE("Get events %c", msgReceived.dataReceived.logEvent);
        break;

    case C_GET_EVENTS_NUMBER:
        int length = (sizeof(tabEvents/sizeof(EVENT)));
        RemoteUI_setEventsCount(length);
        TRACE("Get the number of events %d", length);
        break;

    default:
        break;
    }    
}


/* ----------------------- RUN FUNCTION ----------------------- */

/**
 * @brief Main running function of the Example class
 */
static void DispatcherRun(Dispatcher * this) {
    ACTION action;
    STATE state;
    Wrapper wrapper;

    while (this->state != S_DEATH) {
        mailboxReceive(this->mb, wrapper.toString); ///< Receiving an EVENT from the mailbox

        if (wrapper.data.event == E_KILL) { // If we received the stop EVENT, we do nothing and we change the STATE to death.
            this->state = S_DEATH;

        } else {
            action = stateMachine[this->state][wrapper.data.event].action;

            TRACE("Action %s\n", ACTION_toString[action])

            state = stateMachine[this->state][wrapper.data.event].nextState;
            TRACE("State %s\n", STATE_toString[state])

            if (state != S_FORGET) {
                this->msg = wrapper.data;
                actionPtr[action](this);
                this->state = state;
            }
        }
    }
}


Dispatcher * Dispatcher_New() {
    // TODO : initialize the object with it particularities
    dispatcherCounter ++; ///< Incrementing the instances counter.
    TRACE("DispatcherNew function \n")
    Dispatcher * this = (Dispatcher *) malloc(sizeof(Dispatcher));
    this->mb = mailboxInit("Dispatcher", dispatcherCounter, sizeof(Msg));
    this->state = S_FORGET;

    //this->wd = WatchdogConstruct(1000, &ExampleTimeout, this); ///< Declaration of a watchdog.

    int err = sprintf(this->nameTask, NAME_TASK, dispatcherCounter);
    STOP_ON_ERROR(err < 0, "Error when setting the tasks name.")

    return this; // TODO: Handle the errors
}


int Dispatcher_Start(Dispatcher * this) {
    // TODO : start the object with it particularities
    TRACE("ExampleStart function \n")
    int err = pthread_create(&(this->threadId), NULL, (void *) DispatcherRun, this);
    STOP_ON_ERROR(err != 0, "Error when creating the thread")

    return 0; // TODO: Handle the errors
}


int Dispatcher_Stop(Dispatcher * this) {
    // TODO : stop the object with it particularities
    Msg msg = { .event = E_KILL };

    Wrapper wrapper;
    wrapper.data = msg;

    mailboxSendStop(this->mb, wrapper.toString);
    TRACE("Waiting for the thread to terminate \n")

    int err = pthread_join(this->threadId, NULL);
    STOP_ON_ERROR(err != 0, "Error when waiting for the thread to end")

    return 0; // TODO: Handle the errors
}


int Dispatcher_Free(Dispatcher * this) {
    // TODO : free the object with it particularities
    TRACE("ExampleFree function \n")
    mailboxClose(this->mb);

    free(this);

    return 0; // TODO: Handle the errors
}
