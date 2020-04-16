//
// Created by cleme on 05/02/2020.
//

#ifndef ROBOCOM_H
#define ROBOCOM_H

#include "../util.h"

#define SERVER_PORT 8080
#define SA struct sockaddr

// ---------- Enums declarations ----------

ENUM_DECL(DIRECTION, STOP, RIGHT, LEFT, FORWARD, BACKWARD)
ENUM_DECL(COMMAND, C_LEFT, C_RIGHT, C_FORWARD, C_BACKWARD, C_STOP, C_LOGS, C_STATE, C_QUIT)
ENUM_DECL(RQ_TYPE, RQ_GET, RQ_POST, RQ_PUT, RQ_DELETE, RQ_ES)


// ---------- Struct declarations ----------

typedef struct {
    RQ_TYPE rq_type;
    COMMAND command;
} RQ_data;

/**
 * @brief Wrapper enum. It is used to send events and parameters in a mailBox.
 */
typedef union {
    RQ_data request; ///< Message sent, interpreted as a structure
    char toString[sizeof(RQ_data)]; ///< Message sent, interpreted as a char array
} RQ_Wrapper;


// ---------- Functions prototypes ----------

extern void send_network(int socket, RQ_data data_to_send);
extern RQ_data read_network(int socket);


#endif //ROBOCOM_H
