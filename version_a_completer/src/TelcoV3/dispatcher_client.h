#ifndef DISPATCHER_CLIENT_H
#define DISPATCHER_CLIENT_H

#include <pthread.h>

#include "util.h"
#include "robocom.h"
#include "mailbox.h"
#include "remoteui.h"
#include "../../lib/include/mailbox.h"

typedef struct Dispatcher_t Dispatcher;

Dispatcher * Dispatcher_New();

int Dispatcher_Start(Dispatcher * this);

int Dispatcher_Stop(Dispatcher * this);

int Dispatcher_Free(Dispatcher * this);

#endif //DISPATCHER_CLIENT_H
