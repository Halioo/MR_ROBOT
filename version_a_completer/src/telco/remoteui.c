
#include <stdio.h>
#include <stdlib.h>

#include "../commando/pilot.h"
#include "client.h"

#define LANG FRENCH


#define DEFAULT_POWER_FWD 80
#define DEFAULT_POWER_BCKWD 60
#define DEFAULT_POWER_TURN 50


typedef enum {OFF=0, ON} Flag;


// GESTION DE LA LANGUE D'AFFICHAGE

typedef enum {
    FRENCH, ENGLISH, GERMAN,
    LANGUAGE_NUMBER
} LANGUAGE;

typedef enum {
    MSG_DEFAULT, MSG_START, MSG_STOP,
    MSG_COMMANDS, MSG_LOGS, MSG_QUIT,
    MSG_UNKNOWN_COMMAND, MSG_COMMAND_ASKED,
    MSG_COMMAND_LEFT,MSG_COMMAND_RIGHT,MSG_COMMAND_FWD, MSG_COMMAND_BCKWD,
    MSG_COMMAND_STOP, MSG_COMMAND_LOGS, MSG_COMMAND_STATE,
    MSG_NUMBER
} TYPES_MSG;

// Default langage
#ifndef LANG
#define LANG ENGLISH
#endif

static char const * const msg[MSG_NUMBER][LANGUAGE_NUMBER] =
{
    {   // MSG_DEFAULT
        "Langue : Français\n",
        "Language : English\n",
        "Sprache: Deutsch\n"
    },
    {   // MSG_START
        "Bienvenue sur Robot V1\n",
        "Welcome to Robot V1\n",
        "Willkommen bei Robot V1\n"
    },
    {   // MSG_STOP
        "Merci d'avoir utilisé Robot V1\nA bientôt !\n",
        "Thank you for using Robot V1\nSee you soon!\n",
        "Vielen Dank, dass Sie Robot V1 verwenden\nBis bald !\n"
    },
    {   // MSG_COMMANDS
        "Vous pouvez faire les actions suivantes :\n"
        "q:aller à gauche\nd:aller à droite\nz:avancer\ns:reculer\n :stopper\n"
        "e:effacer les logs\nr:afficher l'état du robot\na:quitter\n",

        "You can do the following actions:\n"
        "q:go left\nd:go right\nz:go forward\ns:go backward\n :stop\n"
        "e:clear logs\nr:show robot's state\na:quit\n",

        "Sie können die folgenden Aktionen ausführen :\n"
        "q:gehe nach links\nd:gehe nach rechts\nz:voraus\ns:rückzug\n :anschlag\n"
        "e:lösche sie logs\nr:roboterstatus anzeigen\na:leave\n"
    },
    {   // MSG_LOGS
        "Etat du robot: Vitesse %d, Collision %d, Lumiere %f\n",
        "Robot's state: Speed %d, Collision %d, Light %f\n",
        "Roboterstatus: Geschwindigkeit %d, Kollision %d, Licht %f\n"
    },
    {   // MSG_QUIT
        "quitter\n",
        "quit\n",
        "leave\n"
    },
    {   // MSG_UNKNOWN_COMMAND
        "Cette commande n'est pas reconnue\n",
        "This command is not recognized\n",
        "Dieser befehl wird nicht erkannt\n"
    },
    {   // MSG_COMMAND_ASKED
        "Vous avez demandé l'action :\n",
        "You requested the following action:\n",
        "Sie haben folgende aktion angefordert :\n"
    },
    {   // MSG_COMMAND_LEFT
        "aller à gauche\n",
        "go left\n",
        "gehe nach links\n"
    },
    {   // MSG_COMMAND_RIGHT
        "aller à droite\n",
        "go right\n",
        "gehe nach rechts\n"
    },
    {   // MSG_COMMAND_FWD
        "avancer\n",
        "go forward\n",
        "voraus\n"
    },
    {   // MSG_COMMAND_BCKWD
        "reculer\n",
        "go backward\n",
        "rückzug\n"
    },
    {   // MSG_COMMAND_STOP
        "stopper\n",
        "stop\n",
        "anschlag\n"
    },
    {   // MSG_COMMAND_LOGS
        "effacer les logs\n",
        "clear logs\n",
        "lösche sie logs\n"
    },
    {   // MSG_COMMAND_STATE
        "afficher l'état du robot\n",
        "show robot's state\n",
        "roboterstatus anzeigen\n"
    },
};


static int k_input;
static Flag flag_stop;


static void ask_mvt(Direction dir);
static void ask4log(void);
static void clear_logs(void);

