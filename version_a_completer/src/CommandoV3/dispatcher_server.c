#include "dispatcher_server.h"


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
    Pilot * pilot;
    Logger * logger;
    FLAG flagListening;
    RQ_data dataToProcess;

    // TODO : add here the instance variables you need to use.
    //Watchdog * wd; ///< Example of a watchdog implementation
    //int b; ///< Instance example variable
};

/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/*------------- ACTION functions -------------*/

/**
 * @brief Function called when there is the EVENT Example 1 and when the STATE is Idle
 */
static void ActionStartThreadListening(Dispatcher * this);

/**
 * @brief Function called when there is the EVENT Example 1 and when the STATE is Idle
 */
static void ActionStopThreadListening(Dispatcher * this);

/**
 * @brief Function called when there is the EVENT Example 1 and when the STATE is Idle
 */
static void ActionProcessData(Dispatcher * this);

/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(Dispatcher * this);

/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(Dispatcher * this);


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
    this->flagListening = DOWN;
    pthread_create(&(this->threadListening), NULL, (void *) Listen, this); 
    TRACE("[ActionStartThreadListening]\n")
}

static void ActionStopThreadListening(Dispatcher * this) { 
    this->flagListening = UP;
    pthread_join(this->threadListening, NULL);
    TRACE("[ActionStopThreadListening]\n")
}

static void ActionProcessData(Dispatcher * this) {  
    processData(this, this->msg);
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
 * @brief Proccess the data received
 * 
 * Commande possible :
 * 
 * C_LEFT = 0,
 * C_RIGHT,
 * C_FORWARD,
 * C_BACKWARD,
 * C_STOP,
 * C_LOGS,
 * C_STATE,
 * C_QUIT,
 * C_EVENTS,
 * C_EVENTSCOUNT
 * 
 * 
 */
void processData(Dispatcher * this, Msg msgReceived){

    COMMAND cmd = msgReceived.dataReceived.command;
    VelocityVector vel =
    {
        .dir = STOP,
        .power = 80
    };

    switch (cmd)
    {
    case C_LEFT:
        vel.dir = LEFT;
        Pilot_setRobotVelocity(this->pilot, vel);
        TRACE("Going Left");
        break;
    case C_RIGHT:
        vel.dir = RIGHT;
        Pilot_setRobotVelocity(this->pilot, vel);
        TRACE("Going Right");
        break;
    case C_FORWARD:
        vel.dir = FORWARD;
        Pilot_setRobotVelocity(this->pilot, vel);
        TRACE("Going Forward");
        break;
    case C_BACKWARD:
        vel.dir = BACKWARD;
        Pilot_setRobotVelocity(this->pilot, vel);
        TRACE("Going Backward");
        break;
    case C_STOP:
        Pilot_EventStop(this->pilot);
        TRACE("Stop");
        break;
    case C_LOGS:
        //TO DO : A définir si utile ou non
        TRACE("Logs");
        break;
    case C_STATE:
        Pilot_getState();
        TRACE("State");
        break;
    case C_QUIT:
        TRACE("Quit");
        break;
    case C_EVENTS:
        Logger_setEvents(this->dataToProcess.logEvent, this->logger);
        TRACE("Get events %c", this->dataToProcess.logEvent);
        break;
    case C_EVENTSCOUNT:
        Logger_setEventsCount(this->dataToProcess.eventsCount, this->logger);
        TRACE("Get the number of events %d", this->dataToProcess.eventsCount);
        break;
    default:
        break;
    }  
  
}

/**
 * @brief Proccess the data received
 * 
 * Commande possible :
 * 
 * C_EVENTS = 0,
 * C_EVENTSCOUNT
 * 
 */
static RQ_data Listen(Dispatcher * this){
    //TO DO : Définir le socket à utiliser pour 
    int socketBidon;

    while(this->flagListening == DOWN){
        this->dataToProcess = readNwk(RemoteUI_getSocket(socketBidon));
        Wrapper wrapper;
        wrapper.data.event = E_MSG_RECEIVED;
        mailboxSendMsg(this->mb,wrapper.toString);
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


Dispatcher * Dispatcher_New(Logger * logger, Pilot * pilot) {
    // TODO : initialize the object with it particularities
    dispatcherCounter ++; ///< Incrementing the instances counter.
    TRACE("DispatcherNew function \n")
    Dispatcher * this = (Dispatcher *) malloc(sizeof(Dispatcher));
    this->mb = mailboxInit("Dispatcher", dispatcherCounter, sizeof(Msg));
    this->state = S_IDLE;
    this->pilot = pilot;
    this->logger = logger;

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

