//
// Created by ludovic on 27/05/2020.
//


#include "postmanTelco.h"
#include "robocom.h"

static int socketCommunication;

extern int PostmanTelco_start(char * ip){
    socketCommunication = connectNwk(ip,SERVER_PORT);
    return socketCommunication;
}

extern int PostmanTelco_getSocketComm(){
    return socketCommunication;
}










