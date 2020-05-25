//
// Created by gwendal on 25/05/2020.
//
#include "util.h"


/**STATE
 * Liste des différents états possibles du logger
 * FORGET:aucune action n'est effectuée
 * IDLE: en attente du démarage du logger
 * POLLING: recolte les logEvent
 */
ENUM_DECL(State,
        S_FORGET=0,
        S_IDLE,
        S_POLLING,
        STATE_NB
)

/**
 * Liste des event possibles
 * START_POLLING:
 * Evenement pour commencer à "Polling"
 * STOP_POLLING:
 * Evenement pour arreter de "Polling"
 * AFTER_250MS:
 * Evenement pour attendre 250ms avant d'effectuer les actions de "Polling"
 */
typedef enum {
    E_INIT=0,
    E_START_POLLING,
    E_STOP_POLLING,
    E_AFTER_250MS,
    EVENT_NB
} Event;

/**
 * Liste des différentes actions possibles
 */
typedef enum
{
    A_NOP=0,
    A_POLL,
    ACTION_NB
} TransitionAction;

/**
 * Structure de transition, contient l'état suivant et l'action à effectuer
 * L'action à effectuer est stocké sur la forme d'enum, il faut utiliser
 * la table de conversion pour obtenir le pointeur de fonction
 */
typedef struct
{
    State next_state;
    TransitionAction action_to_perform;
} Transition;

/**
 * Tableau des transitions
 * [etat_courant][type_event]
 */
static Transition transition_tab[STATE_NB][EVENT_NB] =
        {
            [S_IDLE][E_START_POLLING] = {S_POLLING, A_NOP},
            [S_POLLING][E_STOP_POLLING] = {S_IDLE, A_NOP},
            [S_POLLING][E_AFTER_250MS] = {S_POLLING, A_NOP},

        };

typedef void (*f_ptr);

static void poll();


// Action NO OPERATION
static void action_NOP(){
    // Fonction vide
}

static const f_ptr actions_tab[ACTION_NB] = {
        &action_NOP,
        &poll
};



extern void Logger_new(){

}