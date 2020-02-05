//
// Created by cleme on 05/02/2020.
//

#ifndef MR_ROBOT_SERVER_H
#define MR_ROBOT_SERVER_H


#define MAX_PENDING_CONNECTIONS (5)
#define SERVER_PORT (8080)

/**
 * Starts the server
 */
extern void Server_start();

/**
 * Stops the server
 */
extern void Server_stop();


#endif //MR_ROBOT_SERVER_H
