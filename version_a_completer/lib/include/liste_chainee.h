

#ifndef MR_ROBOT_LISTE_CHAINEE_H
#define MR_ROBOT_LISTE_CHAINEE_H

#include "robocom.h"



typedef struct Element Element;
struct Element{
    LogEvent logEvent;
    Element *suivant;
    int indice;
};

typedef struct Liste Liste;
struct Liste{
    Element *premier;
};

Liste *initialisation();
void insertion(Liste *liste, LogEvent logEvent);
void suppression(Liste *liste);

#endif //MR_ROBOT_LISTE_CHAINEE_H
