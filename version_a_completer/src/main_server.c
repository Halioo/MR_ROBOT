//
// Created by cleme on 05/02/2020.
//

#include "CommandoV3/server.h"
#include "pilot.h"
#include "logger.h"

/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
//    Server_start();
//    Server_stop();
//    Pilot * lePilote = Pilot_new();
//    Pilot_Start(lePilote);
//
//    VelocityVector vel = {
//            .power = 80,
//            .dir = FORWARD
//    };
//    Pilot_setRobotVelocity(lePilote,vel);
//
//    Pilot_Stop(lePilote);
//    Pilot_Free(lePilote);

    Logger * myLogger = Logger_new();
    Logger_test(myLogger);

    return 0;
}