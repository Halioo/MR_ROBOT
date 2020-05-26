#ifndef PROXY_LOGGER_H
#define PROXY_LOGGER_H

#include "util.h"
#include <stdbool.h>

static bool boolES;

extern void askEvents(int from, int to);

extern void askEventsCount();

extern void clearEvents();

extern void signalES(bool s);

extern void Proxy_Pilot_store_Socket(int socket);

#endif //PROXY_LOGGER_H



