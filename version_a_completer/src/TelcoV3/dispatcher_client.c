#include "dispatcher_client.h"

/**
 * @brief Enumeration of all the STATEs that can be taken by the STATE machine
 */
ENUM_DECL(STATE,
    S_FORGET,      ///< Nothing happens
    S_IDLE,        ///< Idle STATE
    S_RUNNING,     ///< Running STATE
    S_DEATH        ///< Transition STATE for stopping the STATE machine
)


/**
 * @brief Enumaration of all the possible ACTIONs called by the STATE machine
 */
ENUM_DECL(ACTION,
    A_NOP,                      ///< Nothing happens
    A_STOP_RUNNING,             ///< ACTION called when passing from the RUNNING STATE to the IDLE STATE
    A_START_RUNNING,            ///< ACTION called when passing from the IDLE STATE to the RUNNING STATE
    A_KILL                      ///< Kills the STATE machine
)


/**
 * @brief Enumeration of all the possible EVENTs that triggers the STATE machine
 */
ENUM_DECL(EVENT,
    E_NOP,      ///< Do nothing
    E_VEL_IS_NULL, ///< EVENT START RUNNING
    E_SET_VEL, ///< EVENT STOP RUNNING
    E_KILL     ///< Kills the STATE machine
)


/**
 * @brief Transition structure of the STATE machine
 */
typedef struct {
    STATE nextState; ///< Next STATE of the STATE machine
    ACTION action;   ///< ACTION done before going in the next STATE
} Transition;

/**
 * @brief Structure of a message sent in the mailbox
 */
typedef struct {
    EVENT event;            ///< EVENT sent in the message
    int eventsCount;        ///< Example of an other parameter
    EVENT tabEvents[10];      ///< Example of a possible parameter
} Msg;




extern void setEvents() {
    TRACE("from : remoteUI // msg: setEvents \n")
}

extern void setEventsCount() {
    TRACE("to : remoteUI // msg: setEventsCount")
}