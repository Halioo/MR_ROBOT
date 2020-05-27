
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../lib/include/remoteui.h"
#include "../../lib/include/liste_chainee.h"


//#define LANG FRENCH


static int remoteUIcounter = 0;
static Liste * myEvents;
static int myNbEvents;

/**
 * @def Name of the task. Each instance will have this name,
 * followed by the number of the instance
 */
#define NAME_TASK "RemoteUITask%d"

/**
 * @def Size of a task name
 */
#define SIZE_TASK_NAME 20

/* ---------- TYPE DEFINITIONS ----------*/

/**
 * @brief Enumeration of all the states that can be taken by the state machine
 */
ENUM_DECL(STATE,
    S_FORGET,      ///< Nothing happens
    S_CONNECT_SCREEN,
    PS_CONNECT,
    S_MAIN_SCREEN,
    S_LOG_SCREEN,
    S_ERROR_SCREEN,
    S_DEATH
)

/**
 * @brief Enumeration of all the possible events that triggers the state machine
 */
ENUM_DECL(EVENT,
    E_NOP,  ///< Do nothing

    E_SETIP,
    E_VALIDATE,

    E_CONNECT_SUCCESS,
    E_CONNECT_FAILURE,

    E_SETDIR,
    E_TOGGLE_ES,
    E_GO_LOG,
    E_BACK_MAIN,

    E_AFTER_ONE_SEC,

    E_QUIT,

    E_KILL    ///< Kills the STATE machine
)

/**
 * @brief Enumaration of all the possible actions called by the state machine
 */
ENUM_DECL(ACTION,
    A_NOP,  ///< Nothing happens

    A_INIT,
    A_SETIP,

    A_CONNECT,
    A_CONNECT_SUCCESS,
    A_CONNECT_FAILURE,

    A_SETDIR,
    A_TOGGLE_ES,

    A_QUIT,

    A_KILL  ///< Kills the state machine
)

ENUM_DECL(SCREEN,
    CONNECT_SCREEN,
    MAIN_SCREEN,
    ERROR_SCREEN,
    LOG_SCREEN
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
    EVENT event; ///< EVENT sent in the message
    char ip[15];
    DIRECTION dir;
} Msg;

/**
 * @brief Wrapper enum. It is used to send EVENTs and parameters in a mailBox.
 */
wrapperOf(Msg)


/**
* @brief Structure of the RemoteUI object
*/
struct RemoteUI_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    STATE state;        ///< Actual STATE of the STATE machine
    Msg msg;            ///< Structure used to pass parameters to the functions pointer.

    char nameTask[SIZE_TASK_NAME]; ///< Name of the task
    Mailbox * mb;
    Watchdog * wd;

    char myIP[15];
    int socket;

    int previousEventNumber;
    int currentEventNumber;
    VelocityVector vel;

    LogEvent * myEvents; // TODO : définir la taille max de myEvents pour ne pas être embêté
    int myEventsCount;
};


/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/*------------- ACTION functions -------------*/

/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(RemoteUI * this);

/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(RemoteUI * this);

static void ActionInit(RemoteUI * this);
static void ActionSetIp(RemoteUI * this);
static void ActionConnect(RemoteUI * this);
static void ActionConnectSuccess(RemoteUI * this);
static void ActionConnectFailure(RemoteUI * this);
static void ActionSetDir(RemoteUI * this);
static void ActionToggleES(RemoteUI * this);
static void ActionQuit(RemoteUI * this);

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Function pointer used to call the ACTIONs of the STATE machine.
 */
typedef void (*ActionPtr)(RemoteUI *);

/**
 * @brief Function pointer array used to call the ACTIONs of the STATE machine.
 */
static const ActionPtr actionPtr[NB_ACTION] = { // TODO : add all the function pointers corresponding to the ACTION enum in the right order.
    &ActionNop,
    &ActionInit,
    &ActionSetIp,
    &ActionConnect,
    &ActionConnectSuccess,
    &ActionConnectFailure,
    &ActionSetDir,
    &ActionToggleES,
    &ActionQuit,
    &ActionKill
};


