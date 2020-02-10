//
// Created by cleme on 05/02/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include "server.h"
#include "pilot.h"

#define MAX_PENDING_CONNECTIONS (5)

#define SA struct sockaddr


typedef enum {OFF=0, ON=1} Flag;

Flag FLAG_STOP = OFF;
static int socket_listen;
struct sockaddr_in server_address;


/**
 * Sends msg
 */
static void sendMsg(Data data) {}

/**
 * Reads msg
 */
static void readMsg(int socket) {
    // TODO gestion des inputs possibles --> librairie de gestion des coms ?
    Data data;
    read(socket, &data, sizeof(data));
    data.id = ntohl(data.id);
    printf("COMMUNICATION RECEIVED :: id = %d\n", data.id);
    close(socket);
    exit(0);
}

/**
 * Initialize socket's params: address, port...
 * Bind the socket to the address
 * Begin listening
 */
static void init()
{
    // Socket creation and exit if fail
    socket_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_listen == -1) {
        printf("Socket creation failed!\n");
        exit(0);
    } else {
        printf("Socket creation successful...\n");
    }

    // Initialize IP and Port
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to IP, exit if fail
    if (bind(socket_listen, (SA*)&server_address, sizeof(server_address)) != 0) {
        printf("Socket binding failed!\n");
        exit(0);
    } else {
        printf("Socket binding successful...\n");
    }
    // Begin listening, exit if fail
    if (listen(socket_listen, MAX_PENDING_CONNECTIONS) != 0) {
        printf("Listen failed!\n");
        exit(0);
    } else {
        printf("Server listening...\n");
    }
}


/**
 * Starts the server
 */
extern void Server_start() {
    //Pilot_start();
    init();
    while (FLAG_STOP == 0) {
        printf("COUCOU ON RECOMMENCE UN WHILE\n");
        int socket_data;
        // Accept data
        socket_data = accept(socket_listen, NULL, 0);
        // Check data integrity, exit if fail
        if (socket_data < 0) {
            printf("Server accept failed!\n");
            exit(0);
        } else {
            printf("Server accepted client's request...\n");
            if (fork() == 0) {
                readMsg(socket_data);
            }
        }
    }
}

/**
 * Stops the server
 */
extern void Server_stop() {
    //Pilot_stop();
    int error;
    error = close(socket_listen);
    printf("Closing server...");
    printf("%d", error);
    exit(0);
}
