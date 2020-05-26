//
// Created by cleme on 10/02/2020.
//

#include <stdio.h>
#include "commands_functions.h"
#include "pilot.h"

#define DEFAULT_POWER_FWD 80
#define DEFAULT_POWER_BCKWD 60
#define DEFAULT_POWER_TURN 50

/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
extern void ask_mvt(DIRECTION dir)
{
//    Pilot_SetVelocity(translateDir(dir));
}

/**
 * Demande l'actualisation du pilote, récupère
 * ses états et les affiche dans la console
 */
extern void ask4log()
{
//    Pilot_check();
//    PilotState pt = Pilot_getState();
    //clear_logs();
    //printf(get_msg(MSG_LOGS), pt.speed, pt.collision, pt.luminosity);
//    printf("%d :: %d :: %f\n", pt.speed, pt.collision, pt.luminosity);
}