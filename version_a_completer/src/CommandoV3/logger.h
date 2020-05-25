//
// Created by gwendal on 25/05/2020.
//

#ifndef MR_ROBOT_LOGGER_H
#define MR_ROBOT_LOGGER_H


#include <robocom.h>


typedef struct logEvent_t logEvent;


/**
 * get the robot sensorState
 */
extern SensorState Logger_getSensorState();

/**
 * get the robot speed
 */
extern int Logger_getRobotSpeed();

/**
 * to be notified when an emergency stop is needed
 */
extern void Logger_signalES();

/**
 * ??????????????????
 */
extern void Logger_askEvents(int from,int to);

/**
 * ??????????????????
 */
extern void Logger_askEventsCount();

/**
 * ??????????????????
 */
extern void Logger_setEvents(logEvent *events);

/**
 * ??????????????????
 */
extern void Logger_setEventsCount(int indice);

/**
 * start polling
 */
extern void Logger_startPolling();

/**
 * stop polling
 */
extern void Logger_stopPolling();

/**
 * clear events
 */
extern void Logger_clearEvents();

/**
 * return the number of events
 */
extern int Logger_getEventsCount();

/**
 * get a a tab of event from indice "from" to "to"
 */
extern logEvent* Logger_getEvents(int from,int to);

/**
 * create the logger
 */
extern void Logger_new();

/**
 * start the logger
 */
extern void Logger_start();

/**
 * stop the logger
 */
extern void Logger_stop();

/**
 * free the logger
 */
extern void Logger_free();

#endif //MR_ROBOT_LOGGER_H
