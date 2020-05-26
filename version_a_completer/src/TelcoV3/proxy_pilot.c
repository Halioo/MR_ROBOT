//
// Created by cleme on 24/05/2020.
//

#include "proxy_pilot.h"

/*----------------------- STATIC VARIABLES -----------------------*/
/**

 * @var int myCurrentSocket

 * @brief Current socket to which the next message will be sent

 */

static int myCurrentSocket;

/* ----------------------- FUNCTIONS ----------------------- */

extern void ProxyPilot_new(int socket){
    myCurrentSocket = socket;
}

extern void Pilot_toggleES() {
    RQ_data data={
            //.rq_type=RQ_ES
            .command=C_ES
    };
    sendNwk(myCurrentSocket,data);
    TRACE("to: Pilot // msg: toggleES\n")
}

extern void Pilot_SetVelocity(VelocityVector vel) {
    DIRECTION dir=vel.dir;
    COMMAND command;
    switch (dir){
        case FORWARD:
            command=C_FORWARD;
            break;
        case BACKWARD:
            command=C_BACKWARD;
            break;
        case LEFT:
            command=C_LEFT;
            break;
        case RIGHT:
            command=C_RIGHT;
            break;
        case STOP:
            command=C_STOP;
            break;
        default:
            command=C_NOP;
    }
    RQ_data data={
            .command=command
            //.rq_type=RQ_PUT //TODO ou RQ_POST? j'ai du mal à faire la différence
    };
    sendNwk(myCurrentSocket,data);
    TRACE("to: Pilot // msg: setVelocity")
}
