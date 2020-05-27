#ifndef DISPATCHER_CLIENT_H
#define DISPATCHER_CLIENT_H

#include <pthread.h>

#include "../../lib/include/util.h"
#include "../../lib/include/robocom.h"
#include "../../lib/include/mailbox.h"
#include "../../lib/include/remoteui.h"
#include "../../lib/include/mailbox.h"

typedef struct Dispatcher_t Dispatcher;

Dispatcher * Dispatcher_New(RemoteUI * myRemoteUI);

extern int Dispatcher_Start(Dispatcher * this);

extern int Dispatcher_Stop(Dispatcher * this);

extern int Dispatcher_Free(Dispatcher * this);

#endif //DISPATCHER_CLIENT_H
