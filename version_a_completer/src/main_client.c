//
// Created by cleme on 05/02/2020.
//

#include "TelcoV3/ihm.h"
#include "../lib/include/remoteui.h"


/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
//    Ihm * monIhm = IhmNew();
//    IhmStart(monIhm);
//    IhmStop(monIhm);
//    IhmFree(monIhm);
    RemoteUI * myRemoteUI = RemoteUI_new();
    RemoteUI_start(myRemoteUI);


    return 0;
}
