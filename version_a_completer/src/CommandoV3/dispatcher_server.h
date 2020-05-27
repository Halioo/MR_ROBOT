#ifndef DISPATCHER_SERVER_H
#define DISPATCHER_SERVER_H

#include <pthread.h>

#include "../../lib/include/logger.h"
#include "../../lib/include/pilot.h"
#include "../../lib/include/util.h"
#include "../../lib/include/robocom.h"
#include "../../lib/include/mailbox.h"



typedef struct Dispatcher_t Dispatcher;

Dispatcher * Dispatcher_New();

int Dispatcher_Start(Dispatcher * this);

int Dispatcher_Stop(Dispatcher * this);

int Dispatcher_Free(Dispatcher * this);

#endif //DISPATCHER_SERVER_H
