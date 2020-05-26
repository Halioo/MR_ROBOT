//
// Created by cleme on 20/04/2020.
//

#ifndef ADMINUI_H
#define ADMINUI_H

#include <util.h>
#include "pilot.h"
#include "logger.h"

typedef struct AdminUI_t AdminUI;

extern AdminUI * AdminUI_new(Pilot * pilot, Logger * logger);
extern int AdminUI_start(AdminUI * this);
extern int AdminUI_stop(AdminUI * this);
extern int AdminUI_free(AdminUI * this);

extern void AdminUI_ToggleES(AdminUI * this);
extern void AdminUI_Quit(AdminUI * this);
extern void AdminUI_GoScreenLog(AdminUI * this);
extern void AdminUI_BackMainScreen(AdminUI * this);
extern void AdminUI_ClearLog(AdminUI * this);


#endif /* ADMINUI_H */
