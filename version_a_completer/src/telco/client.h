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
extern void sendMsg();

/**
 * Reads a message
 */
extern void readMsg();


#endif //MR_ROBOT_CLIENT_H
