//
// Created by cleme on 05/02/2020.
//

#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "robocom.h"


#define DEFAULT_POWER_FWD 80
#define DEFAULT_POWER_BCKWD 60
#define DEFAULT_POWER_TURN 50

static struct sockaddr_in serverAddr;

/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
extern VelocityVector translateDir(DIRECTION dir) {
    VelocityVector vel = {
            .dir = dir,
    };
    switch (dir) {
        case FORWARD:
            vel.power = DEFAULT_POWER_FWD;
            break;
        case BACKWARD:
            vel.power = DEFAULT_POWER_BCKWD;
            break;
        case LEFT:
        case RIGHT:
            vel.power = DEFAULT_POWER_TURN;
            break;
        default:
            vel.power = 0;
    }
    return vel;
}



extern int createNwk(int nwkPort) {
    int createSocket = socket(AF_INET, SOCK_STREAM, 0); // TODO error handling

    int option = 1;
    setsockopt(createSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // Initialize IP and Port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nwkPort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(createSocket, (SA*)&serverAddr, sizeof(serverAddr)); // TODO error handling
    listen(createSocket, MAX_PENDING_CONNECTIONS);
    TRACE("Network créé\n")
    return createSocket;
}

extern int createNwkClient(char * nwkIp, int nwkPort) {
    struct sockaddr_in serverAddr;
    int connectSocket = socket(AF_INET, SOCK_STREAM, 0); // TODO error handling

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nwkPort);
    serverAddr.sin_addr.s_addr = inet_addr(nwkIp);

    return connectSocket;
}

extern int connectNwkClient(int socket){
    int test = connect(socket, (SA*)&serverAddr, sizeof(serverAddr)); // TODO error handling
    TRACE("Network connecté")
    return test;
}

extern RQ_data readNwk(int socket) {
    RQ_Wrapper data;
    // TODO error handling
    read(socket, &data.toString, sizeof(RQ_Wrapper));
    return data.request;
}

extern void sendNwk(int socket, RQ_data data_to_send) {
    RQ_Wrapper data;
    data.request = data_to_send;
    // TODO error handling
    write(socket, &data.toString, sizeof(RQ_Wrapper));
}

