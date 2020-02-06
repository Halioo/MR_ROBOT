
#include <stdbool.h>
#include <assert.h>

#include "pilot.h"
#include "robot.h"

#define DEFAULT_SPEED 0


typedef enum {
    S_FORGET=0,
    S_IDLE,
    S_RUNNING,
    PS_SET_VEL,
    PS_CHECK,
    S_DEATH,
    STATE_NB
} State;

typedef enum {
    E_SET_VEL=0,
    E_CHECK,
    E_VEL_IS_NULL,
    E_VEL_IS_NOT_NULL,
    E_COLLISION,
    E_NO_COLLISION,
    EVENT_NB
} Event;

typedef enum
{
    A_NOP=0,
    A_SET_VEL,
    A_EVAL_VEL,
    A_EVAL_CHECK,
    ACTION_NB
} TransitionAction;

typedef struct
{
    State next_state;
    TransitionAction action_to_perform;
} Transition;

static Transition transition_tab[STATE_NB][EVENT_NB] =
{
    [S_IDLE][E_SET_VEL] = {PS_SET_VEL, A_EVAL_VEL},
    [S_IDLE][E_CHECK] = {S_FORGET, A_NOP},
    [S_RUNNING][E_SET_VEL] = {PS_SET_VEL, A_EVAL_VEL},
    [S_RUNNING][E_CHECK] = {PS_CHECK, A_EVAL_CHECK},
    [PS_SET_VEL][E_VEL_IS_NULL] = {S_IDLE, A_SET_VEL},
    [PS_SET_VEL][E_VEL_IS_NOT_NULL] = {S_RUNNING, A_SET_VEL},
    [PS_CHECK][E_COLLISION] = {S_IDLE, A_SET_VEL},
    [PS_CHECK][E_NO_COLLISION] = {S_FORGET, A_NOP}
};

typedef void (*f_ptr)(VelocityVector vel);

static void send_mvt(VelocityVector vel);
static void eval_vel(VelocityVector vel);
static void eval_check(VelocityVector vel);

static void action_NOP(){}

static const f_ptr actions_tab[ACTION_NB] = {
        &action_NOP,
        &send_mvt,
        &eval_vel,
        &eval_check
};

static const VelocityVector DEFAULT_VELOCITY_VECTOR =
{
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
    assert(current_state != S_DEATH);
    Transition transition = transition_tab[current_state][event];
    if (transition.next_state != S_FORGET) {
        current_state = transition.next_state;
        actions_tab[transition.action_to_perform](vel);
    }
}


static void eval_vel(VelocityVector vel) {
    if (vel.power == 0) {
        run(E_VEL_IS_NULL, DEFAULT_VELOCITY_VECTOR);
    } else {
        run(E_VEL_IS_NOT_NULL, vel);
    }
}

static void eval_check(VelocityVector vel) {
    if (has_bumped()) {
        run(E_COLLISION, DEFAULT_VELOCITY_VECTOR);
    } else {
        run(E_NO_COLLISION, DEFAULT_VELOCITY_VECTOR);
    }
}

/**
 * Start Pilot
 */
extern void Pilot_start() {
    Pilot_new();
    Robot_start();
    switch_state(S_IDLE);
}

/**
 * Stop Pilot
 */
extern void Pilot_stop() {
    switch_state(S_IDLE);
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
    run(E_CHECK, DEFAULT_VELOCITY_VECTOR);
}

/**
 * setVelocity
 *
 * @brief description
 * @param vel
 */
extern void Pilot_setVelocity(VelocityVector vel) {
    run(E_SET_VEL, vel);
}