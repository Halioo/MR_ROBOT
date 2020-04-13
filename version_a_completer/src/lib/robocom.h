//
// Created by cleme on 05/02/2020.
//

#ifndef MR_ROBOT_ROBOCOM_H
#define MR_ROBOT_ROBOCOM_H


#define SERVER_PORT 8080


typedef enum
{
    C_LEFT,
    C_RIGHT,
    C_FORWARD,
    C_BACKWARD,
    C_STOP,
    C_LOGS,
    C_STATE,
    C_QUIT,
    COMMANDS_NUMBER
} COMMANDS;

typedef enum
{
    R_NULL,
    R_GET,
    R_POST,
    R_ANSWER,
    REQUEST_TUPE_NUMBER
} REQUEST_TYPE;

typedef struct {
    int speed;
    int collision;
    float luminosity;
} Robot_logs;

typedef struct
{
    REQUEST_TYPE r_type;
    COMMANDS command;
    Robot_logs r_logs;
} Command_order;



#endif //MR_ROBOT_ROBOCOM_H
