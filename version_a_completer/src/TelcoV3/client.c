//
// Created by cleme on 05/02/2020.
//

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>



#include "client.h"


static int socket_to_connect;
static struct sockaddr_in server_address;


static void init()
{
    socket_to_connect = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_to_connect == -1) {
        TRACE("Socket creation failed!\n")
    } else {
        TRACE("Socket creation successful...\n")
    }
    // Initialize IP and Port
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    //server_address.sin_addr = *((struct in_addr *)gethostbyname (SERVER_ADRESS)->h_addr_list[0]);
    // Connection request to server, exit if fail
    if (connect(socket_to_connect, (SA*)&server_address, sizeof(server_address)) != 0) {
        TRACE("connection with the server failed!\n")
    } else {
        TRACE("connected to the server...\n")
    }
}


/**
 * Send a msg
 */
extern void Client_sendMsg(RQ_data data) {
    RQ_data data_to_send;
    data_to_send.command = htonl(data.command);
    if (write(socket_to_connect, &data_to_send, sizeof(data_to_send)) <0) {
        TRACE("--- Sending message failed !\n")
        close(socket_to_connect);
    } else {
        TRACE("Message sent successfully...\n")
    }
}

/**
 * Read a msg
 */
extern void Client_readMsg() {}


/**
 * Start the client
 */
extern void Client_start() {
    init();
}

/**
 * Stop the client
 */
extern void Client_stop() {
    close(socket_to_connect);
}
