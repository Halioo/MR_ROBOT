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
#include <mailbox.h>
#include <ncurses.h>

#include "util.h"
#include "ihm.h"
#include "remoteUI.h"

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
          A_DISPLAY_ERROR,
          A_DISPLAY_MAIN,
          A_DISPLAY_LOG,
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


pthread_t threadId; ///< Pthread identifier for the active function of the class.
pthread_t secondaryThread;
STATE state = S_IDLE;        ///< Actual STATE of the STATE machine
Msg msg;            ///< Structure used to pass parameters to the functions pointer.
char nameTask[SIZE_TASK_NAME]; ///< Name of the task
Mailbox * mb;

RemoteUI * remoteUi;



/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/**
 * Retourne le string correspondant au type de message
 * passé en paramètre
 */
static const char * get_appMsg(TYPES_MSG type_msg) {
    return appMsg[type_msg][LANG];
}

/**
 * Structure d'une commande, une commande est associée à :
 * key: une touche du clavier
 * msg: un message à afficher dans la console
 * command: une commande
 */
typedef struct {
    char key;
    TYPES_MSG msg;
    COMMAND command;
} Command;

// Array regroupant les différentes commandes possibles
static Command list_commands[] = {
        {'q', MSG_COMMAND_LEFT,  C_LEFT},
        {'d', MSG_COMMAND_RIGHT, C_RIGHT},
        {'z', MSG_COMMAND_FWD,   C_FORWARD},
        {'s', MSG_COMMAND_BCKWD, C_BACKWARD},
        {' ', MSG_COMMAND_STOP,  C_STOP},
        {'r', MSG_COMMAND_STATE, C_STATE},
        {'e', MSG_COMMAND_LOGS,  C_LOGS}
};
// Calcul du nombre de commandes possibles (la commande pour quitter est exclue)
static const int command_number = sizeof(list_commands) / sizeof(list_commands[0]);
/**
 * Cherche l'id de la commande correspondant à
 * la touche passée en paramètre
 *
 * @param elem
 * @return l'id de la commande, -1 si
 * la touche ne correspond à aucun élément
 */
static int get_id(char elem) {
    int id = -1;
    for (int i=0; i < command_number; i++) {
        if (list_commands[i].key == elem) {
            id = i;
        }
    }
    return id;
}


/*------------- ACTION functions -------------*/
// TODO : put here all the ACTION functions prototypes
/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop();
/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill();
static void ActionDisplayConnect();
static void ActionDisplayMain();
static void ActionDisplayError();
static void ActionDisplayLog();


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
        &ActionDisplayConnect,
        &ActionDisplayError,
        &ActionDisplayMain,
        &ActionDisplayLog,
        &ActionKill
};


/**
 * @brief STATE machine of the Ihm class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = { // TODO : fill the STATE machine
        [S_IDLE][E_DISPLAY_CONNECT] = {S_CONNECT, A_DISPLAY_CONNECT},
        [S_CONNECT][E_DISPLAY_ERROR] = {S_ERROR, A_DISPLAY_ERROR},
        [S_CONNECT][E_DISPLAY_MAIN] = {S_MAIN, A_DISPLAY_MAIN},
        [S_ERROR][E_DISPLAY_CONNECT] = {S_CONNECT, A_DISPLAY_CONNECT},
        [S_MAIN][E_DISPLAY_LOG] = {S_LOG, A_DISPLAY_LOG},
        [S_LOG][E_DISPLAY_LOG] = {S_LOG, A_DISPLAY_LOG},
        [S_ERROR][E_KILL] = {S_DEATH, A_KILL},
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


static void ActionDisplayConnect() {
    TRACE("[%s] - Action displayConnect\n", nameTask)

    clear();

    //wprintw(wTitle, "CONNECT_SCREEN");
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
    RemoteUI_setIp(remoteUi, ip);
}

static void ActionDisplayError() {
    TRACE("[%s] - Action displayError\n", nameTask)

    clear();
    noecho();
    wprintw(wTitle, "ERROR_SCREEN");
    mvprintw(2,2, get_appMsg(MSG_INVALID_IP));
    mvprintw(4,2,get_appMsg(MSG_VALIDATE));
    mvprintw(6,2,get_appMsg(MSG_COMMAND_QUIT));
    int c = 0;
    while (c != '\n' && c != 'a') {
        c = getch();
        TRACE("CHAR = %c\n", c)
        if (c == '\n') {
            RemoteUI_validate(remoteUi);
        } else if (c == 'a') {
            RemoteUI_quit(remoteUi);
        }
    }
    TRACE("LE THREAD A FINI NJBGEABHFEVBHLFBEHJZ\n")
}

static void ActionDisplayMain() {
    TRACE("[%s] - Action displayMain\n", nameTask)

    clear();
    wprintw(wTitle, "MAIN_SCREEN");
    mvprintw(2,2, get_appMsg(MSG_COMMANDS));

    int c = 0;
    while (c != 'r' && c != 'a') {
        c = getch();
        int id = get_id((char)c);
        if (id == -1) {
            mvprintw(15, 2, "%s", get_appMsg(MSG_UNKNOWN_COMMAND));
        } else {
            Command command = list_commands[id];
            mvprintw(15, 2, "%s", get_appMsg(command.msg));
            switch (command.command) {
                case C_LEFT:
                    RemoteUI_setDir(remoteUi, LEFT);
                    break;
                case C_RIGHT:
                    RemoteUI_setDir(remoteUi, RIGHT);
                    break;
                case C_FORWARD:
                    RemoteUI_setDir(remoteUi, FORWARD);
                    break;
                case C_BACKWARD:
                    RemoteUI_setDir(remoteUi, BACKWARD);
                    break;
                case C_STOP:
                    RemoteUI_toggleEmergencyStop(remoteUi);
                    break;
                case C_LOGS:
                    clear();
                    mvprintw(2,2, get_appMsg(MSG_COMMANDS));
                    refresh();
                    break;
                case C_STATE:
                    RemoteUI_goScreenLog(remoteUi);
                    break;
                default: break;
            }
        }
    }
    TRACE("LE THREAD A FINI NJBGEABHFEVBHLFBEHJZ\n")
}
static void ActionDisplayLog() {}

static void ActionNop() {
    TRACE("[%s] - ActionNop\n", nameTask)
}
static void ActionKill() {
    TRACE("[%s] - ActionKill\n", nameTask)
    endwin();
    state = S_DEATH;
}


/*----------------------- EVIhmStop(): Waiting for the thread to terminateENT FUNCTIONS -----------------------*/
// TODO : write the events functions


