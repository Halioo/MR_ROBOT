//
// Created by cleme on 24/05/2020.
//

#include "pilot.h"
#include "postmanTelco.h"
#include "robocom.h"


/* ----------------------- FUNCTIONS ----------------------- */


extern void Pilot_setRobotVelocity(Pilot * unused, VelocityVector velocity){
    RQ_data msgToSend = {
            .vel = velocity,
            .rq_type = RQ_SET_VEL
    };
    sendNwk(PostmanTelco_getSocketComm(),msgToSend);
}


extern void Pilot_ToggleES(Pilot * this){
    RQ_data msgToSend = {
            .rq_type = RQ_TOGGLE_ES
    };
    sendNwk(PostmanTelco_getSocketComm(),msgToSend);
}
