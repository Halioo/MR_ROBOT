//
// Created by gwendal on 26/05/2020.
//

#ifndef MR_ROBOT_PROXY_REMOTEUI_H
#define MR_ROBOT_PROXY_REMOTEUI_H

#include <robocom.h>

extern void setIp(char * ip);
extern void toggleEmergencyStop();
extern void setDir(DIRECTION dir);
extern void validate();
extern void goScreenLog();
extern void backMainScreen();

extern void Proxy_Pilot_store_Socket(int socket);


#endif //MR_ROBOT_PROXY_REMOTEUI_H
