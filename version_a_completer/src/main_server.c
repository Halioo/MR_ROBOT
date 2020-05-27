//
// Created by cleme on 05/02/2020.
//

#include "CommandoV3/server.h"
#include "pilot.h"
#include "logger.h"
#include "CommandoV3/ihm.h"

/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
    Pilot * myPilot = Pilot_new();
    Pilot_Start(myPilot);
    Logger * myLogger = Logger_new();
    Logger_start(myLogger);
    Server_start();

    return 0;
}