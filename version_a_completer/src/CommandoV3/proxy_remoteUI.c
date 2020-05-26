//
// Created by gwendal on 26/05/2020.
//

#include "remoteui.h"

/*----------------------- STATIC VARIABLES -----------------------*/

/**
 * @var int myCurrentSocket
 * @brief Current socket to which the next message will be sent
 */
static int myCurrentSocket = 0;

/* ----------------------- FUNCTIONS ----------------------- */

extern void RemoteUI_setEvents(){

}

extern void RemoteUI_setEventsCount(int nbEvents){

}

extern void Proxy_RemoteUI_store_Socket(int socket){
    myCurrentSocket = socket; ///< Assign the new socket
}



