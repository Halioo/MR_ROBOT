//
// Created by cleme on 05/02/2020.
//

#include "TelcoV3/ihm.h"
#include "../lib/include/remoteui.h"
#include "TelcoV3/dispatcher_client.h"
#include "TelcoV3/client.h"
#include "TelcoV3/postmanTelco.h"
#include "logger.h"
//#include "pilot.h"
#include "unistd.h"

/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
//    PostmanTelco_createNwkClient("127.0.0.1");
//    TRACE("Network client créé\n")
//    PostmanTelco_connectClient();
//    TRACE("Network client connecté\n")

    /*Ihm * monIhm = IhmNew();
    IhmStart(monIhm);
    IhmStop(monIhm);
    IhmFree(monIhm);
     */


    RemoteUI *remoteUi=RemoteUI_new();
    RemoteUI_start(remoteUi);

    RemoteUI_setIp(remoteUi,"127.0.0.1");
    RemoteUI_validate(remoteUi);

    sleep(1);
    RemoteUI_setDir(remoteUi,FORWARD);
    sleep(1);
    RemoteUI_setDir(remoteUi,BACKWARD);
    sleep(1);
    RemoteUI_setDir(remoteUi,LEFT);
    sleep(1);
    RemoteUI_setDir(remoteUi,RIGHT);




//    Dispatcher  *dispatcher=Dispatcher_New(remoteUi);
//    Dispatcher_Start(dispatcher);

//
//    IhmNew();
//    IhmStart();
//
//    PostmanTelco_start(ip);
//    PostmanTelco_stop();
//
//    IhmStop();
//    IhmFree();
//
//    Dispatcher_Stop(dispatcher);
//    Dispatcher_Free(dispatcher);

    RemoteUI_stop(remoteUi);
    RemoteUI_free(remoteUi);





    return 0;
}
