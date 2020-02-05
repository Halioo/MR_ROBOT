//
// Created by cleme on 05/02/2020.
//

#ifndef MR_ROBOT_CLIENT_H
#define MR_ROBOT_CLIENT_H

#define SERVER_ADRESS "localhost"
#define SERVER_PORT 8080

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
