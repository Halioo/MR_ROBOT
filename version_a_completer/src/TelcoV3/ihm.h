/**
 * @file ihm.h
 *
 * @brief This is an ihm of active class
 *
 * You can copy and paste this ihm to create an active class.
 * To use it :
 * - Search and replace (Distinguish upper and lower case) the following
 * words in the .c and .h file: ihm, Ihm
 * - Follow the "TODO" instructions
 * - Remove the Ihm events and parameters.
 *
 * You can explain here the way to use the module (preconditions, etc).
 *
 * You can create several paragraphs by letting a blank space.
 *
 * You must code and document everything in english.
 *
 * @date April 2020
 *
 * @authors Thomas CRAVIC, Clément PUYBAREAU, Louis FROGER
 *
 * @version 1.0
 *
 * @copyright CCBY 4.0
 */

#ifndef IHM_H
#define IHM_H

#include "../../lib/include/util.h"

#include "messages.h"


// Default langage
#ifndef LANG
#define LANG ENGLISH
#endif


ENUM_DECL(UI_SCREEN, SCREEN_CONNECT, SCREEN_MAIN, SCREEN_LOG, SCREEN_ERROR)




/* ----------------------- PUBLIC FUNCTIONS PROTOTYPES -----------------------*/

extern void Ihm_displayScreen(UI_SCREEN screen);
//extern void Ihm_quit();

/* ----------------------- NEW START STOP FREE -----------------------*/

/**
 * @brief Ihm class constructor
 *
 * Allocates an Ihm object
 *
 *
 * @retval 0 If the allocation worked
 * @retval -1 if the allocation didn't work
 */
extern int IhmNew();

/**
 * @brief Ihm class starter
 *
 * Starts the Ihm object
 *
 * @retval 0 If the start worked
 * @retval -1 If the start didn't work
 */
extern int IhmStart();

/**
 * @brief Ihm singleton stopper
 *
 * @retval 0 If the object stopped properly
 * @retval -1 If the object didn't stopped properly
 */
extern int IhmStop();

/**
 * @brief Ihm singleton destructor
 *
 * @retval 0 If the destruction worked
 * @retval -1 if the destruction didn't work
 */
extern int IhmFree();

#endif //IHM_H
