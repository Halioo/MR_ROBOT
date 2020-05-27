/**
 * @file ihm.c
 *
 * @brief This is an ihm of an active class
 *
 * @date April 2020
 *
 * @authors Thomas CRAVIC, Nathan LE GRANVALLET, Clément PUYBAREAU, Louis FROGER
 *
 * @version 1.0
 *
 * @copyright CCBY 4.0
 */

#include <pthread.h>
#include "../../lib/include/mailbox.h"
#include <ncurses.h>

#include "../../lib/include/util.h"
#include "ihm.h"
#include "../../lib/include/remoteui.h"


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
    S_FORGET,
    S_IDLE,
    S_CONNECT,
    S_ERROR,
    S_MAIN,
    S_LOG,
    S_DEATH
)


/**
 * @brief Enumaration of all the possible ACTIONs called by the STATE machine
 */
ENUM_DECL(ACTION,
    A_NOP,                      ///< Nothing happens
    A_DISPLAY_CONNECT,
    A_SETIP,
    A_KILL                      ///< Kills the STATE machine
)


/**
 * @brief Enumeration of all the possible EVENTs that triggers the STATE machine
 */
ENUM_DECL(EVENT,
    E_NOP,
    E_DISPLAY_CONNECT,
    E_DISPLAY_ERROR,
    E_DISPLAY_MAIN,
    E_DISPLAY_LOG,
    E_INPUT,
    E_VALIDATE,
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

    RemoteUI * remoteUi;
};


/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/**
 * Retourne le string correspondant au type de message
 * passé en paramètre
 */
static const char * get_appMsg(TYPES_MSG type_msg) {
    return appMsg[type_msg][LANG];
}


/*------------- ACTION functions -------------*/
// TODO : put here all the ACTION functions prototypes
/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(Ihm * this);
/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(Ihm * this);
static void ActionDisplayConnect(Ihm * this);


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
        &ActionDisplayConnect,
        &ActionKill
};


/**
 * @brief STATE machine of the Ihm class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = { // TODO : fill the STATE machine
    [S_IDLE][E_DISPLAY_CONNECT] = {S_CONNECT, A_DISPLAY_CONNECT},
    //[S_CONNECT][E_VALIDATE]= {S_CONNECT, A_SETIP}

    /**
    [S_CONNECT][E_DISPLAY_ERROR] = {S_ERROR, A_DISPLAY_ERROR},
    [S_CONNECT][E_DISPLAY_MAIN] = {S_MAIN, A_DISPLAY_MAIN},
    [S_ERROR][E_VALIDATE] = {S_CONNECT, A_DISPLAY_CONNECT},
    [S_MAIN][E_INPUT] = {S_MAIN, A_HANDLE_INPUT},
    [S_MAIN][E_DISPLAY_LOG] = {S_LOG, A_DISPLAY_LOG},
    [S_LOG][E_DISPLAY_LOG] = {S_LOG, A_DISPLAY_LOG},
    [S_LOG][E_INPUT]
    [S_LOG][E_DISPLAY_MAIN] = {S_MAIN, A_DISPLAY_MAIN},
     */
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


static void ActionDisplayConnect(Ihm * this) {
    TRACE("[%s] - Action displayConnect\n", this->nameTask)

    clear();
    wprintw(wTitle, "CONNECT_SCREEN");
    mvprintw(2, 2, get_appMsg(MSG_WAITING_IP));
    mvprintw(8, 4, get_appMsg(MSG_VALIDATE));

    int height = 3; int width = 30;
    int y = 5, x = 1;
    WINDOW * winInput = newwin(height, width, y, x);
    box(winInput, 0, 0);

    move(y+1, x+1);
    wrefresh(winInput);
    refresh();

    char ip[15];
    getnstr(ip, 15);
    RemoteUI_setIp(this->remoteUi, ip);
}

static void ActionDisplayError(Ihm * this) {
    TRACE("[%s] - Action displayError\n", this->nameTask)

    clear();
    wprintw(wTitle, "ERROR_SCREEN");
    mvprintw(2,2, get_appMsg(MSG_INVALID_IP));
    mvprintw(2,2,get_appMsg(MSG_VALIDATE));
    int c;
    do {c = getch();} while (c != '\n');
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


extern void IhmDisplayConnect(Ihm * this) {
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_CONNECT;
    mailboxSendMsg(this->mb, wrapper.toString);
}

void IhmValidate(Ihm * this) {
    Msg msg;

    Wrapper wrapper;
    wrapper.data = msg;

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

Ihm * IhmNew(RemoteUI * remoteUi) {
    ihmCounter ++; ///< Incrementing the instances counter.
    TRACE("IhmNew function \n")
    Ihm * this = (Ihm *) malloc(sizeof(Ihm));
    this->mb = mailboxInit("Ihm", ihmCounter, sizeof(Msg));
    int err = sprintf(this->nameTask, NAME_TASK, ihmCounter);
    STOP_ON_ERROR(err < 0, "Error when setting the tasks name.")

    this->remoteUi = remoteUi;

    initScreen();

    return this; // TODO: Handle the errors
}


int IhmStart(Ihm * this) {
    // TODO : start the object with it particularities
    TRACE("IhmStart function \n")
    int err = pthread_create(&(this->threadId), NULL, (void *) IhmRun, this);
    STOP_ON_ERROR(err != 0, "Error when creating the thread")

    IhmDisplayConnect(this);

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

