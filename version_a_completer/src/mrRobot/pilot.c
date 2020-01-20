
#include <stdbool.h>
#include "pilot.h"
#include "robot.h"


typedef int Event;


/**
 * Start Pilot
 */
extern void Pilot_start() {
    Robot_start();
}

/**
 * Stop Pilot
 */
extern void Pilot_stop() {
    Robot_stop();
}

/**
 * initialize in memory the object Pilot
 */
extern void Pilot_new() {}

/**
 * destruct the object Pilot from memory
 */
extern void Pilot_free() {}

/**
 * setVelocity
 *
 * @brief description
 * @param vel
 */
extern void Pilot_setVelocity(VelocityVector vel) {}

/**
 * getState
 *
 * @brief description
 * @return PilotState
 */
extern PilotState Pilot_getState() {}

/**
 * check
 *
 * @brief description
 */
extern void Pilot_check() {}


static void run(Event event, VelocityVector vel) {}

static void send_mvt(VelocityVector vel) {}

static bool has_bumped() {
    return false;
}