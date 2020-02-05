//
// Created by cleme on 05/02/2020.
//

#include "client.h"
#include "remoteui.h"


/**
 * Send a msg
 */
extern void sendMsg() {}

/**
 * Read a msg
 */
extern void readMsg() {}


/**
 * Start the client
 */
extern void Client_start() {
    RemoteUI_start();
}

/**
 * Stop the client
 */
extern void Client_stop() {
    RemoteUI_stop();
}