//
// Created by cleme on 05/02/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

#include "client.h"
#include "remoteui.h"


#define SA struct sockaddr


static int socket_to_connect;
static struct sockaddr_in server_address;


static void init()
{
    socket_to_connect = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_to_connect == -1) {
        printf("Socket creation failed!\n");
        exit(0);
    } else {
        printf("Socket creation successful...\n");
    }
    // Initialize IP and Port
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr = *((struct in_addr *)gethostbyname (SERVER_ADRESS)->h_addr_list[0]);
    // Connection request to server, exit if fail
    if (connect(socket_to_connect, (SA*)&server_address, sizeof(server_address)) != 0) {
        printf("connection with the server failed!\n");
        exit(0);
    } else {
        printf("connected to the server...\n");
    }
}


/**
 * Send a msg
 */
extern void sendMsg() {}

/**
 * Read a msg
 */
extern void readMsg() {}


/**
 * Start the client
 */
extern void Client_start() {
    RemoteUI_start();
    init();
}

/**
 * Stop the client
 */
extern void Client_stop() {
    RemoteUI_stop();
    close(socket_to_connect);
}