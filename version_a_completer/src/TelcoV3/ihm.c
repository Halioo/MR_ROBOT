//
// Created by cleme on 24/05/2020.
//


#include "ihm.h"

#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>


//RemoteUI * remoteUI;
char userInput;
FLAG flag_stop = DOWN;
pthread_t threadId;

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
    for (int i=0; i < command_number; i++) {
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
/**
static void capture_choice()
{
    RQ_data data;

    system("stty cooked");
    // Si le user veut quitter, lève le flag
    if (k_input == 'a') {
        printf("%s", get_msg(MSG_QUIT));
        data.command = C_QUIT;
        //Client_sendMsg(data);
        flag_stop = UP;
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
            //Client_sendMsg(command.command_order);
        }
        display();
    }
    system ("stty raw");
}
*/

/**
 * Fonction principale du programme
 * Boucle tant que le flag du stop n'est pas levé
 */
 /**
static void run()
{
    // Stop displaying keys input in terminal
    system("stty -echo");
    display();
    while (flag_stop == DOWN) {
        k_input = getchar();
        capture_choice();
    }
}
  */

/**
 * Fonction de réinitialisation des paramètres
 */
static void quit()
{
    // Reset terminal parameters
    system("stty echo cooked");
}

/**
 * Stop RemoteUI
 */
extern int stop()
{
    printf("%s", get_msg(MSG_STOP));
    fflush(stdout);
    return 0; // TODO: Handle the errors
}


static void ihm_run() {
    while(flag_stop == DOWN) {
        userInput = getch();
        if (userInput == 'a') {
            flag_stop = UP;
        } else {
            mvprintw(5, 10, "%c", userInput);
        }
        refresh();
    }
}


extern int ihm_new() {
    // Initialise l'écran ncurses
    initscr();
    noecho();
    refresh();
    return 0;
}
extern int ihm_start() {
    TRACE("ExampleStart function \n")
    int err = pthread_create(&threadId, NULL, (void *) &ihm_run, NULL);
    STOP_ON_ERROR(err != 0, "Error when creating the thread")
    return 0;
}
extern int ihm_stop() {
    int err = pthread_join(threadId, NULL);
    STOP_ON_ERROR(err != 0, "Error when waiting for the thread to end")
    return 0;
}
extern int ihm_free() {
    // Détruit l'écran
    endwin();
    return 0;
}




