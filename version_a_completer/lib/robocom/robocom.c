//
// Created by cleme on 05/02/2020.
//

#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include "robocom.h"

extern int createNwk(int nwkPort) {
    struct sockaddr_in serverAddr;
    int createSocket = socket(AF_INET, SOCK_STREAM, 0); // TODO error handling

    int option = 1;
    setsockopt(createSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // Initialize IP and Port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nwkPort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(createSocket, (SA*)&serverAddr, sizeof(serverAddr)); // TODO error handling
    listen(createSocket, MAX_PENDING_CONNECTIONS);

    return createSocket;
}

extern int connectNwk(char * nwkIp, int nwkPort) {
    struct sockaddr_in serverAddr;
    int connectSocket = socket(AF_INET, SOCK_STREAM, 0); // TODO error handling

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nwkPort);
    serverAddr.sin_addr = *((struct in_addr *)gethostbyname (nwkIp)->h_addr_list[0]);

    connect(connectSocket, (SA*)&serverAddr, sizeof(serverAddr)); // TODO error handling

    return connectSocket;
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
