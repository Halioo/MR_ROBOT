/**
 * @file  adminUI.h
 *
 * @brief  This component describe the admin user interface. It captures the user's controls and send them to the pilot module.
 *
 * @author Jerome Delatour
 * @date 17-04-2016
 * @version 1
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


#ifndef REMOTEUI_H
#define REMOTEUI_H

#include "../../src/TelcoV3/ihm.h"
#include "mailbox.h"
#include "robocom.h"
#include "liste_chainee.h"


typedef struct RemoteUI_t RemoteUI;


extern void RemoteUI_setIp(RemoteUI*, char * ip);
extern void RemoteUI_validate(RemoteUI*);
extern void RemoteUI_connectSuccess(RemoteUI*);
extern void RemoteUI_connectFailure(RemoteUI*);

extern void RemoteUI_setDir(RemoteUI*, DIRECTION dir);
extern void RemoteUI_toggleEmergencyStop(RemoteUI*);

extern void RemoteUI_goScreenLog(RemoteUI*);
extern void RemoteUI_backMainScreen(RemoteUI*);

// TODO ajouter le getEvent/setEvent (transition intern de logScreen)

extern void RemoteUI_quit(RemoteUI*);


/**
 * @brief Example function that treats a wathdog event.
 */
extern void ExampleTimeout(Watchdog * wd, void * caller);


/**
 * initialize in memory RemoteUI
 */
extern RemoteUI * RemoteUI_new();

/**
 * Start RemoteUI
 */
extern int RemoteUI_start(RemoteUI*);

/**
 * Stop RemoteUI
 */
extern int RemoteUI_stop(RemoteUI*);

/**
 * destruct the RemoteUI from memory
 */
extern int RemoteUI_free(RemoteUI*);

/* ------------------------ OTHER FUNCTIONS ---------------------*/

extern void RemoteUI_setEvents(Liste * myEvents);

extern void RemoteUI_setEventsCount(int nbEvents);

#endif /* REMOTEUI_H */
