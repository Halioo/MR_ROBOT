

#ifndef MR_ROBOT_LISTE_CHAINEE_H
#define MR_ROBOT_LISTE_CHAINEE_H

#include "robocom.h"

#define INDICE_INITIAL (-1)

typedef struct Element Element;
struct Element{
    LogEvent logEvent;
    Element *suivant;
    int indice;
};

typedef struct {
    Element *premier;
} Liste;

extern Liste *ListeChainee_init();
void ListeChainee_ajout(Liste *liste, LogEvent logEvent);
void ListeChainee_supprDernierElem(Liste *liste);
void ListeChainee_reset(Liste * liste);


#endif //MR_ROBOT_LISTE_CHAINEE_H
