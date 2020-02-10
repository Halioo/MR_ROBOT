//
// Created by cleme on 05/02/2020.
//

#ifndef MR_ROBOT_ROBOCOM_H
#define MR_ROBOT_ROBOCOM_H


#define SERVER_PORT 8080


typedef enum {
    SET_VEL,
    CHECK
} COMMANDS;


typedef struct
{
    int id;
} Data;


#endif //MR_ROBOT_ROBOCOM_H
