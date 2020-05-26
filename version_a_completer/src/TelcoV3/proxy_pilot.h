//
// Created by cleme on 24/05/2020.
//

#ifndef PROXY_PILOT_H
#define PROXY_PILOT_H

#include "util.h"
#include <robocom.h>

extern void Pilot_toggleES();
extern void Pilot_SetVelocity(VelocityVector vel);
extern void Proxy_Pilot_store_Socket(int socket);

#endif //PROXY_PILOT_H
