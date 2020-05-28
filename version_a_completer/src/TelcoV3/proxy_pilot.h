//
// Created by ludovic on 28/05/2020.
//

#ifndef MR_ROBOT_PROXY_PILOT_H
#define MR_ROBOT_PROXY_PILOT_H

#include "robocom.h"

typedef struct Pilot_t Pilot;

extern void Proxy_pilot_setRobotVelocity(Pilot * unused, VelocityVector velocity);
extern void Proxy_pilot_ToggleES(Pilot * this);
extern void Proxy_pilot_quit(Pilot * this);

#endif //MR_ROBOT_PROXY_PILOT_H