/**
 * @brief STATE machine of the Example class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = { // TODO : fill the STATE machine
        [S_CONNECT_SCREEN][E_SETIP] = {S_CONNECT_SCREEN, A_SETIP},
};


/* ----------------------- EVENT FUNCTIONS ----------------------- */

extern void RemoteUI_setIp(RemoteUI * this, char * ip) {
    Wrapper wrapper;
    wrapper.data.event = E_SETIP;
    strcpy(wrapper.data.ip, ip);
    mailboxSendMsg(this->mb, wrapper.toString);
}

extern void RemoteUI_validate(RemoteUI * this) {
    Wrapper wrapper;
    wrapper.data.event = E_VALIDATE;
    mailboxSendMsg(this->mb, wrapper.toString);
}
extern void RemoteUI_connectSuccess(RemoteUI * this) {
    Wrapper wrapper;
    wrapper.data.event = E_CONNECT_SUCCESS;
    mailboxSendMsg(this->mb, wrapper.toString);
}
extern void RemoteUI_connectFailure(RemoteUI * this) {
    Wrapper wrapper;
    wrapper.data.event = E_CONNECT_FAILURE;
    mailboxSendMsg(this->mb, wrapper.toString);
}

extern void RemoteUI_setDir(RemoteUI * this, DIRECTION dir) {
    Wrapper wrapper;
    wrapper.data.event = E_SETDIR;
    wrapper.data.dir = dir;
    mailboxSendMsg(this->mb, wrapper.toString);
}
extern void RemoteUI_toggleEmergencyStop(RemoteUI * this) {
    Wrapper wrapper;
    wrapper.data.event = E_TOGGLE_ES;
    mailboxSendMsg(this->mb, wrapper.toString);
}

extern void RemoteUI_goScreenLog(RemoteUI * this) {
    Wrapper wrapper;
    wrapper.data.event = E_GO_LOG;
    mailboxSendMsg(this->mb, wrapper.toString);
}
extern void RemoteUI_backMainScreen(RemoteUI * this) {
    Wrapper wrapper;
    wrapper.data.event = E_BACK_MAIN;
    mailboxSendMsg(this->mb, wrapper.toString);
}

extern void RemoteUI_quit(RemoteUI * this) {
    Wrapper wrapper;
    wrapper.data.event = E_QUIT;
    mailboxSendMsg(this->mb, wrapper.toString);
}


/**
 * @brief Example function that treats a wathdog event.
 */
extern void Wd_timeout(Watchdog * wd, void * caller) {
    Wrapper wrapper;
    wrapper.data.event = E_AFTER_ONE_SEC;
    mailboxSendMsg(((RemoteUI *)caller)->mb, wrapper.toString);
}

/* ----------------------- ENTRY FUNCTIONS ----------------------- */

static void Entry_connectScreen(RemoteUI * this) {
    memset(this->myIP, 0, sizeof(this->myIP));
}
static void Entry_mainScreen(RemoteUI * this) {

}
static void Entry_logScreen(RemoteUI * this) {
    WatchdogStart(this->wd);
}

/* ----------------------- AFTER FUNCTIONS ----------------------- */

static void After_logScreen(RemoteUI * this) {
    TRACE("[%s] AFTER - LOG_SCREEN\n", this->nameTask)
    WatchdogCancel(this->wd);
}

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(RemoteUI * this) {
    TRACE("[%s] ACTION - Nop\n", this->nameTask)
}

/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(RemoteUI * this) {
    TRACE("[%s] ACTION - Kill\n", this->nameTask)
    this->state = S_DEATH;
}

static void ActionInit(RemoteUI * this) {
    TRACE("[%s] ACTION - Init\n", this->nameTask)
    Entry_connectScreen(this);
}

static void ActionSetIp(RemoteUI * this) {
    TRACE("[%s] ACTION - setIP\n", this->nameTask)
    strcpy(this->myIP, this->msg.ip);
}

static void ActionConnect(RemoteUI * this) {
    TRACE("[%s] ACTION - connect\n", this->nameTask)
    // TODO Faire la tentative de connection ici
}

