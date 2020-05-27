//
// Created by gwendal on 27/05/2020.
//

#ifndef MR_ROBOT_IHM_H
#define MR_ROBOT_IHM_H

#include "../../lib/include/util.h"



ENUM_DECL(UI_SCREEN, SCREEN_MAIN, SCREEN_LOG, SCREEN_ERROR)

typedef struct Ihm_t Ihm;


/* ----------------------- PUBLIC FUNCTIONS PROTOTYPES -----------------------*/

extern void IhmDisplayMainScreen(Ihm * this);

extern void IhmDisplayLogScreen(Ihm * this);

extern void IhmQuit(Ihm *this);


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
extern Ihm * IhmNew();

/**
 * @brief Ihm class starter
 *
 * Starts the Ihm object
 *
 * @retval 0 If the start worked
 * @retval -1 If the start didn't work
 */
extern int IhmStart(Ihm * ihm);

/**
 * @brief Ihm singleton stopper
 *
 * @retval 0 If the object stopped properly
 * @retval -1 If the object didn't stopped properly
 */
extern int IhmStop(Ihm * ihm);

/**
 * @brief Ihm singleton destructor
 *
 * @retval 0 If the destruction worked
 * @retval -1 if the destruction didn't work
 */
extern int IhmFree(Ihm * ihm);


#endif //MR_ROBOT_IHM_H
