/**
 * @file  pilot.h
 *
 * @brief  description
 *
 * @author Jerome Delatour
 * @date 17-04-2016
 * @version version
 * @section License
 *
 * The MIT License
 *
 * Copyright (c) 2016, Jerome Delatour
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */

#ifndef PILOT_H
#define PILOT_H

#include "robocom.h"

typedef struct Pilot_t Pilot;

typedef struct
{
    int speed;
    int collision;
    float luminosity;
} PilotState;


/**
 * Start Pilot
 */
extern int Pilot_Start(Pilot * this);

/**
 * Stop Pilot
 */
extern int Pilot_Stop(Pilot * this);

/**
 * initialize in memory the object Pilot
 */
extern Pilot * Pilot_new();

/**
 * destruct the object Pilot from memory 
 */
extern int Pilot_Free(Pilot * this);

/**
 * setVelocity
 *
 * @brief description
 * @param vel
 */
extern void Pilot_SetVelocity(VelocityVector vel);

/**
 * getState
 * 
 * @brief description 
 * @return PilotState
 */
extern PilotState Pilot_getState();

/**
 * check
 * 
 * @brief description 
 */
extern void Pilot_check();

extern void Pilot_ToggleES(Pilot * this);

extern void Pilot_setRobotVelocity(Pilot * this, VelocityVector vel);

extern void Pilot_EventStop(Pilot * this);

#endif /* PILOT_H */

