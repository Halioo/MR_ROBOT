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
    ihm_new();
    ihm_start();
    ihm_stop();
    ihm_free();
    return 0;
}
