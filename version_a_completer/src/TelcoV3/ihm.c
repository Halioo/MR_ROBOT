//
// Created by cleme on 24/05/2020.
//


#include "ihm.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "../../lib/include/mailbox.h"
#include "../../../infox_prose-x86_64-v0.3/include/infox/prose/prose.h"


char k_input;
FLAG flag_stop = DOWN;

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


/**
 * Fonction principale du programme
 * Boucle tant que le flag du stop n'est pas levé
 */
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