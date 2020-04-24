//
// Created by cleme on 10/02/2020.
//

#include <stdio.h>
#include "commands_functions.h"

#define DEFAULT_POWER_FWD 80
#define DEFAULT_POWER_BCKWD 60
#define DEFAULT_POWER_TURN 50

/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
extern void ask_mvt(DIRECTION dir)
{
    VelocityVector vel = {
            .dir = dir,
    };
    switch (dir) {
        case FORWARD:
            vel.power = DEFAULT_POWER_FWD;
            break;
        case BACKWARD:
            vel.power = DEFAULT_POWER_BCKWD;
            break;
        case LEFT:
        case RIGHT:
            vel.power = DEFAULT_POWER_TURN;
            break;
        default:
            vel.power = 0;
    }
    Pilot_setVelocity(vel);
}

/**
 * Demande l'actualisation du pilote, récupère
 * ses états et les affiche dans la console
 */
extern void ask4log()
{
    Pilot_check();
    PilotState pt = Pilot_getState();
    //clear_logs();
    //printf(get_msg(MSG_LOGS), pt.speed, pt.collision, pt.luminosity);
    printf("%d :: %d :: %f\n", pt.speed, pt.collision, pt.luminosity);
}