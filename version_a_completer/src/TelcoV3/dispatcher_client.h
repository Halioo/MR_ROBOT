#ifndef DISPATCHER_CLIENT_H
#define DISPATCHER_CLIENT_H

#include <pthread.h>

#include "util.h"
#include "mailbox.h"
#include "../../lib/include/mailbox.h"

typedef struct Dispatcher_t Dispatcher;

typedef char Buffer [10]; 

extern Dispatcher* Dispatcher_new();

extern void Dispatcher_free(Dispatcher* this);

#endif //DISPATCHER_CLIENT_H
