//
// Created by ludovic on 27/05/2020.
//


#include "postmanTelco.h"
#include "../../lib/include/robocom.h"

static int socketCommunication;

extern int PostmanTelco_createNwkClient(char * ip){
    socketCommunication = createNwkClient(ip, SERVER_PORT);
    return socketCommunication;
}

extern int PostmanTelco_connectClient(int socket){
    int test = connectNwkClient(socket);
    return test;
}


extern int PostmanTelco_getSocketComm(){
    return socketCommunication;
}










