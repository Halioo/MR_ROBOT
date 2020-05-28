//
// Created by cleme on 05/02/2020.
//

#include "CommandoV3/server.h"
//#include "../lib/include/pilot.h"
#include "CommandoV3/pilot.h"
#include "../lib/include/logger.h"
#include "CommandoV3/ihm.h"
#include "CommandoV3/dispatcher_server.h"
#include "CommandoV3/robot.h"
#include "CommandoV3/postmanCommando.h"


/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
//    PostmanCommando_createNetwork();
//    TRACE("Network server créé\n")
//    PostmanCommando_accept();
//    TRACE("Network server accepté connexion\n")

    Robot_start(); // appelle Robot new

    Pilot * myPilot = Pilot_new();
    Pilot_Start(myPilot);

    Logger * myLogger = Logger_new();
    Logger_start(myLogger);

    Dispatcher * myDispatcher = Dispatcher_New(myPilot,myLogger);
    Dispatcher_Start(myDispatcher);
    TRACE("C BON\n")

    AdminUI * myAdminUI = AdminUI_new(myPilot,myLogger);
    AdminUI_start(myAdminUI);

    //Debut
    Dispatcher_startListening(myDispatcher);

//    Ihm * myIhm = IhmNew(myAdminUI);
//    IhmStart(myIhm);

    AdminUI_stop(myAdminUI);
    AdminUI_free(myAdminUI);

    Dispatcher_Stop(myDispatcher);
    Dispatcher_Free(myDispatcher);

    Logger_stop(myLogger);
    Logger_free(myLogger);

    Pilot_Stop(myPilot);
    Pilot_Free(myPilot);

    Robot_stop(); // appelle le robot free


    return 0;
}