/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include <stdlib.h>

#include "../../../infox_prose-x86_64-v0.3/include/infox/prose/prose.h"
#include "prose.h"
#include "robot.h"

#define LEFT_MOTOR MD
#define RIGHT_MOTOR MA
#define LIGHT_SENSOR S1
#define FRONT_BUMPER S3
#define FLOOR_SENSOR S2

const int ROBOT_CMD_STOP = 0;

typedef struct Robot_t {
    Motor * motor_r;
    Motor * motor_l;
    LightSensor * sensor_light;
    ContactSensor * sensor_bump;
    //ContactSensor* sensor_floor;
} Robot;

static Robot * robot;

/**
 * Start the Robot (initialize communication and open port)
 */
extern void Robot_start() {
    // Creation of a new robot in memory
    Robot_new();
    // Connect to Intox simulator
    if (ProSE_Intox_init("127.0.0.1", 12345) == -1) {
        PProseError("Problème d'initialisation du simulateur Intox");
    }
    // Open ports
    robot->motor_r = Motor_open(RIGHT_MOTOR);
    robot->motor_l = Motor_open(LEFT_MOTOR);
    robot->sensor_light = LightSensor_open(LIGHT_SENSOR);
    robot->sensor_bump = ContactSensor_open(FRONT_BUMPER);
    //robot->sensor_floor = ContactSensor_open(FLOOR_SENSOR);
}

/**
 * Stop Robot (stop communication and close port)
 */
extern void Robot_stop() {
    // Stop the motors
    Robot_setWheelsVelocity(ROBOT_CMD_STOP, ROBOT_CMD_STOP);
    // Close ports
    Motor_close(robot->motor_r);
    Motor_close(robot->motor_l);
    LightSensor_close(robot->sensor_light);
    ContactSensor_close(robot->sensor_bump);
    //ContactSensor_close(robot->sensor_floor);
    // Erase Robot from memory
    Robot_free();
}

/**
 * @briel initialize in memory the object Robot
 */
extern void Robot_new() {
    robot = (Robot *) malloc(sizeof(Robot));
}

/**
 * @brief destruct the object Robot from memory
 */
extern void Robot_free() {
    free(robot);
}

/**
 * Robot_getRobotSpeed
 *
 * @brief return the speed of the robot (positive average of the right's and left's current wheel power)
 * @return speed of the robot (beetween 0 and 100)
 */
extern int Robot_getRobotSpeed() {
    Cmd cmd_r = Motor_getCmd(robot->motor_r);
    Cmd cmd_l = Motor_getCmd(robot->motor_l);

    return abs((cmd_r + cmd_l)/2);
}

/**
 * Robot_getSensorState
 *
 * @brief return the captor's states of the bumper and the luminosity
 * @return SensorState
 */
extern SensorState Robot_getSensorState() {
    SensorState st;
    st.collision = ContactSensor_getStatus(robot->sensor_bump);
    st.luminosity = LightSensor_getStatus(robot->sensor_light);
    return st;
}

/**
 * Robot_setWheelsVelocity
 *
 * @brief set the power on the wheels of the robot
 * @param int mr : right's wheel power, value between -100 and 100
 * @param int ml : left's wheel power, value between -100 and 100
 */
extern void Robot_setWheelsVelocity(int mr, int ml) {
    if(Motor_setCmd(robot->motor_r, mr) == -1) {
        PProseError("Problème de commande du moteur droit");
    }
    if(Motor_setCmd(robot->motor_l, ml) == -1) {
        PProseError("Problème de commande du moteur gauche");
    }
}
