//
// Created by cleme on 10/02/2020.
//

#include "commands_functions.h"


/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
static void ask_mvt(Direction dir)
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