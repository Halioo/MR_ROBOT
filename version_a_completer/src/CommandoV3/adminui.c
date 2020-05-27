
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "adminui.h"
#include "robocom.h"
#include "mailbox.h"
#include "logger.h"
#include "pilot.h"

static int adminUIcounter = 0;
static Liste * myEvents;

/**
 * @def Name of the task. Each instance will have this name,
 * followed by the number of the instance
 */
#define NAME_TASK "AdminUITask%d"

/**
 * @def Size of a task name
 */
#define SIZE_TASK_NAME (20)

/* ---------- TYPE DEFINITIONS ----------*/

/**
 * @brief Enumeration of all the states that can be taken by the state machine
 */
ENUM_DECL(STATE,
        S_FORGET,      ///< Nothing happens
        S_MAIN_SCREEN,
        S_LOG_SCREEN,
        S_DEATH
)

/**
 * @brief Enumeration of all the possible events that triggers the state machine
 */
ENUM_DECL(EVENT,
        E_NOP,  ///< Do nothing
        E_GO_SCREEN_LOG,
        E_BACK_MAIN_SCREEN,
        E_TOGGLE_ES,
        E_CLEAR_LOG,
        E_TO_LOG,
        E_QUIT,
        E_KILL    ///< Kills the STATE machine
)

/**
 * @brief Enumaration of all the possible actions called by the state machine
 */
ENUM_DECL(ACTION,
        A_NOP,  ///< Nothing happens
        A_GO_SCREEN_LOG,
        A_BACK_MAIN_SCREEN,
        A_CLEAR_LOG,
        A_TO_LOG,
        A_TOGGLE_ES,
        A_QUIT,
        A_KILL  ///< Kills the state machine
)

ENUM_DECL(SCREEN,
        MAIN_SCREEN,
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
} Msg;

/**
 * @brief Wrapper enum. It is used to send EVENTs and parameters in a mailBox.
 */
wrapperOf(Msg)


/**
* @brief Structure of the AdminUI object
*/
struct AdminUI_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    STATE state;        ///< Actual STATE of the STATE machine
    Msg msg;            ///< Structure used to pass parameters to the functions pointer.

    char nameTask[SIZE_TASK_NAME]; ///< Name of the task
    Mailbox * mailbox;
    Watchdog * watchdogLog;
    Pilot * myPilot;
    Logger * myLogger;
    char myEvents[1];

    int previousEventNumber;
    int currentEventNumber;
};


/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/*------------- EVENT functions -------------*/

static void AdminUI_TOLog(AdminUI * this);



/*------------- ACTION functions -------------*/

/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(AdminUI * this);

static void ActionGoScreenLog(AdminUI * this);
static void ActionBackMainScreen(AdminUI * this);
static void ActionClearLog(AdminUI * this);
static void ActionTOLog(AdminUI * this);
static void ActionToggleES(AdminUI * this);
static void ActionQuit(AdminUI * this);

/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(AdminUI * this);

/*------------- OTHER functions -------------*/

static void updateEvents(); // TODO : compléter une fois que les Events seront gérés

static void displayScreen(SCREEN screen);

static void setTO(AdminUI * this);

static void resetTO(AdminUI * this);

static void AdminUI_TOHandle(void * this);

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Function pointer used to call the ACTIONs of the STATE machine.
 */
typedef void (*ActionPtr)(AdminUI *);

/**
 * @brief Function pointer array used to call the ACTIONs of the STATE machine.
 */
static const ActionPtr actionPtr[NB_ACTION] = { // TODO : add all the function pointers corresponding to the ACTION enum in the right order.
        &ActionNop,
        &ActionGoScreenLog,
        &ActionBackMainScreen,
        &ActionClearLog,
        &ActionTOLog,
        &ActionToggleES,
        &ActionQuit,
        &ActionKill
};


/**
 * @brief STATE machine of the Example class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = {
        [S_MAIN_SCREEN][E_TOGGLE_ES] = {S_MAIN_SCREEN,A_TOGGLE_ES},
        [S_MAIN_SCREEN][E_QUIT] = {S_DEATH,A_QUIT},
        [S_MAIN_SCREEN][E_GO_SCREEN_LOG] = {S_LOG_SCREEN,A_GO_SCREEN_LOG},
        [S_LOG_SCREEN][E_BACK_MAIN_SCREEN] = {S_MAIN_SCREEN, },
        [S_LOG_SCREEN][E_TOGGLE_ES] = {S_LOG_SCREEN,A_TOGGLE_ES},
        [S_LOG_SCREEN][E_CLEAR_LOG] = {S_LOG_SCREEN,A_CLEAR_LOG},
        [S_LOG_SCREEN][E_TO_LOG] = {S_LOG_SCREEN,A_TO_LOG},
        [S_LOG_SCREEN][E_QUIT] = {S_DEATH,A_QUIT}

};


/* ----------------------- EVENT FUNCTIONS ----------------------- */

