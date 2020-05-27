//
// Created by gwendal on 27/05/2020.
//

#include "ihm.h"
#include "adminui.h"
#include <pthread.h>
#include "../../lib/include/mailbox.h"
#include <ncurses.h>


/**
 * @brief Ihm instances counter used to have a unique queuename per thread
 */
static int ihmCounter = 0;

/* ----------------------- MAILBOX DEFINITIONS -----------------------*/

/**
 * @def Name of the task. Each instance will have this name,
 * followed by the number of the instance
 */
#define NAME_TASK "IhmTask%d"

/**
 * @def Size of a task name
 */
#define SIZE_TASK_NAME 20


/*----------------------- TYPE DEFINITIONS -----------------------*/

/**
 * @brief Enumeration of all the STATEs that can be taken by the STATE machine
 */
ENUM_DECL(STATE,
          S_IDLE,
          S_MAIN_SCREEN,
          S_LOG_SCREEN,
          S_FORGET,
          S_DEATH
)


/**
 * @brief Enumaration of all the possible ACTIONs called by the STATE machine
 */
ENUM_DECL(ACTION,
          A_NOP,                      ///< Nothing happens
          A_STARTPOLLING,
          A_STOPPOLLING,
          A_KILL                      ///< Kills the STATE machine
)


/**
 * @brief Enumeration of all the possible EVENTs that triggers the STATE machine
 */
ENUM_DECL(EVENT,
          E_NOP,
          E_DISPLAY_MAIN_SCREEN,
          E_DISPLAY_LOG_SCREEN,
          E_QUIT,
          E_KILL
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
 * @brief Structure of the Ihm object
 */
struct Ihm_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    STATE state;        ///< Actual STATE of the STATE machine
    Msg msg;            ///< Structure used to pass parameters to the functions pointer.
    char nameTask[SIZE_TASK_NAME]; ///< Name of the task
    Mailbox * mb;

    AdminUI * adminUi;
    Logger *logger;
};

/*------------- ACTION functions -------------*/
// TODO : put here all the ACTION functions prototypes
/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(Ihm * this);

static void ActionStartPolling(Ihm *this);

static void ActionStopPolling(Ihm *this);

/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(Ihm * this);

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Function pointer used to call the ACTIONs of the STATE machine.
 */
typedef void (*ActionPtr)(Ihm*);

/**
 * @brief Function pointer array used to call the ACTIONs of the STATE machine.
 */
static const ActionPtr actionPtr[NB_ACTION] = { // TODO : add all the function pointers corresponding to the ACTION enum in the right order.
        &ActionNop,
        &ActionStartPolling,
        &ActionStopPolling,
        &ActionKill
};


/**
 * @brief STATE machine of the Ihm class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = { // TODO : fill the STATE machine

        [S_IDLE][E_DISPLAY_MAIN_SCREEN]={S_MAIN_SCREEN,A_STARTPOLLING},
        [S_MAIN_SCREEN][E_DISPLAY_LOG_SCREEN]={S_LOG_SCREEN,A_NOP},
        [S_LOG_SCREEN][E_DISPLAY_MAIN_SCREEN]={S_MAIN_SCREEN,A_NOP},
        [S_MAIN_SCREEN][E_QUIT]={S_DEATH,A_STOPPOLLING},
        [S_LOG_SCREEN][E_QUIT]={S_DEATH,A_STOPPOLLING}

};

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

// TODO : Write all the ACTION functions

WINDOW * wTitle;
WINDOW * wBottom;

int xBeg, yBeg, xMax, yMax;

static void initScreen() {
    initscr();
    getbegyx(stdscr, yBeg, xBeg);
    getmaxyx(stdscr, yMax, xMax);
    wTitle = newwin(yBeg+3, xMax-2, yBeg+1, xBeg+1);
    box(wTitle, 0, 0);
    wBottom = newwin(yBeg+3, xMax-2, yMax-4, xBeg+1);
    box(wBottom, 0, 0);
    refresh();
}

static void ActionStartPolling(Ihm *this){
    this->logger=Logger_new();
    Logger_start(this->logger);
    Logger_startPolling(this->logger);
}

static void ActionStopPolling(Ihm *this){
    Logger_stopPolling(this->logger);
    Logger_stop(this->logger);
    Logger_free(this->logger);
}

static void ActionNop(Ihm * this) {
    TRACE("[%s] - ActionNop\n", this->nameTask)
}

static void ActionKill(Ihm * this) {
    TRACE("[%s] - ActionKill\n", this->nameTask)
    this->state = S_DEATH;
}

/*----------------------- EVENT FUNCTIONS -----------------------*/
// TODO : write the events functions


extern void IhmDisplayMainScreen(Ihm * this) {
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_MAIN_SCREEN;
    mailboxSendMsg(this->mb, wrapper.toString);
}

extern void IhmDisplayLogScreen(Ihm * this) {
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_LOG_SCREEN;
    mailboxSendMsg(this->mb, wrapper.toString);
}

/* ----------------------- RUN FUNCTION ----------------------- */

/**
 * @brief Main running function of the Ihm class
 */
static void IhmRun(Ihm * this) {
    ACTION action;
    STATE state;
    Wrapper wrapper;

    this->state = S_IDLE;

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

/* ----------------------- NEW START STOP FREE -----------------------*/

Ihm * IhmNew(AdminUI * adminUi) {
    ihmCounter ++; ///< Incrementing the instances counter.
    TRACE("IhmNew function \n")
    Ihm * this = (Ihm *) malloc(sizeof(Ihm));
    this->mb = mailboxInit("Ihm", ihmCounter, sizeof(Msg));
    int err = sprintf(this->nameTask, NAME_TASK, ihmCounter);
    STOP_ON_ERROR(err < 0, "Error when setting the tasks name.")

    this->adminUi = adminUi;

    initScreen();

    return this; // TODO: Handle the errors
}


int IhmStart(Ihm * this) {
    // TODO : start the object with it particularities
    TRACE("IhmStart function \n")
    int err = pthread_create(&(this->threadId), NULL, (void *) IhmRun, this);
    STOP_ON_ERROR(err != 0, "Error when creating the thread")

    IhmDisplayMainScreen(this);

    return 0; // TODO: Handle the errors
}


int IhmStop(Ihm * this) {
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


int IhmFree(Ihm * this) {
    // TODO : free the object with it particularities
    TRACE("IhmFree function \n")

    endwin();

    mailboxClose(this->mb);

    free(this);

    return 0; // TODO: Handle the errors
}


