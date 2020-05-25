//
// Created by gwendal on 25/05/2020.
//

#ifndef MR_ROBOT_LOGGER_H
#define MR_ROBOT_LOGGER_H


#include <robocom.h>
#include <mailbox.h>
#include <liste_chainee.h>


/**STATE
 * Liste des différents états possibles du logger
 * FORGET:aucune action n'est effectuée
 * IDLE: en attente du démarage du logger
 * POLLING: recolte les LogEvent
 */
ENUM_DECL(State,
          S_FORGET=0,
          S_IDLE,
          S_POLLING,
          S_DEATH,         ///< Etat de mort du robot, utilisé lorsqu'on veut l'arrêter
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

ENUM_DECL(Event,
          E_INIT=0,
          E_START_POLLING,
          E_STOP_POLLING,
          E_AFTER_250MS,
          E_KILL,                  ///< Détruit la MaE
          EVENT_NB
        )

/**
 * Liste des différentes actions possibles
 */

ENUM_DECL(TransitionAction,
          A_NOP,                      ///< Ne rien faire
          A_POLL,
          ACTION_NB
)

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



/**
 * @brief Structure d'un message ajouté à la BAL
 */
typedef struct {
    Event event; ///< Paramètre event du message
} Msg;

/**
 * @brief Wrapper enum. Utilisé pour envoyer des Msg dans une BAL
 */
wrapperOf(Msg)

/**
 * @brief Structure de l'objet Logger
 */
struct Logger_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    SensorState sens;
    int speed;
    Mailbox * mailbox;
    Msg message; ///< Structure utilisée pour passer les donnés de la BAL aux pointeurs de fonction
    State myState; ///< Etat actuel de la MaE
    Liste *eventList;

};

typedef struct Logger_t Logger;

typedef struct LogEvent_t LogEvent;



/**
 * get the robot sensorState
 */
extern SensorState Logger_getSensorState(Logger * this);

/**
 * get the robot speed
 */
extern int Logger_getRobotSpeed(Logger * this);

/**
 * to be notified when an emergency stop is needed
 */
extern void Logger_signalES(Logger * this);

/**
 * ??????????????????
 */
extern void Logger_askEvents(int from,int to,Logger * this);

/**
 * ??????????????????
 */
extern void Logger_askEventsCount(Logger * this);

/**
 * ??????????????????
 */
extern void Logger_setEvents(LogEvent *events, Logger * this);

/**
 * ??????????????????
 */
extern void Logger_setEventsCount(int indice,Logger * this);

/**
 * start polling
 */
extern void Logger_startPolling(Logger * this);

/**
 * stop polling
 */
extern void Logger_stopPolling(Logger * this);

/**
 * clear events
 */
extern void Logger_clearEvents(Logger * this);

/**
 * return the number of events
 */
extern int Logger_getEventsCount(Logger * this);

/**
 * get a tab of event from indice "from" to "to"
 * you have to pass an array of LogEvent in parameters to put the LogEvents collected in it. The size should be <(to-from)+1>.
 * I had to do this because I cannot return a localy created array
 */
extern LogEvent* Logger_getEvents(int from, int to,LogEvent *logEventToReturn,Logger * this);

/**
 * create the logger
 */
extern Logger* Logger_new();

/**
 * start the logger
 */
extern void Logger_start(Logger * this);

/**
 * stop the logger
 */
extern void Logger_stop(Logger * this);

/**
 * free the logger
 */
extern void Logger_free(Logger * this);

#endif //MR_ROBOT_LOGGER_H
