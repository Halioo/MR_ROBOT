//
// Created by cleme on 05/02/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include "../../lib/include/util.h"
#include "../../lib/include/robocom.h"

#include "server.h"
//#include "../../lib/include/pilot.h"
#include "pilot.h"

#include "commands_functions.h"


typedef void (*f_ptr_generic)(void);
typedef void (*f_ptr_dir)(DIRECTION dir);

/**
 * Structure contenant les arguments qu'il
 * est possible de passer dans les fonctions
 */
typedef struct {
    DIRECTION dir;
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
static Command list_commands[NB_COMMAND] =
{
    {&args_dir_left, (f_ptr_generic) &ask_mvt},
    {&args_dir_right, (f_ptr_generic) &ask_mvt},
    {&args_dir_forward, (f_ptr_generic) &ask_mvt},
    {&args_dir_backward, (f_ptr_generic) &ask_mvt},
    {&args_dir_stop, (f_ptr_generic) &ask_mvt},
    {NULL, NULL},
    {NULL, &ask4log}
};


FLAG FLAG_STOP = DOWN;
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
    RQ_data command_rcvd;
    read(socket, &command_rcvd, sizeof(command_rcvd));
    command_rcvd.command = ntohl(command_rcvd.command);
    // printf("COMMUNICATION RECEIVED :: id = %d\n", command_rcvd.command);
    Command command_to_exec = list_commands[command_rcvd.command];
    if (command_to_exec.command_args == NULL) {
        command_to_exec.func();
    } else {
        ((f_ptr_dir)command_to_exec.func)(command_to_exec.command_args->dir);
    }
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

    int option = 1;
    setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

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

static void run() {// TODO : Write all the ACTION functions

    int socket_data;
    // Accept data
    socket_data = accept(socket_listen, NULL, 0);
    while (FLAG_STOP == DOWN) {
        // Check data integrity, exit if fail
        if (socket_data < 0) {
            printf("Server accept failed!\n");
            exit(0);
        } else {
            printf("Server accepted client's request...\n");
            Server_readMsg(socket_data);
        }
    }
}


/**
 * Starts the server
 */
extern void Server_start() {
    Pilot * myPilot = Pilot_new();
    Pilot_Start(myPilot);

    init();
    run();
}

/**
 * Stops the server
 */
extern void Server_stop() {
//    Pilot_Stop();
    int error;
    error = close(socket_listen);
    printf("Closing server...");
    printf("%d", error);
    exit(0);
}
