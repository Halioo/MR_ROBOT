//
// Created by cleme on 05/02/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include "robocom.h"
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
static Command list_commands[COMMANDS_NUMBER] =
{
    {&args_dir_left, (f_ptr_generic) &ask_mvt},
    {&args_dir_right, (f_ptr_generic) &ask_mvt},
    {&args_dir_forward, (f_ptr_generic) &ask_mvt},
    {&args_dir_backward, (f_ptr_generic) &ask_mvt},
    {&args_dir_stop, (f_ptr_generic) &ask_mvt},
    {NULL, NULL},
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
 * Send a msg
 */
extern void Server_sendLogs(int speed, int collision, float luminosity) {
    Command_order data_to_send;
    Robot_logs r_logs = {
        .speed = speed,
        .collision = collision,
        .luminosity = luminosity
    };
    data_to_send.r_logs = r_logs;
    if (write(socket_listen, &data_to_send, sizeof(data_to_send)) <0) {
        perror("--- Sending message failed !\n");
    } else {
        printf("Message sent successfully...\n");
    }
}

/**
 * Reads msg
 */
static void Server_readMsg(int socket) {
    Command_order command_rcvd;
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
        perror("Socket creation failed!\n");
        //exit(EXIT_FAILURE);
    } else {
        printf("Socket creation successful...\n");
    }

    int yes = 1;
    if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
        perror("Socket option configuration failed!\n");
        //exit(EXIT_FAILURE);
    } else {
        printf("Socket option configuration successful...\n");
    }

    // Initialize IP and Port
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to IP, exit if fail
    if (bind(socket_listen, (SA*)&server_address, sizeof(server_address)) != 0) {
        perror("Socket binding failed!\n");
        //exit(EXIT_FAILURE);
    } else {
        printf("Socket binding successful...\n");
    }
    // Begin listening, exit if fail
    if (listen(socket_listen, MAX_PENDING_CONNECTIONS) != 0) {
        perror("Listen failed!\n");
        //exit(EXIT_FAILURE);
    } else {
        printf("Server listening...\n");
    }
}

static void run() {
    int socket_data;
    // Accept data
    socket_data = accept(socket_listen, NULL, 0);
    while (FLAG_STOP == OFF) {
        // Check data integrity, exit if fail
        if (socket_data < 0) {
            perror("Server accept failed!\n");
            //exit(EXIT_FAILURE);
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
    Pilot_start();
    init();
    run();
}

/**
 * Stops the server
 */
extern void Server_stop() {
    Pilot_stop();
    int error;
    error = close(socket_listen);
    printf("Closing server...");
    printf("%d", error);
    exit(EXIT_SUCCESS);
}
