

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <mailbox.h>
#include <prose.h>

#include "remoteui.h"

//#define LANG FRENCH


static int remoteUIcounter = 0;

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
 * @brief Enumaration of all the possible actions called by the state machine
 */
ENUM_DECL(ACTION,
    A_NOP,  ///< Nothing happens
    A_SETIP,
    A_ENTRY_MAINSCREEN,
    A_ENTRY_LOGSCREEN,
    A_MAINSCREEN_LOGSCREEN,
    A_LOGSCREEN_MAINSCREEN,
    A_LOG_LOOP,
    A_QUIT,
    A_KILL  ///< Kills the state machine
)


/**
 * @brief Enumeration of all the possible events that triggers the state machine
 */
ENUM_DECL(EVENT,
    E_NOP,  ///< Do nothing
    E_ENTRY_CONNECTSCREEN,
    E_ENTRY_MAINSCREEN,
    E_ENTRY_LOGSCREEN,

    E_TO_LOGSCREEN,

    E_PS_CONNECT_SUCCESS,
    E_PS_CONNECT_FAIL,

    E_GO_LOGSCREEN,
    E_BACK_MAINSCREEN,

    E_SETIP,
    E_SETDIR,
    E_VALIDATE,
    E_TOGGLE_ES,
    E_QUIT,
    E_KILL    ///< Kills the STATE machine
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
    char kin;
} Msg;

/**
 * @brief Wrapper enum. It is used to send EVENTs and parameters in a mailBox.
 */
typedef union {
    Msg msg; ///< Message sent, interpreted as a structure
    char * toString; ///< Message sent, interpreted as a char array
} Wrapper;


/**
* @brief Structure of the RemoteUI object
*/
struct RemoteUI_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    STATE state;        ///< Actual STATE of the STATE machine
    Msg msg;            ///< Structure used to pass parameters to the functions pointer.
    char nameTask[SIZE_TASK_NAME]; ///< Name of the task
    Mailbox * mb;

    int k_input; ///< keyboard input
    int currentEventNumber;
    int previousEventNumber;
    int myIp;
    int socket;
    VelocityVector vel;
};


/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/*------------- ACTION functions -------------*/
// TODO : put here all the ACTION functions prototypes
/**
 * @brief Function called when nothing needs to be done
 */
static void ActionNop(RemoteUI * this);

/**
 * @brief Changes the STATE of the STATE machine to S_DEATH
 */
static void ActionKill(RemoteUI * this);

/**
 * @brief Function called when there is the EVENT Example 1 and when the STATE is Idle
 */
static void ActionExample1FromIdle(RemoteUI * this);

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
        &ActionExample1FromRunning,
        &ActionExample1FromIdle,
        &ActionExample2,
        &ActionKill
};


/**
 * @brief STATE machine of the Example class
 */
static Transition stateMachine[NB_STATE][NB_EVENT] = { // TODO : fill the STATE machine
        [S_CONNECT_SCREEN][E_SETIP] = {S_CONNECT_SCREEN, A_SETIP},
};





/**
 * Efface les logs de la console
 * (de manière un peu sale)
 */
static void clear_logs()
{
    printf("\033c");
}

/**
 * Show all possible inputs in the terminal
 */
static void display()
{
    system("stty cooked");
    printf("%s", get_msg(MSG_COMMANDS));
    fflush(stdout);
    system("stty raw");
}

/**
 * Effectue l'action correspondant à la touche pressée
 */
static void capture_choice()
{
    RQ_data data;

    system("stty cooked");
    // Si le user veut quitter, lève le flag
    if (k_input == 'a') {
        printf("%s", get_msg(MSG_QUIT));
        data.command = C_QUIT;
        Client_sendMsg(data);
        flag_stop = ON;
    } else if (k_input == 'e') {
        clear_logs();
    } else {
        printf("%s", get_msg(MSG_COMMAND_ASKED));
        int id = get_id((char)k_input);
        if (id == -1) {
            printf("%s", get_msg(MSG_UNKNOWN_COMMAND));
        } else {
            Command command = list_commands[id];
            printf("%s", get_msg(command.msg));
            Client_sendMsg(command.command_order);
        }
        display();
    }
    system ("stty raw");
}

/**
 * Fonction principale du programme
 * Boucle tant que le flag du stop n'est pas levé
 */
static void run()
{
    // Stop displaying keys input in terminal
    system("stty -echo");
    display();
    while (flag_stop == OFF) {
        k_input = getchar();
        capture_choice();
    }
}

/**
 * Fonction de réinitialisation des paramètres
 */
static void quit()
{
    // Reset terminal parameters
    system("stty echo cooked");
}



/* ----------------------- RUN FUNCTION ----------------------- */

/**
 * @brief Main running function of the RemoteUI class
 */
static void RemoteUI_run(RemoteUI * this) {
    ACTION action;
    STATE state;
    Wrapper wrapper;

    TRACE("[%s] RUN - Queue name : %s\n", this->nameTask, this->mb->queueName)

    while (this->state != S_DEATH) {
        mailboxReceive(this->mb, &wrapper.toString); ///< Receiving an EVENT from the mailbox

        if (wrapper.msg.event == E_KILL) { // If we received the stop EVENT, we do nothing and we change the STATE to death.
            this->state = S_DEATH;

        } else {
            action = stateMachine[this->state][wrapper.msg.event].action;

            TRACE("\t [%s] | Action %s\n", this->nameTask, ACTION_toString[action])

            state = stateMachine[this->state][wrapper.msg.event].nextState;
            TRACE("\t [%s] | State %s\n", this->nameTask, STATE_toString[state])

            if (state != S_FORGET) {
                this->msg = wrapper.msg;
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
    TRACE("[RemoteUI] new function \n")
    RemoteUI * this = (RemoteUI *) malloc(sizeof(RemoteUI));
    this->mb = Mailbox_new("RemoteUI", remoteUIcounter, sizeof(Msg));
    sprintf(this->nameTask, NAME_TASK, remoteUIcounter);
}

/**
 * Start RemoteUI and waits for the user's input
 * until the user ask to quit
 */
extern void RemoteUI_start(RemoteUI * this)
{
    TRACE("[RemoteUI] start function \n")
    printf("%s", get_msg(MSG_START));
    int err = pthread_create(&(this->threadId), NULL, (void *) RemoteUI_run, this);

    return 0; // TODO: Handle the errors
}

/**
 * Stop RemoteUI
 */
extern void RemoteUI_stop()
{
    quit();
    Client_stop();
    //Pilot_stop();
    printf("%s", get_msg(MSG_STOP));
    fflush(stdout);
}

/**
 * destruct the RemoteUI from memory
 */
extern void RemoteUI_free() {}

