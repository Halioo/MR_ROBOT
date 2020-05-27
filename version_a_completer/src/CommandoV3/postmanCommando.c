//
// Created by ludovic on 27/05/2020.
//

#include <sys/types.h>
#include <sys/socket.h>
#include "postmanCommando.h"
#include "../../lib/include/robocom.h"

static int socketServeur;
static int socketCommunication;

extern int PostmanCommando_createNetwork(){
    socketServeur = createNwk(SERVER_PORT);
    return socketServeur;
}

extern int PostmanCommando_accept(){
    socketCommunication = accept(socketServeur,NULL,0);
    return socketCommunication;
}

extern int PostmanCommando_getSocketComm(){
    return socketCommunication;
}










