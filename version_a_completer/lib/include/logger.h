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


extern void Logger_askEvents(int from,int to, Logger * this);
extern void Logger_askEventsCount(Liste * liste);


extern Liste * Logger_getEvents(int from, int to, Logger * this);

extern int Logger_getEventsCount(Liste * liste);


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
