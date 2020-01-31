
#include <stdbool.h>

#include "pilot.h"
#include "robot.h"

#define DEFAULT_SPEED 0


typedef enum {SET_VEL=0, CHECK=1} Event;
typedef enum {IDLE=0, RUNNING=1} State;

/**
typedef void (*f_ptr_generic)(void);
typedef void (*f_ptr_vel)(VelocityVector vel);
typedef void (*f_ptr_ev_vel)(Event event, VelocityVector vel);

typedef struct {
    f_ptr_ev_vel state_f;
    f_ptr_vel set_vel_f;
    f_ptr_generic check_vel;
} State_s;

static void init_f();
static void idle_f(Event event, VelocityVector vel);
static void idle_vel_f(VelocityVector vel);
static void running_f(Event event, VelocityVector vel);
static void running_vel_f(VelocityVector vel);
static void running_check_f();

static const State_s INIT_s = {
    .state_f = &init_f
};
static const State_s IDLE_s = {
    .state_f = idle_f,
    .set_vel_f = &idle_vel_f
};
static const State_s RUNNING_s = {
    .state_f = running_f,
    .set_vel_f = running_vel_f,
    .check_vel = running_check_f
};

static State_s etats[] = {INIT_s, IDLE_s, RUNNING_s};
*/

static void idle_f_vel(VelocityVector vel);
static void idle_f_check(VelocityVector vel);
static void running_f_vel(VelocityVector vel);
static void running_f_check(VelocityVector vel);

typedef void (*f_ptr)(VelocityVector vel);

static f_ptr state_f_list[] =
    {
        &idle_f_vel,       // 0 = 00 = IDLE | SET_VEL
        &idle_f_check,     // 1 = 01 = IDLE | CHECK
        &running_f_vel,    // 2 = 10 = RUNNING | SET_VEL
        &running_f_check   // 3 = 11 = RUNNING | CHECK
    };

static const VelocityVector DEFAULT_VELOCITY_VECTOR = {
    .dir = STOP,
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
 * Fonction run permettant la mise à jour de l'état du pilote
 * Machine à état asynchrone
 */
static void run(Event event, VelocityVector vel) {
    state_f_list[current_state<<1|event](vel);
}

/**
 * Met à jour l'état actuel du pilote et effectue
 * les actions en entrée de cet état
 */
static void switch_state(State new_state) {
    switch (new_state) {
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
 * Fonctions de l'état IDLE
 */
static void idle_f_vel(VelocityVector vel) {
    if (vel.power != 0) {
        send_mvt(vel);
        switch_state(RUNNING);
    }
}
static void idle_f_check(VelocityVector vel) {}

/**
 * Fonctions de l'état RUNNING
 */
static void running_f_vel(VelocityVector vel) {
    if (vel.power == 0) {
        switch_state(IDLE);
    } else {
        send_mvt(vel);
    }
}
static void running_f_check(VelocityVector vel) {
    if (has_bumped()) {
        switch_state(IDLE);
    }
}


/**
 * Start Pilot
 */
extern void Pilot_start() {
    Pilot_new();
    Robot_start();
    switch_state(IDLE);
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