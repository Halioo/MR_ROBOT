//
// Created by cleme on 05/02/2020.
//

#include "TelcoV3/ihm.h"
#include "../lib/include/remoteui.h"
#include "TelcoV3/dispatcher_client.h"
#include "TelcoV3/client.h"
#include "TelcoV3/postmanTelco.h"
#include "logger.h"
#include "pilot.h"

/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
<<<<<<< 57307e2cfd59915ce32ccb843a4519e237e690de
//    Ihm * monIhm = IhmNew();
//    IhmStart(monIhm);
//    IhmStop(monIhm);
//    IhmFree(monIhm);
    RemoteUI * myRemoteUI = RemoteUI_new();
    RemoteUI_start(myRemoteUI);
=======
    /*Ihm * monIhm = IhmNew();
    IhmStart(monIhm);
    IhmStop(monIhm);
    IhmFree(monIhm);
     */

    Client_start();

    RemoteUI *remoteUi=RemoteUI_new();
    RemoteUI_start(remoteUi);

    Dispatcher  *dispatcher=Dispatcher_New(remoteUi);
    Dispatcher_Start(dispatcher);

    Ihm *ihm=IhmNew();
    IhmStart(ihm);

    PostmanTelco_start();

    PostmanTelco_stop();

>>>>>>> début de main_client


    return 0;
}
