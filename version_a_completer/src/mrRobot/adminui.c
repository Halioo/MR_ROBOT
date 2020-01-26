
#include <stdio.h>
#include <stdlib.h>
#include "pilot.h"

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
    MSG_COMMAND_Q,MSG_COMMAND_D,MSG_COMMAND_Z, MSG_COMMAND_S,
    MSG_COMMAND_SPACE, MSG_COMMAND_E, MSG_COMMAND_R,
    MSG_NUMBER
} TYPES_MSG;

// Default langage
#ifndef LANG
#define LANG FRENCH
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
    {   // MSG_COMMAND_Q
        "aller à gauche\n",
        "go left\n",
        "gehe nach links\n"
    },
    {   // MSG_COMMAND_D
        "aller à droite\n",
        "go right\n",
        "gehe nach rechts\n"
    },
    {   // MSG_COMMAND_Z
        "avancer\n",
        "go forward\n",
        "voraus\n"
    },
    {   // MSG_COMMAND_S
        "reculer\n",
        "go backward\n",
        "rückzug\n"
    },
    {   // MSG_COMMAND_SPACE
        "stopper\n",
        "stop\n",
        "anschlag\n"
    },
    {   // MSG_COMMAND_E
        "effacer les logs\n",
        "clear logs\n",
        "lösche sie logs\n"
    },
    {   // MSG_COMMAND_R
        "afficher l'état du robot\n",
        "show robot's state\n",
        "roboterstatus anzeigen\n"
    },
};


static int k_input;
static Flag flag_stop;


/**
 * Retourne le string correspondant
 */
static const char * get_msg(TYPES_MSG type_msg) {
    return msg[type_msg][LANG];
}

/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
static void ask_mvt(Direction dir) {
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
        default:
            vel.power = DEFAULT_POWER_TURN;
            break;
    }
    Pilot_setVelocity(vel);
}

/**
 * Demande l'actualisation du pilote, récupère
 * ses états et les affiche dans la console
 */
static void ask4log() {
    Pilot_check();
    PilotState pt = Pilot_getState();
    printf(get_msg(MSG_LOGS), pt.speed, pt.collision, pt.luminosity);
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
static void capture_choice() {
    system("stty cooked");
    // Si le user veut quitter, lève le flag
    if (k_input == 'a') {
        printf("%s", get_msg(MSG_QUIT));
        flag_stop = ON;
    } else {
        printf("%s", get_msg(MSG_COMMAND_ASKED));
        switch (k_input) {
            case 'q':
                printf("%s", get_msg(MSG_COMMAND_Q));
                ask_mvt(LEFT);
                break;
            case 'd':
                printf("%s", get_msg(MSG_COMMAND_D));
                ask_mvt(RIGHT);
                break;
            case 'z':
                printf("%s", get_msg(MSG_COMMAND_Z));
                ask_mvt(FORWARD);
                break;
            case 's':
                printf("%s", get_msg(MSG_COMMAND_S));
                ask_mvt(BACKWARD);
                break;
            case ' ':
                printf("%s", get_msg(MSG_COMMAND_SPACE));
                VelocityVector vel;
                vel.power = 0;
                Pilot_setVelocity(vel);
                break;
            case 'e':
                printf("%s", get_msg(MSG_COMMAND_E));
                for (int i=0; i<16; i++) {printf("\n");}
                break;
            case 'r':
                printf("%s", get_msg(MSG_COMMAND_R));
                for (int i=0; i<16; i++) {printf("\n");}
                ask4log();
                break;
            default:
                printf("%s", get_msg(MSG_UNKNOWN_COMMAND));
                break;
        }
        display();
    }
    system ("stty raw");
}

/**
 * Fonction principale du programme
 * Boucle tant que le flag du stop n'est pas levé
 */
static void run() {
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
static void quit() {
    // Reset terminal parameters
    system("stty echo cooked");
}

/**
 * Start AdminUI and waits for the user's input
 * until the user ask to quit
 */
extern void AdminUI_start()
{
    printf("%s", get_msg(MSG_START));
    Pilot_start();
    run();
}

/**
 * Stop AdminUI
 */
extern void AdminUI_stop()
{
    quit();
    Pilot_stop();
    printf("%s", get_msg(MSG_STOP));
    fflush(stdout);
}

/**
 * initialize in memory AdminUI
 */
extern void AdminUI_new() {}

/**
 * destruct the AdminUI from memory
 */
extern void AdminUI_free() {}

