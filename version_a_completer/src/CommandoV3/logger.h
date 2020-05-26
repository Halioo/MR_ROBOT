//
// Created by gwendal on 25/05/2020.
//

#ifndef MR_ROBOT_LOGGER_H
#define MR_ROBOT_LOGGER_H


#include <robocom.h>
#include <mailbox.h>
#include <liste_chainee.h>


typedef struct Logger_t Logger;

/**
 * to be notified when an emergency stop is needed
 */
extern void Logger_signalES(Logger * this);

/**
 * ??????????????????
 */
extern void Logger_askEvents(int from,int to,Logger * this);

/**
 * ??????????????????
 */
extern void Logger_askEventsCount(Logger * this);

/**
 * ??????????????????
 */
extern void Logger_setEvents(LogEvent *events, Logger * this);

/**
 * ??????????????????
 */
extern void Logger_setEventsCount(int indice,Logger * this);

/* ----------------------- EVENT FUNCTIONS ----------------------- */

/**
 * start polling
 */
extern void Logger_startPolling(Logger * this);

/**
 * stop polling
 */
extern void Logger_stopPolling(Logger * this);


/**
 * clear events
 */
extern void Logger_clearEvents(Logger * this);

/**
 * return the number of events
 */
extern int Logger_getEventsCount(Logger * this);

/**
 * get a tab of event from indice "from" to "to"
 * you have to pass an array of LogEvent in parameters to put the LogEvents collected in it. The size should be <(to-from)+1>.
 * I had to do this because I cannot return a localy created array
 */
extern void Logger_getEvents(int from, int to,LogEvent *logEventToReturn,Logger * this);

/**
 * create the logger
 */
extern Logger* Logger_new();

/**
 * start the logger
 */
extern void Logger_start(Logger * this);

/**
 * stop the logger
 */
extern void Logger_stop(Logger * this);

/**
 * free the logger
 */
extern void Logger_free(Logger * this);


// TODO : remove me after tests
extern void Logger_test(Logger * this);


#endif //MR_ROBOT_LOGGER_H
