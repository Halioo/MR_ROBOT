//
// Created by cleme on 05/02/2020.
//

#ifndef MR_ROBOT_CLIENT_H
#define MR_ROBOT_CLIENT_H

#include "../lib/robocom.h"

#define SERVER_ADRESS "localhost"


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
extern void Client_sendMsg(Command_order data);

/**
 * Reads a message
 */
extern void Client_readMsg();


#endif //MR_ROBOT_CLIENT_H