static void ActionConnectSuccess(RemoteUI * this) {
    TRACE("[%s] ACTION - connect success\n", this->nameTask)
    this->previousEventNumber = 0;
    this->currentEventNumber = 0;
    Entry_mainScreen(this);
}

static void ActionConnectFailure(RemoteUI * this) {
    TRACE("[%s] ACTION - connect failure\n", this->nameTask)
    // TODO displaySreen(ERROR_SCREEN)
}

static void ActionSetDir(RemoteUI * this) {
    TRACE("[%s] ACTION - setDir\n", this->nameTask)
    this->vel = translateDir(this->msg.dir);
    Pilot_SetVelocity(this->vel);
}

static void ActionToggleES(RemoteUI * this) {
    TRACE("[%s] ACTION - toggleES\n", this->nameTask)
    Pilot_toggleES();
}

static void ActionQuit(RemoteUI * this) {
    TRACE("[%s] ACTION - quit\n", this->nameTask)
    // TODO à implémenter
}

/* ----------------------- OTHER FUNCTIONS ----------------------- */

extern void RemoteUI_setEvents(Liste * liste){
    ListeChainee_reset(myEvents);
    myEvents = liste;
}

extern void RemoteUI_setEventsCount(int nbEvents){
    myNbEvents = nbEvents;
}


/* ----------------------- RUN FUNCTION ----------------------- */

/**
 * @brief Main running function of the RemoteUI class
 */
static void RemoteUI_run(RemoteUI * this) {
    ACTION action;
    STATE state;
    Wrapper wrapper;

    TRACE("[%s] RUN\n", this->nameTask)

    while (this->state != S_DEATH) {
        mailboxReceive(this->mb, wrapper.toString); ///< Receiving an EVENT from the mailbox

        if (wrapper.data.event == E_KILL) { // If we received the stop EVENT, we do nothing and we change the STATE to death.
            this->state = S_DEATH;
        } else {
            action = stateMachine[this->state][wrapper.data.event].action;

            TRACE("\t [%s] | Action %s\n", this->nameTask, ACTION_toString[action])

            state = stateMachine[this->state][wrapper.data.event].nextState;
            TRACE("\t [%s] | State %s\n", this->nameTask, STATE_toString[state])

            if (state != S_FORGET) {
                this->msg = wrapper.data;
                actionPtr[action](this);
                this->state = state;
            }
        }
    }
}


/* ------------- NEW START STOP FREE -------------*/

/**
 * initialize in memory RemoteUI
 */
extern RemoteUI * RemoteUI_new() {
    remoteUIcounter++;
    TRACE("[RemoteUI] NEW \n")
    RemoteUI * this = (RemoteUI *) malloc(sizeof(RemoteUI));
    this->mb = mailboxInit("RemoteUI", remoteUIcounter, sizeof(Msg));
    this->wd = WatchdogConstruct(1000, &Wd_timeout, this);
    myEvents = ListeChainee_init();
    sprintf(this->nameTask, NAME_TASK, remoteUIcounter);
}

/**
 * Start RemoteUI and waits for the user's input
 * until the user ask to quit
 */
extern int RemoteUI_start(RemoteUI * this)
{
    TRACE("[RemoteUI] start function \n")
    printf("%s", get_msg(MSG_START));
    int err = pthread_create(&(this->threadId), NULL, (void *) RemoteUI_run, this);

    return 0; // TODO: Handle the errors
}

/**
 * Stop RemoteUI
 */
extern int RemoteUI_stop(RemoteUI * this) {
    // TODO : stop the object with it particularities
    Wrapper wrapper;
    wrapper.data.event = E_KILL;
    WatchdogCancel(this->wd);
    mailboxSendStop(this->mb, wrapper.toString);

    int err = pthread_join(this->threadId, NULL);
    STOP_ON_ERROR(err != 0, "Error when waiting for the thread to end")

    return 0; // TODO: Handle the errors
}

/**
 * destruct the RemoteUI from memory
 */
extern int RemoteUI_free(RemoteUI * this) {
    WatchdogDestroy(this->wd);
    mailboxClose(this->mb);
    free(this);
    return 0; // TODO: Handle the errors
}




