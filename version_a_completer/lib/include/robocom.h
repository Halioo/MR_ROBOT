//
// Created by cleme on 05/02/2020.
//

#ifndef ROBOCOM_H
#define ROBOCOM_H


#include "util.h"
#include "watchdog.h"


#define SA struct sockaddr

#define SERVER_PORT 8080
#define MAX_PENDING_CONNECTIONS (5)

#define DEFAULT_POWER_FWD 80
#define DEFAULT_POWER_BCKWD 60
#define DEFAULT_POWER_TURN 50


// ---------- Enums declarations ----------

ENUM_DECL(DIRECTION, STOP, RIGHT, LEFT, FORWARD, BACKWARD)
<<<<<<< 022bef0ad0266274ba0544e150c8c0be35fe9a8b
ENUM_DECL(COMMAND, C_LEFT, C_RIGHT, C_FORWARD, C_BACKWARD, C_STOP, C_LOGS, C_STATE, C_QUIT, C_CLEAR, C_ES, C_NOP, C_GET_EVENTS, C_GET_EVENTS_NUMBER)
=======
ENUM_DECL(COMMAND, C_LEFT, C_RIGHT, C_FORWARD, C_BACKWARD, C_STOP, C_LOGS, C_STATE, C_EVENTSCOUNT, C_EVENTS, C_QUIT, C_CLEAR, C_ES, C_NOP)
>>>>>>> Implémentation du proxy_logger
ENUM_DECL(RQ_TYPE, RQ_GET, RQ_POST, RQ_PUT, RQ_DELETE, RQ_ES)


// ---------- Struct declarations ----------

typedef struct
{
    DIRECTION dir;
    int power;
} VelocityVector;

/** the captor's states of the robot (bumper and luminosity) */
typedef struct
{
    FLAG collision_f;
    float luminosity;
} SensorState;

typedef struct {
    RQ_TYPE rq_type;
    COMMAND command;
<<<<<<< 022bef0ad0266274ba0544e150c8c0be35fe9a8b
    EVENT logEvent[];
=======
    int from;
    int to;
>>>>>>> Implémentation du proxy_logger
} RQ_data;

/**
 * @brief Wrapper enum. It is used to send events and parameters in a mailBox.
 */
typedef union {
    RQ_data request; ///< Message sent, interpreted as a structure
    char toString[sizeof(RQ_data)]; ///< Message sent, interpreted as a char array
} RQ_Wrapper;


// ---------- Functions prototypes ----------

extern int createNwk(int nwkPort);
extern int connectNwk(char * nwkIp, int nwkPort);
extern RQ_data readNwk(int socket);
extern void sendNwk(int socket, RQ_data data_to_send);

/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
extern VelocityVector translateDir(DIRECTION);



#endif //ROBOCOM_H
