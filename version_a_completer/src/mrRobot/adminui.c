
#include <stdio.h>
#include <stdlib.h>
#include "pilot.h"
#include "robot.h"


typedef enum {ACTIVATED=1, DEACTIVATED=0} Flag;


static int k_input;
static Flag flag_stop = DEACTIVATED;


static void ask_mvt(Direction dir) {}

static void ask4log() {}

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

static void capture_choice() {
    system("stty cooked");
    if (k_input == 'a') {
        printf("quitter\n");
        flag_stop = ACTIVATED;
    } else {
        printf("Vous avez demandé l'action :\n");
        switch (k_input) {
            case 'q':
                printf("aller à gauche\n");
                // TODO faire tourner à gauche
                break;
            case 'd':
                printf("aller à droite\n");
                // TODO faire tourner à droite
                break;
            case 'z':
                printf("avancer\n");
                // TODO avancer
                break;
            case 's':
                printf("reculer\n");
                // TODO reculer
                break;
            case ' ':
                printf("stopper\n");
                // TODO Stopper robot
                break;
            case 'e':
                printf("effacer les logs\n");
                for (int i=0; i<16; i++) {printf("\n");}
                break;
            case 'r':
                printf("afficher l'état du robot\n");
                // TODO afficher l'etat du robot
                break;
            case 'a':
                printf("quitter\n");
                flag_stop = ACTIVATED;
                break;
            default:
                printf("Cette commande n'est pas reconnue\n");
                break;
        }
        display();
    }
    system ("stty raw");
}

static void run() {
    // Stop displaying keys input in terminal
    system("stty -echo");
    display();
    flag_stop = DEACTIVATED;
    while (flag_stop == DEACTIVATED) {
        k_input=getchar();
        capture_choice();
    }
}

static void quit() {
    // Reset terminal parameters
    //system("stty echo cooked");
}

/**
 * Start AdminUI and waits for the user's input
 * until the user ask to quit
 */
extern void AdminUI_start()
{
    printf("Bienvenue sur Robot V1\n");
    run();
}

/**
 * Stop AdminUI
 */
extern void AdminUI_stop()
{
    quit();
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

