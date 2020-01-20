
#include <stdio.h>
#include <stdlib.h>
#include "pilot.h"
#include "robot.h"


static int k_input;


static void capture_choice() {
    system("stty cooked");
    printf("Vous avez demandé l'action :\n");
    switch (k_input) {
        case 'q':
            printf("aller à gauche\n");
            break;
        case 'd':
            printf("aller à droite\n");
            break;
        case 'z':
            printf("avancer\n");
            break;
        case 's':
            printf("reculer\n");
            break;
        case ' ':
            printf("stopper\n");
            break;
        case 'e':
            printf("effacer les logs\n");
            system("clear");
            break;
        case 'r':
            printf("Afficher l'état du robot");
        default:
            printf("Cette commande n'est pas reconnue\n");
            break;
    }
    system ("stty raw");
}

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

static void run() {
    // Stop displaying keys input in terminal
    system("stty -echo");
    display();
    while ((k_input=getchar())!= 'a') {
        capture_choice();
        display();
    }
}

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
    run();
}

/**
 * Stop AdminUI
 */
extern void AdminUI_stop()
{
    quit();
    printf("\nMerci d'avoir utilisé Robot V1\n");
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

