//
// Created by cleme on 05/02/2020.
//

#include "remoteui.h"


/**
 * starts the robot V1 application
 */
int main (int argc, char *argv[])
{
    RemoteUI *remoteUi=RemoteUI_new();
    RemoteUI_start(remoteUi);
    RemoteUI_stop(remoteUi);
    RemoteUI_free(remoteUi);
    return 0;
}
