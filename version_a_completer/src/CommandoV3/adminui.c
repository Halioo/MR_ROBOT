//
// Created by cleme on 20/04/2020.
//

#include "adminui.h"


/* ---------- TYPE DEFINITIONS ----------*/

/**
 * @brief Enumeration of all the states that can be taken by the state machine
 */
ENUM_DECL(STATE,
        S_FORGET,      ///< Nothing happens
S_INIT,
        S_MAIN_SCREEN,
        S_LOG_SCREEN,
        S_DEATH
)

/**
 * @brief Enumaration of all the possible actions called by the state machine
 */
ENUM_DECL(ACTION,
        A_NOP,  ///< Nothing happens
A_INIT_MAINSCREEN,
        A_MAINSCREEN_LOGSCREEN,
        A_LOGSCREEN_MAINSCREEN,
        A_ONENTRY_MAINSCREEN,
        A_ONENTRY_LOGSCREEN,
        A_LOG_LOOP,
        A_QUIT,
        A_KILL  ///< Kills the state machine
)


/**
 * @brief Enumeration of all the possible events that triggers the state machine
 */
ENUM_DECL(EVENT,
        E_NOP,  ///< Do nothing
E_TOGGLE_ES,

        E_SHOW_LOGSCREEN,
        E_LOGSCREEN_TO,
        E_QUIT,
        E_KILL    ///< Kills the STATE machine
)