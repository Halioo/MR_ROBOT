//
// Created by cleme on 05/02/2020.
//

#ifndef ROBOCOM_H
#define ROBOCOM_H


#include "util.h"
#include "watchdog.h"


#define SA struct sockaddr

#define SERVER_PORT 12345
#define MAX_PENDING_CONNECTIONS (5)

#define DEFAULT_POWER_FWD 80
#define DEFAULT_POWER_BCKWD 60
#define DEFAULT_POWER_TURN 50


// ---------- Enums declarations ----------

ENUM_DECL(DIRECTION, STOP, RIGHT, LEFT, FORWARD, BACKWARD)
ENUM_DECL(COMMAND, C_LEFT, C_RIGHT, C_FORWARD, C_BACKWARD, C_STOP, C_LOGS, C_STATE, C_EVENTSCOUNT, C_EVENTS, C_SETIP, C_VALIDATE, C_GOSCREENLOG, C_BACKMAINSCREEN, C_QUIT, C_CLEAR, C_ES, C_NOP)
ENUM_DECL(RQ_TYPE, RQ_ASK_EVENTS, RQ_ASK_EVENTS_NB, RQ_SET_EVENTS, RQ_SET_EVENT_NB, RQ_END_SET_EVENT, RQ_TOGGLE_ES, RQ_SET_VEL, RQ_STOP, RQ_QUIT)


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
    SensorState sens;
    int speed;
}LogEvent;

typedef struct {
    RQ_TYPE rq_type;
    COMMAND command;
    int from;
    int to;
    int eventsCount;
    LogEvent logEvent;
    VelocityVector vel;
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
extern int createNwkClient(char * nwkIp, int nwkPort);
extern RQ_data readNwk(int socket);
extern void sendNwk(int socket, RQ_data data_to_send);
extern int connectNwkClient(int socket);

/**
 * Transforme une direction en un VelocityVector
 * et l'envoie au pilote
 */
extern VelocityVector translateDir(DIRECTION);



#endif //ROBOCOM_H
