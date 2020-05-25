//
// Created by cleme on 05/02/2020.
//

#ifndef CLIENT_H
#define CLIENT_H

#include "../../lib/include/watchdog.h"
#include "../../lib/include/robocom.h"


/**
 * Starts the client
 */
extern void Client_start();

/**
 * Stops the client
 */
extern void Client_stop();

/**
 * Sends a message
 */
extern void Client_sendMsg(RQ_data data);

/**
 * Reads a message
 */
extern void Client_readMsg();


#endif //CLIENT_H
