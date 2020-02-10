//
// Created by cleme on 05/02/2020.
//

#ifndef MR_ROBOT_ROBOCOM_H
#define MR_ROBOT_ROBOCOM_H


#define SERVER_PORT 8080


typedef enum {
    C_LEFT,
    C_RIGHT,
    C_FORWARD,
    C_BACKWARD,
    C_STOP,
    C_LOGS,
    C_STATE,
    C_QUIT,
    COMMANDS_NUMBER,
} COMMANDS;


typedef struct
{
    COMMANDS command;
} Command_order;


#endif //MR_ROBOT_ROBOCOM_H
