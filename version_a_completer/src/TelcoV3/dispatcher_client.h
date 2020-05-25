#ifndef DISPATCHER_CLIENT_H
#define DISPATCHER_CLIENT_H

#include "util.h"

typedef struct Dispatcher_t Dispatcher;

extern Dispatcher* Dispatcher_new();

extern void Dispatcher_free(Dispatcher* this);

#endif //DISPATCHER_CLIENT_H
