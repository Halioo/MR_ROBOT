
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "util.h"

#include "pilot.h"
#include "robot.h"


/**
 * @brief Liste des états du pilot
 */
ENUM_DECL(State,
        S_FORGET,       ///< Etat par défaut, utilisé pour consommer une action
        S_IDLE,         ///< Etat où le robot est arrêté
        S_RUNNING,      ///< Etat où le robot est en mouvement
        S_BUMP_CHECK,   ///< Etat pour vérifier si le robot a heurté un obstacle
        S_EMERGENCY,    ///< Etat d'arrêt d'urgence
        S_DEATH         ///< Etat de mort du robot, utilisé lorsqu'on veut l'arrêter
)

/**
 * @brief Liste des évènements du pilot
 */
ENUM_DECL(Event,
        E_NOP,                  ///< Ne rien faire
        E_SET_ROBOT_VELOCITY,   ///< Demande d'envoi d'une vitesse au robot
        E_STOP,                 ///< Met la vitesse du robot à 0
        E_TOGGLE_ES,            ///< Signal d'urgence reçu
        E_TO_BUMP,              ///< Watchdog arrivé à expiration pour vérifier la collision
        E_BUMPED,               ///< Le robot s'est cogné
        E_NOT_BUMPED,           ///< Le robot ne s'est pas cogné
        E_KILL                  ///< Détruit la MaE
)

/**
 * @brief Liste des actions du pilot
 */
ENUM_DECL(Action,
        A_NOP,                      ///< Ne rien faire
        A_SEND_MVT_0,               ///< Envoi d'une vitesse nulle au robot
        A_IDLE_TO_RUNNING,          ///< Action appellée quand on passe de l'état IDLE à l'état RUNNING
        A_RUNNING_TO_IDLE,          ///< Action appellée quand on passe de l'état RUNNING à l'état IDLE
        A_RUNNING_TO_RUNNING,       ///< Action appellée quand on passe de l'état RUNNING à l'état RUNNING
        A_RUNNING_TO_BUMP_CHECK,    ///< Action appellée quand on passe de l'état RUNNING à l'état BUMP_CHECK
        A_BUMP_CHECK_TO_RUNNING,    ///< Action appellée quand on passe de l'état BUMP_CHECK à l'état RUNNING
        A_KILL                      ///< Action pour détruire la MaE
)

/**
 * @brief Structire de transition de la MaE
 */
typedef struct {
    State nextState; ///< Prochain State de la MaE
    Action action;   ///< Action réalisée avant d'aller dans le prochain State
} Transition;


/**
 * @brief Structure d'un message ajouté à la BAL
 */
typedef struct {
    Event event; ///< Paramètre event du message
    VelocityVector vel; ///< Paramètre VelocityVector du message
} Msg;

/**
 * @brief Wrapper enum. Utilisé pour envoyer des Msg dans une BAL
 */
wrapperOf(Msg)

/**
 * @brief Structure de l'objet Pilot
 */
struct Pilot_t {
    // TODO : ajouter les paramètres statiques pour en faire un objet multi-instances
};


/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

/*------------- ACTION functions -------------*/

/**
 * @brief Fonction appellée quand il ne faut rien faire
 */
static void ActionNop();

/**
 * @brief Fonction appellée quand il faut immobiliser le robot
 */
static void ActionSendMvt0();

/**
 * @brief Fonction appellée quand il faut passer de l'état IDLE à l'état RUNNING
 */
static void ActionIdleToRunning();

/**
 * @brief Fonction appellée quand il faut passer de l'état RUNNING à l'état IDLE
 */
static void ActionRunningToIdle();

/**
 * @brief Fonction appellée quand il faut passer de l'état RUNNING à l'état RUNNING
 */
static void ActionRunningToRunning();

/**
 * @brief Fonction appellée quand il faut passer de l'état RUNNING à l'état BUMP_CHECK
 */
static void ActionRunningToBumpCheck();

/**
 * @brief Fonction appellée quand il faut passer de l'état BUMP_CHECK à l'état RUNNING
 */
static void ActionBumpCheckToRunning();

/**
 * @brief Fonction appellée quand il faut détruire la MaE
 */
static void ActionKill();


/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Pointeur de fonction pour les fonction d'Action de la MaE
 */
typedef void (*ActionPtr)();

/**
 * @brief Tableau de pointeur de fonctions pour stocker les fonctions correspondantes aux actions dans le même ordre qu'elles ont été définies
 */
static const ActionPtr actionPtrTab[NB_Action]={
        &ActionNop,
        &ActionSendMvt0,
        &ActionIdleToRunning,
        &ActionRunningToIdle,
        &ActionRunningToRunning,
        &ActionRunningToBumpCheck,
        &ActionBumpCheckToRunning,
        &ActionKill
};


/**
 * @brief MaE de la classe Pilot
 */
static const Transition stateMachine[NB_State][NB_Event] = {
        [S_IDLE][E_TOGGLE_ES] = {S_EMERGENCY,A_SEND_MVT_0},
        [S_IDLE][E_SET_ROBOT_VELOCITY] = {S_RUNNING,A_IDLE_TO_RUNNING},
        [S_RUNNING][E_STOP] = {S_IDLE,A_RUNNING_TO_IDLE},
        [S_RUNNING][E_TOGGLE_ES] = {S_EMERGENCY,A_SEND_MVT_0},
        [S_RUNNING][E_SET_ROBOT_VELOCITY] = {S_RUNNING,A_RUNNING_TO_RUNNING},
        [S_RUNNING][E_TO_BUMP] = {S_BUMP_CHECK,A_RUNNING_TO_BUMP_CHECK},
        [S_BUMP_CHECK][E_NOT_BUMPED] = {S_RUNNING,A_BUMP_CHECK_TO_RUNNING},
        [S_BUMP_CHECK][E_BUMPED] = {S_IDLE,A_SEND_MVT_0},
        [S_BUMP_CHECK][E_TOGGLE_ES] = {S_EMERGENCY,A_SEND_MVT_0},
        [S_EMERGENCY][E_TOGGLE_ES] = {S_IDLE,A_SEND_MVT_0}
        // TODO : gérer le kill ?
};

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

// TODO : Write all the ACTION functions


static void ActionNop(){

}


static void ActionSendMvt0(){

}


static void ActionIdleToRunning(){

}


static void ActionRunningToIdle(){

}


static void ActionRunningToRunning(){

}


static void ActionRunningToBumpCheck(){

}


static void ActionBumpCheckToRunning(){

}


static void ActionKill(){

}






















