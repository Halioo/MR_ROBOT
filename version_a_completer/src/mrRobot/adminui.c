
#include <stdio.h>
#include <stdlib.h>
#include "pilot.h"

#define DEFAULT_POWER_FWD 80
#define DEFAULT_POWER_BCKWD 60
#define DEFAULT_POWER_TURN 50


typedef enum {OFF=0, ON} Flag;


static int k_input;
static Flag flag_stop;


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
    printf("Etat du robot: Vitesse %d, Collision %d, Lumiere %f\n",
            pt.speed, pt.collision, pt.luminosity);
}

/**
 * Show all possible inputs in the terminal
 */
static void display()
{
    system("stty cooked");
    printf("Vous pouvez faire les actions suivantes :\n");
    printf("q:aller à gauche\n");
    printf("d:aller à droite\n");
    printf("z:avancer\n");
    printf("s:reculer\n");
    printf(" :stopper\n");
    printf("e:effacer les logs\n");
    printf("r:afficher l'état du robot\n");
    printf("a:quitter\n");
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
        printf("quitter\n");
        flag_stop = ON;
    } else {
        printf("Vous avez demandé l'action :\n");
        switch (k_input) {
            case 'q':
                printf("aller à gauche\n");
                ask_mvt(LEFT);
                break;
            case 'd':
                printf("aller à droite\n");
                ask_mvt(RIGHT);
                break;
            case 'z':
                printf("avancer\n");
                ask_mvt(FORWARD);
                break;
            case 's':
                printf("reculer\n");
                ask_mvt(BACKWARD);
                break;
            case ' ':
                printf("stopper\n");
                VelocityVector vel;
                vel.power = 0;
                Pilot_setVelocity(vel);
                break;
            case 'e':
                printf("effacer les logs\n");
                for (int i=0; i<16; i++) {printf("\n");}
                break;
            case 'r':
                printf("afficher l'état du robot\n");
                for (int i=0; i<16; i++) {printf("\n");}
                ask4log();
                break;
            default:
                printf("Cette commande n'est pas reconnue\n");
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
    printf("Bienvenue sur Robot V1\n");
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
    printf("Merci d'avoir utilisé Robot V1\n");
    printf("A bientôt\n");
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

