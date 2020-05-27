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

    IhmNew();
    IhmStart();

    //PostmanTelco_start(ip);//TODO je sais pas quelle adresse mettre
    //PostmanTelco_stop();//TODO y a toujours pas

    IhmStop();
    IhmFree();

    Dispatcher_Stop(dispatcher);
    Dispatcher_Free(dispatcher);

    RemoteUI_stop(remoteUi);
    RemoteUI_free(remoteUi);





    return 0;
}
