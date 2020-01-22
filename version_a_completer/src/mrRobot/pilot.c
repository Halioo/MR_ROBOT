
#include <stdbool.h>
#include "pilot.h"
#include "robot.h"


typedef enum {DEFAULT=0, SET_VEL, CHECK} Event;

typedef enum {INIT=0, IDLE, RUNNING} State;



static State current_state;


static void run(Event event, VelocityVector vel) {
    switch (current_state) {
        case INIT:
            
            current_state = IDLE;
            break;
        case IDLE:
            break;
        case RUNNING:
            break;
        default: break;
    }
}

static void send_mvt(VelocityVector vel) {
    int vel_r = 0, vel_l = 0;
    switch (vel.dir) {
        case FORWARD:
            vel_r = vel.power;
            vel_l = vel.power;
            break;
        case BACKWARD:
            vel_r = -vel.power;
            vel_l = -vel.power;
            break;
        case RIGHT:
            vel_r = -vel.power;
            vel_l = vel.power;
            break;
        case LEFT:
            vel_r = vel.power;
            vel_r = -vel.power;
            break;
        default: break;
    }
    Robot_setWheelsVelocity(vel_r, vel_l);
}

static bool has_bumped() {
    return Robot_getSensorState().collision;
}


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
extern void Pilot_setVelocity(VelocityVector vel) {
    run(SET_VEL, vel);
}

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