typedef void (*f_ptr_generic)(void);
typedef void (*f_ptr_dir)(Direction dir);


/**
 * Structure contenant les arguments qu'il
 * est possible de passer dans les fonctions
 */
typedef struct {
    Direction dir;
} Command_args;

/**
 * Structure d'une commande, une commande est associée à :
 * key: une touche du clavier
 * msg: un message à afficher dans la console
 * args: des arguments à passer dans une fonction
 * func: une fonction à appeler
 */
typedef struct {
    char key;
    TYPES_MSG msg;
    Command_args* args;
    f_ptr_generic func;
} Command;

// Liste des arguments possibles (ici les différentes directions)
static Command_args args_dir_left = {.dir = LEFT};
static Command_args args_dir_right = {.dir = RIGHT};
static Command_args args_dir_forward = {.dir = FORWARD};
static Command_args args_dir_backward = {.dir = BACKWARD};
static Command_args args_dir_stop = {.dir = STOP};

// Array regroupant les différentes commandes possibles
static Command list_commands[] =
{
    {'q', MSG_COMMAND_LEFT,  &args_dir_left, (f_ptr_generic) &ask_mvt},
    {'d', MSG_COMMAND_RIGHT, &args_dir_right, (f_ptr_generic) &ask_mvt},
    {'z', MSG_COMMAND_FWD,   &args_dir_forward, (f_ptr_generic) &ask_mvt},
    {'s', MSG_COMMAND_BCKWD, &args_dir_backward, (f_ptr_generic) &ask_mvt},
    {' ', MSG_COMMAND_STOP,  &args_dir_stop, (f_ptr_generic) &ask_mvt},
    {'e', MSG_COMMAND_LOGS,  NULL, &clear_logs},
    {'r', MSG_COMMAND_STATE, NULL, &ask4log}
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
static int get_id(char elem)
{
    int id = -1;
    for (int i=0; i<command_number; i++) {
        if (list_commands[i].key == elem) {
            id = i;
        }
    }
    return id;
}

/**
 * Retourne le string correspondant au type de message
 * passé en paramètre
 */
static const char * get_msg(TYPES_MSG type_msg)
{
    return msg[type_msg][LANG];
}

/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
static void ask_mvt(Direction dir)
{
    VelocityVector vel = {
            .dir = dir,
    };
    switch (dir) {
        case FORWARD:
            vel.power = DEFAULT_POWER_FWD;
            break;
        case BACKWARD:
            vel.power = DEFAULT_POWER_BCKWD;
            break;
        case LEFT:
        case RIGHT:
            vel.power = DEFAULT_POWER_TURN;
            break;
        default:
            vel.power = 0;
    }
    Pilot_setVelocity(vel);
}

/**
 * Demande l'actualisation du pilote, récupère
 * ses états et les affiche dans la console
 */
static void ask4log()
{
    Pilot_check();
    PilotState pt = Pilot_getState();
    clear_logs();
    printf(get_msg(MSG_LOGS), pt.speed, pt.collision, pt.luminosity);
}

/**
 * Efface les logs de la console
 * (de manière un peu sale)
 */
static void clear_logs()
{
    for (int i=0; i<16; i++) {printf("\n");}
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
    static int id_data=0;
    id_data++;
    Data data =
    {
        .id = id_data,
    };
    sendMsg(data);

    system("stty cooked");
    // Si le user veut quitter, lève le flag
    if (k_input == 'a') {
        printf("%s", get_msg(MSG_QUIT));
        flag_stop = ON;
    } else {
        printf("%s", get_msg(MSG_COMMAND_ASKED));
        int id = get_id((char)k_input);
        if (id == -1) {
            printf("%s", get_msg(MSG_UNKNOWN_COMMAND));
        } else {
            Command command = list_commands[id];
            printf("%s", get_msg(command.msg));
            if (command.args == NULL) {
                command.func();
            } else {
                ((f_ptr_dir) command.func)(command.args->dir);
            }
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

/**
 * Start RemoteUI and waits for the user's input
 * until the user ask to quit
 */
extern void RemoteUI_start()
{
    printf("%s", get_msg(MSG_START));
    Client_start();
    Pilot_start();
    run();
}

/**
 * Stop RemoteUI
 */
extern void RemoteUI_stop()
{
    quit();
    Client_stop();
    Pilot_stop();
    printf("%s", get_msg(MSG_STOP));
    fflush(stdout);
}

/**
 * initialize in memory RemoteUI
 */
extern void RemoteUI_new() {}

/**
 * destruct the RemoteUI from memory
 */
extern void RemoteUI_free() {}

