
#include <stdbool.h>

#include "pilot.h"
#include "robot.h"

#define DEFAULT_SPEED 0


typedef enum {DEFAULT_EVENT=0, SET_VEL, CHECK} Event;
typedef enum {INIT=0, IDLE, RUNNING} State;

static const VelocityVector DEFAULT_VELOCITY_VECTOR = {
    .dir = FORWARD,
    .power = DEFAULT_SPEED
};

static State current_state;


/**
 * Retourne un booléen donnant l'état du capteur de collision avant
 */
static bool has_bumped() {
    return Robot_getSensorState().collision;
}

/**
 * Traduit le VelocityVector en deux valeurs
 * correspondant aux états des moteurs droit et gauche
 */
static void send_mvt(VelocityVector vel) {
    int vel_r = DEFAULT_SPEED, vel_l = DEFAULT_SPEED;
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
            vel_l = -vel.power;
            break;
        default: break;
    }
    Robot_setWheelsVelocity(vel_r, vel_l);
}

/**
 * Met à jour l'état actuel du pilote et effectue
 * les actions en entrée de cet état
 */
static void switch_state(State new_state) {
    switch (new_state) {
        case INIT:
            current_state = INIT;
            break;
        case IDLE:
            send_mvt(DEFAULT_VELOCITY_VECTOR);
            current_state = IDLE;
            break;
        case RUNNING:
            current_state = RUNNING;
            break;
        default: break;
    }
}

/**
 * Fonction run permettant la mise à jour de l'état du pilote
 * Machine à état asynchrone
 */
static void run(Event event, VelocityVector vel) {
    switch (current_state) {
        // INIT : passe à l'état IDLE directement
        case INIT:
            switch_state(IDLE);
            break;
        // IDLE : ne fait quelque chose que si l'event est un SET_VEL
        case IDLE:
            switch (event) {
                case SET_VEL:
                    if (vel.power == 0) {
                        switch_state(IDLE);
                    } else {
                        send_mvt(vel);
                        switch_state(RUNNING);
                    }
                    break;
                case CHECK: default: break;
            }
            break;
        // RUNNING : revient à IDLE si vitesse à 0
        // Ne check pas s'il y a eu un bump !
        case RUNNING:
            switch (event) {
                case SET_VEL:
                    if (vel.power == 0) {
                        switch_state(IDLE);
                    } else {
                        send_mvt(vel);
                    }
                    break;
                case CHECK:
                    if (has_bumped()) {
                        switch_state(IDLE);
                    }
                    break;
                default: break;
            }
            break;
        default: break;
    }
}


/**
 * Start Pilot
 */
extern void Pilot_start() {
    Pilot_new();
    Robot_start();
    run(DEFAULT_EVENT, DEFAULT_VELOCITY_VECTOR);
}

/**
 * Stop Pilot
 */
extern void Pilot_stop() {
    switch_state(IDLE);
    Robot_stop();
    Pilot_free();
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
 * getState
 *
 * @brief description
 * @return PilotState
 */
extern PilotState Pilot_getState() {
    PilotState pt;

    pt.speed = Robot_getRobotSpeed();

    SensorState st = Robot_getSensorState();
    pt.collision = st.collision;
    pt.luminosity = st.luminosity;

    return pt;
}

/**
 * check
 *
 * @brief description
 */
extern void Pilot_check() {
    run(CHECK, DEFAULT_VELOCITY_VECTOR);
}

/**
 * setVelocity
 *
 * @brief description
 * @param vel
 */
extern void Pilot_setVelocity(VelocityVector vel) {
    run(SET_VEL, vel);
}