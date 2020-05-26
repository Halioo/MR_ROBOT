//
// Created by gwendal on 26/05/2020.
//
#include "proxy_remoteUI.h"

/*----------------------- STATIC VARIABLES -----------------------*/

/**
 * @var int myCurrentSocket
 * @brief Current socket to which the next message will be sent
 */
static int myCurrentSocket = 0;

/* ----------------------- FUNCTIONS ----------------------- */
extern void setIp(char * ip){
    RQ_data data={
            //.rq_type=RQ_ES
            .command=C_SETIP,
            .argc=ip
    };
    sendNwk(myCurrentSocket,data);
}

extern void toggleEmergencyStop(){
    RQ_data data={
            //.rq_type=RQ_ES
            .command=C_ES
    };
    sendNwk(myCurrentSocket,data);
}

extern void setDir(DIRECTION dir){
    COMMAND command;
    switch (dir){
        case FORWARD:
            command=C_FORWARD;
            break;
        case BACKWARD:
            command=C_BACKWARD;
            break;
        case LEFT:
            command=C_LEFT;
            break;
        case RIGHT:
            command=C_RIGHT;
            break;
        case STOP:
            command=C_STOP;
            break;
        default:
            command=C_NOP;
    }
    RQ_data data={
            .command=command
            //.rq_type=RQ_PUT //TODO ou RQ_POST? j'ai du mal à faire la différence
    };
    sendNwk(myCurrentSocket,data);
}

extern void validate(){
    RQ_data data={
            //.rq_type=RQ_ES
            .command=C_VALIDATE
    };
    sendNwk(myCurrentSocket,data);
}

extern void goScreenLog(){
    RQ_data data={
            //.rq_type=RQ_ES
            .command=C_GOSCREENLOG
    };
    sendNwk(myCurrentSocket,data);
}

extern void backMainScreen(){
    RQ_data data={
            //.rq_type=RQ_ES
            .command=C_BACKMAINSCREEN
    };
    sendNwk(myCurrentSocket,data);
}


extern void Proxy_RemoteUI_store_Socket(int socket){
    myCurrentSocket = socket; ///< Assign the new socket
}



