//
// Created by gwendal on 27/05/2020.
//

#include "ihm.h"
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
          //A_STARTPOLLING,
          //A_STOPPOLLING,
          A_DISPLAY_MAIN_SCREEN,
          A_DISPLAY_LOG_SCREEN,
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

pthread_t threadId; ///< Pthread identifier for the active function of the class.
pthread_t secondaryThread;
STATE state;        ///< Actual STATE of the STATE machine
Msg msg;            ///< Structure used to pass parameters to the functions pointer.
char nameTask[SIZE_TASK_NAME]; ///< Name of the task
Mailbox * mb;

AdminUI * adminUi;
Logger *logger;


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
static void ActionNop();


static void ActionDisplayMainScreen();

static void ActionDisplayLogScreen();



/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill();

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Function pointer used to call the ACTIONs of the STATE machine.
 */
typedef void (*ActionPtr)(void);

/**
 * @brief Function pointer array used to call the ACTIONs of the STATE machine.
 */
static const ActionPtr actionPtr[NB_ACTION] = { // TODO : add all the function pointers corresponding to the ACTION enum in the right order.
        &ActionNop,
        &ActionDisplayMainScreen,
        &ActionDisplayLogScreen,
        &ActionKill
};


/**
 * @brief STATE machine of the Ihm class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = { // TODO : fill the STATE machine

        /*[S_IDLE][E_DISPLAY_MAIN_SCREEN]={S_MAIN_SCREEN,A_STARTPOLLING},
        [S_MAIN_SCREEN][E_DISPLAY_LOG_SCREEN]={S_LOG_SCREEN,A_NOP},
        [S_LOG_SCREEN][E_DISPLAY_MAIN_SCREEN]={S_MAIN_SCREEN,A_NOP},
        [S_MAIN_SCREEN][E_QUIT]={S_DEATH,A_STOPPOLLING},
        [S_LOG_SCREEN][E_QUIT]={S_DEATH,A_STOPPOLLING}*/

        [S_IDLE][E_DISPLAY_MAIN_SCREEN]={S_MAIN_SCREEN,A_DISPLAY_MAIN_SCREEN},
        [S_MAIN_SCREEN][E_DISPLAY_LOG_SCREEN]={S_LOG_SCREEN,A_DISPLAY_LOG_SCREEN},
        [S_LOG_SCREEN][E_DISPLAY_MAIN_SCREEN]={S_MAIN_SCREEN,A_DISPLAY_MAIN_SCREEN},
        [S_MAIN_SCREEN][E_QUIT]={S_DEATH,A_KILL},
        [S_LOG_SCREEN][E_QUIT]={S_DEATH,A_KILL}

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

/*static void ActionStartPolling(){
    logger=Logger_new();
    Logger_start(logger);
    Logger_startPolling(logger);
}

static void ActionStopPolling(){
    Logger_stopPolling(logger);
    Logger_stop(logger);
    Logger_free(logger);
}
*/

static void ActionDisplayMainScreen(){
    TRACE("ActionDisplayMainScreen\n")
    clear();
    mvprintw(2, 2, " :stop");
    mvprintw(3, 2, "r: afficher les logs");
    mvprintw(4, 2, "a: quitter");

    noecho();
    int c = 0;

    while (c != 'r' && c != 'a') {
        c = getch();
        switch ((char)c) {
            case 'r':
                AdminUI_GoScreenLog(adminUi);
            case ' ':
                AdminUI_ToggleES(adminUi);
            case 'a':
                AdminUI_Quit(adminUi);
            default: break;
        }
    }
}

static void ActionDisplayLogScreen(){
    TRACE("ActionDisplayLogScreen\n")
    Liste * events = AdminUI_getEvents(adminUi);
    LogEvent event;
    int row = 2;
    clear();
    while(events->premier->indice > INDICE_INITIAL){
        event = events->premier->logEvent;
        mvprintw(row, 2, get_appMsg(MSG_LOGS), event.speed, event.sens.collision_f, event.sens.luminosity);
        events->premier = events->premier->suivant;
        row++;
    }

    refresh();
}

static void ActionNop() {
    TRACE("[%s] - ActionNop\n", nameTask)
}

static void ActionKill() {
    TRACE("[%s] - ActionKill\n", nameTask)
    state = S_DEATH;
}


/*----------------------- EVENT FUNCTIONS -----------------------*/
// TODO : write the events functions


extern void IhmDisplayMainScreen() {
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_MAIN_SCREEN;
    mailboxSendMsg(mb, wrapper.toString);
}

extern void IhmDisplayLogScreen() {
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_LOG_SCREEN;
    mailboxSendMsg(mb, wrapper.toString);
}

extern void IhmQuit(){
    Wrapper wrapper;
    wrapper.data.event = E_QUIT;
    mailboxSendMsg(mb, wrapper.toString);
}

/* ----------------------- RUN FUNCTION ----------------------- */

/**
 * @brief Main running function of the Ihm class
 */
static void IhmRun() {
    ACTION action;
    STATE next_state;
    Wrapper wrapper;

    next_state = S_IDLE;

    while (state != S_DEATH) {
        mailboxReceive(mb, wrapper.toString); ///< Receiving an EVENT from the mailbox

        if (wrapper.data.event == E_KILL) { // If we received the stop EVENT, we do nothing and we change the STATE to death.
            state = S_DEATH;

        } else {
            action = stateMachine[state][wrapper.data.event].action;

            TRACE("Action %s\n", ACTION_toString[action])

            next_state = stateMachine[state][wrapper.data.event].nextState;
            TRACE("State %s\n", STATE_toString[next_state])

            if (next_state != S_FORGET) {
                msg = wrapper.data;
                pthread_create(&(secondaryThread), NULL, (void *) actionPtr[action], NULL);
                state = next_state;
                pthread_join(secondaryThread, NULL);
            }
        }
    }
}

/* ----------------------- NEW START STOP FREE -----------------------*/

IhmNew(AdminUI * adminUi_ext) {
    ihmCounter ++; ///< Incrementing the instances counter.
    TRACE("IhmNew function \n")
    mb = mailboxInit("Ihm", ihmCounter, sizeof(Msg));
    int err = sprintf(nameTask, NAME_TASK, ihmCounter);
    STOP_ON_ERROR(err < 0, "Error when setting the tasks name.")

    adminUi = adminUi_ext;

    initScreen();

    return 0; // TODO: Handle the errors
}


int IhmStart() {
    // TODO : start the object with it particularities
    TRACE("IhmStart function \n")
    int err = pthread_create(&(threadId), NULL, (void *) IhmRun, NULL);
    STOP_ON_ERROR(err != 0, "Error when creating the thread")

    IhmDisplayMainScreen();

    return 0; // TODO: Handle the errors
}


int IhmStop() {
    // TODO : stop the object with it particularities

    Wrapper wrapper;
    wrapper.data.event = E_KILL;

    mailboxSendStop(mb, wrapper.toString);
    TRACE("Waiting for the thread to terminate \n")

    int err = pthread_join(threadId, NULL);
    STOP_ON_ERROR(err != 0, "Error when waiting for the thread to end")

    return 0; // TODO: Handle the errors
}


int IhmFree() {
    // TODO : free the object with it particularities
    TRACE("IhmFree function \n")

    endwin();

    mailboxClose(mb);

    return 0; // TODO: Handle the errors
}


