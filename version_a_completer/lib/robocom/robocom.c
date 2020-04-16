//
// Created by cleme on 05/02/2020.
//

#include <unistd.h>

#include "robocom.h"


extern RQ_data read_from_network(int socket) {
    RQ_Wrapper data;
    // TODO error handling
    read(socket, &data.toString, sizeof(RQ_Wrapper));
    return data.request;
}

extern void send_to_network(int socket, RQ_data data_to_send) {
    RQ_Wrapper data;
    data.request = data_to_send;
    // TODO error handling
    write(socket, &data.toString, sizeof(RQ_Wrapper));
}