extern void IhmDisplayConnect() {
    TRACE("DISPLAY CONNECT\n")
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_CONNECT;
    mailboxSendMsg(mb, wrapper.toString);
}

extern void IhmDisplayError() {
    TRACE("DISPLAY ERROR\n")
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_ERROR;
    mailboxSendMsg(mb, wrapper.toString);
}

extern void IhmDisplayMain() {
    TRACE("DISPLAY MAIN\n")
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_MAIN;
    mailboxSendMsg(mb, wrapper.toString);
}

extern void IhmDisplayLog() {
    TRACE("DISPLAY LOG\n")
    Wrapper wrapper;
    wrapper.data.event = E_DISPLAY_LOG;
    mailboxSendMsg(mb, wrapper.toString);
}

extern void Ihm_displayScreen(UI_SCREEN screen) {
    switch (screen) {
        case SCREEN_CONNECT:
            IhmDisplayConnect();
            break;
        case SCREEN_ERROR:
            IhmDisplayError();
            break;
        case SCREEN_MAIN:
            IhmDisplayMain();
            break;
        case SCREEN_LOG:
            IhmDisplayLog();
            break;
        default: break;
    }
}



/* ----------------------- RUN FUNCTION ----------------------- */

/**
 * @brief Main running function of the Ihm class
 */
static void IhmRun() {
    TRACE("RUN\n")
    ACTION action;
    STATE next_state;
    Wrapper wrapper;

    initScreen();

    while (state != S_DEATH) {
        mailboxReceive(mb, wrapper.toString); ///< Receiving an EVENT from the mailbox

        if (wrapper.data.event == E_KILL) { // If we received the stop EVENT, we do nothing and we change the STATE to death.
            next_state = S_DEATH;

        } else {
            action = stateMachine[state][wrapper.data.event].action;

            TRACE("Action %s\n", ACTION_toString[action])

            next_state = stateMachine[state][wrapper.data.event].nextState;
            TRACE("State %s\n", STATE_toString[state])

            if (state != S_FORGET) {
                msg = wrapper.data;
                TRACE("thread secondaire initialized\n");
                pthread_create(&(secondaryThread), NULL, (void *) actionPtr[action], NULL);
                state = next_state;
                pthread_join(secondaryThread, NULL);
                TRACE("thread secondaire recovered\n");
            }
        }
    }
}

/* ----------------------- NEW START STOP FREE -----------------------*/

int IhmNew(RemoteUI * remoteUi_ext) {
    ihmCounter ++; ///< Incrementing the instances counter.
    TRACE("IhmNew function \n")
    mb = mailboxInit("Ihm", ihmCounter, sizeof(Msg));
    int err = sprintf(nameTask, NAME_TASK, ihmCounter);
    STOP_ON_ERROR(err < 0, "Error when setting the tasks name.")

    remoteUi = remoteUi_ext;

    initScreen();

    return 0; // TODO: Handle the errors
}


int IhmStart() {
    // TODO : start the object with it particularities
    TRACE("IhmStart function \n")
    int err = pthread_create(&(threadId), NULL, (void *) IhmRun, NULL);
    STOP_ON_ERROR(err != 0, "Error when creating the thread")

    Ihm_displayScreen(SCREEN_CONNECT);

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

    mailboxClose(mb);


    return 0; // TODO: Handle the errors
}