extern void AdminUI_ToggleES(AdminUI * this){
    Wrapper wrapper;
    wrapper.data.event = E_TOGGLE_ES;
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void AdminUI_Quit(AdminUI * this){
    Wrapper wrapper;
    wrapper.data.event = E_QUIT;
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void AdminUI_GoScreenLog(AdminUI * this){
    Wrapper wrapper;
    wrapper.data.event = E_GO_SCREEN_LOG;
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void AdminUI_BackMainScreen(AdminUI * this){
    Wrapper wrapper;
    wrapper.data.event = E_BACK_MAIN_SCREEN;
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void AdminUI_ClearLog(AdminUI * this){
    Wrapper wrapper;
    wrapper.data.event = E_CLEAR_LOG;
    mailboxSendMsg(this->mailbox,wrapper.toString);
}


/* ----------------------- AFTER FUNCTIONS ----------------------- */

static void AdminUI_TOLog(AdminUI * this){
    Wrapper wrapper;
    wrapper.data.event = E_TO_LOG;
    mailboxSendMsg(this->mailbox,wrapper.toString);
}


/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(AdminUI * this) {
    TRACE("[%s] ACTION - Nop\n", this->nameTask)
}

/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(AdminUI * this) {
    TRACE("[%s] ACTION - Kill\n", this->nameTask)
    this->state = S_DEATH;
}

static void ActionGoScreenLog(AdminUI * this){
    updateEvents();
    displayScreen(LOG_SCREEN);
    setTO(this);
}

static void ActionBackMainScreen(AdminUI * this){
    displayScreen(MAIN_SCREEN);
    resetTO(this);
}

static void ActionClearLog(AdminUI * this){
    Logger_clearEvents(this->myLogger);
    this->currentEventNumber = 0;
    this->previousEventNumber = 0;
}

static void ActionToggleES(AdminUI * this){
    Pilot_ToggleES(this->myPilot);
}

static void ActionTOLog(AdminUI * this){
    updateEvents();
    displayScreen(LOG_SCREEN);
    setTO(this);
}

static void ActionQuit(AdminUI * this){
//    Logger_stopPolling(this->myLogger);
}


/* ----------------------- RUN FUNCTION ----------------------- */

/**
 * @brief Main running function of the AdminUI class
 */
static void AdminUI_run(AdminUI * this) {
    ACTION action;
    STATE state;
    Wrapper wrapper;

    TRACE("[%s] RUN\n", this->nameTask)

    while (this->state != S_DEATH) {
        mailboxReceive(this->mailbox, wrapper.toString); ///< Receiving an EVENT from the mailbox

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
 * initialize in memory AdminUI
 */
extern AdminUI * AdminUI_new(Pilot * pilot, Logger * logger) {
    adminUIcounter++;
    TRACE("[AdminUI] NEW \n")
    AdminUI * this = (AdminUI *) malloc(sizeof(AdminUI));
    this->mailbox = mailboxInit("AdminUI", adminUIcounter, sizeof(Msg));
    this->watchdogLog = WatchdogConstruct(1000, &AdminUI_TOHandle, this);
    sprintf(this->nameTask, NAME_TASK, adminUIcounter);
    this->state = S_MAIN_SCREEN;

    this->myPilot = pilot;
    this->myLogger = logger;

    // TODO: Handle the errors
    return this;
}

/**
 * Start AdminUI and waits for the user's input
 * until the user ask to quit
 */
extern int AdminUI_start(AdminUI * this)
{
    TRACE("[AdminUI] start function \n")
//    printf("%s", get_msg(MSG_START));
    int err = pthread_create(&(this->threadId), NULL, (void *) AdminUI_run, this);

    // TODO: Handle the errors
    return 0;
}

/**
 * Stop AdminUI
 */
extern int AdminUI_stop(AdminUI * this) {
    // TODO : stop the object with it particularities
    Wrapper wrapper;
    wrapper.data.event = E_KILL;
    WatchdogCancel(this->watchdogLog);
    mailboxSendStop(this->mailbox, wrapper.toString);

    int err = pthread_join(this->threadId, NULL);
    STOP_ON_ERROR(err != 0, "Error when waiting for the thread to end")

    return 0; // TODO: Handle the errors
}

/**
 * destruct the AdminUI from memory
 */
extern int AdminUI_free(AdminUI * this) {
    WatchdogDestroy(this->watchdogLog);
    mailboxClose(this->mailbox);
    free(this);

    return 0; // TODO: Handle the errors
}

/* ----------------------- OTHER FUNCTIONS -----------------------*/

static void displayScreen(SCREEN screen){
    // TODO
}

static void setTO(AdminUI * this){
    WatchdogStart(this->watchdogLog);
}

static void resetTO(AdminUI * this){
    WatchdogCancel(this->watchdogLog);
}

static void AdminUI_TOHandle(void * this){

    Wrapper wrapper = {
            .data.event = E_TO_LOG
    };
    mailboxSendMsg(((AdminUI*)this)->mailbox,wrapper.toString);
}

static void updateEvents(AdminUI * this){
    Logger_askEventsCount(myEvents);
    // TODO
}









