//
// Created by cleme on 05/02/2020.
//

#include "TelcoV3/ihm.h"
#include "../lib/include/remoteui.h"
#include "TelcoV3/postmanTelco.h"


/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
//    Ihm * monIhm = IhmNew();
//    IhmStart(monIhm);
//    IhmStop(monIhm);
//    IhmFree(monIhm);

    int mySocket = PostmanTelco_createNwkClient("127.0.0.1");

    RemoteUI * myRemoteUI = RemoteUI_new();
    RemoteUI_start(myRemoteUI);

    RemoteUI_stop(myRemoteUI);
    RemoteUI_free(myRemoteUI);

    return 0;
}
