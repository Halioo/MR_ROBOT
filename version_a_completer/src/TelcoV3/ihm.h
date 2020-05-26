//
// Created by cleme on 19/04/2020.
//

#ifndef CLIENT_IHM_H
#define CLIENT_IHM_H


#include <robocom.h>
#include "util.h"


// Default langage
#ifndef LANG
#define LANG ENGLISH
#endif


extern int ihm_new();
extern int ihm_start();
extern int ihm_stop();
extern int ihm_free();


// GESTION DE LA LANGUE D'AFFICHAGE

ENUM_DECL(LANGUAGE, FRENCH, ENGLISH, GERMAN)

ENUM_DECL(TYPES_MSG, MSG_DEFAULT, MSG_START, MSG_STOP, MSG_COMMANDS, MSG_LOGS, MSG_QUIT, MSG_UNKNOWN_COMMAND, MSG_COMMAND_ASKED, MSG_COMMAND_LEFT,MSG_COMMAND_RIGHT,MSG_COMMAND_FWD, MSG_COMMAND_BCKWD, MSG_COMMAND_STOP, MSG_COMMAND_LOGS, MSG_COMMAND_STATE)


static char const * const msg[NB_TYPES_MSG][NB_LANGUAGE] = {
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
    {'e', MSG_COMMAND_LOGS, C_LOGS}
};


#endif //CLIENT_IHM_H
