//
// Created by cleme on 05/02/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include "../lib/robocom.h"
#include "server.h"
#include "pilot.h"
#include "commands_functions.h"

#define MAX_PENDING_CONNECTIONS (5)

#define SA struct sockaddr


typedef void (*f_ptr_generic)(void);
typedef void (*f_ptr_dir)(Direction dir);

/**
 * Structure contenant les arguments qu'il
 * est possible de passer dans les fonctions
 */
typedef struct {
    Direction dir;
} Command_args;

// Liste des arguments possibles (ici les différentes directions)
static Command_args args_dir_left = {.dir = LEFT};
static Command_args args_dir_right = {.dir = RIGHT};
static Command_args args_dir_forward = {.dir = FORWARD};
static Command_args args_dir_backward = {.dir = BACKWARD};
static Command_args args_dir_stop = {.dir = STOP};

typedef struct {
    Command_args * command_args;
    f_ptr_generic func;
} Command;

// Array regroupant les différentes commandes possibles
static Command list_commands[] =
        {
                {&args_dir_left, (f_ptr_generic) &ask_mvt},
                {&args_dir_right, (f_ptr_generic) &ask_mvt},
                {&args_dir_forward, (f_ptr_generic) &ask_mvt},
                {&args_dir_backward, (f_ptr_generic) &ask_mvt},
                {&args_dir_stop, (f_ptr_generic) &ask_mvt},
                {NULL, &ask4log}
        };


typedef enum {OFF=0, ON=1} Flag;

Flag FLAG_STOP = OFF;
static int socket_listen;
struct sockaddr_in server_address;





/**
 * Sends msg
 */
extern void Server_sendMsg() {}

/**
 * Reads msg
 */
static void Server_readMsg(int socket) {
    // TODO gestion des inputs possibles --> librairie de gestion des coms ?
    Command_order command_rcvd;
    read(socket, &command_rcvd, sizeof(command_rcvd));
    command_rcvd.command = ntohl(command_rcvd.command);
    printf("COMMUNICATION RECEIVED :: id = %d\n", command_rcvd.command);
    switch (command_rcvd.command) {
        case C_LEFT:
            ask_mvt(LEFT);
            break;
        case C_RIGHT:
            break;
        case C_FORWARD:
            break;
        case C_BACKWARD:
            break;
        case C_STOP:
            break;
        case C_LOGS:
            break;
        case C_STATE:
            break;
        case C_QUIT:
            break;
        case COMMANDS_NUMBER:
            break;
    }

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


static void run() {
    int socket_data;
    while (FLAG_STOP == OFF) {
        // Accept data
        socket_data = accept(socket_listen, NULL, 0);
        // Check data integrity, exit if fail
        if (socket_data < 0) {
            printf("Server accept failed!\n");
            exit(0);
        } else {
            printf("Server accepted client's request...\n");
            if (fork() == 0) {
                Server_readMsg(socket_data);
            }
        }
    }
}


/**
 * Starts the server
 */
extern void Server_start() {
    //Pilot_start();
    init();
    run();
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
