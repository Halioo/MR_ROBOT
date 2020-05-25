

#ifndef MR_ROBOT_LISTE_CHAINEE_H
#define MR_ROBOT_LISTE_CHAINEE_H

#include "robocom.h"

typedef struct LogEvent_t{
    SensorState sens;
    int speed;
}LogEvent;

typedef struct Element Element;
struct Element
{
    struct LogEvent_t logEvent;
    Element *suivant;
};

typedef struct Liste Liste;
struct Liste
{
    Element *premier;
};

Liste *initialisation();
void insertion(Liste *liste, struct LogEvent_t logEvent);
void suppression(Liste *liste);

#endif //MR_ROBOT_LISTE_CHAINEE_H
