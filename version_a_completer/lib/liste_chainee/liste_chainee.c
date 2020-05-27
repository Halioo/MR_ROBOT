
#include <stdio.h>
#include <stdlib.h>
#include "liste_chainee.h"

static int indiceActuel = INDICE_INITIAL;

Liste * ListeChainee_init()
{
    Liste *liste = malloc(sizeof(*liste));
    Element *element = malloc(sizeof(*element));

    if (liste == NULL || element == NULL){
        exit(EXIT_FAILURE);
    }

    element->logEvent.speed = 0;
    element->logEvent.sens.collision_f = 0;
    element->logEvent.sens.luminosity = 0;
    element->indice = INDICE_INITIAL;


    liste->premier = element;

    return liste;
}


void ListeChainee_ajout(Liste *liste, LogEvent logEvent)
{
    ++indiceActuel;
    /* Création du nouvel élément */
    Element *nouveau = malloc(sizeof(*nouveau));
    if (liste == NULL || nouveau == NULL)
    {
        exit(EXIT_FAILURE);
    }
    nouveau->logEvent = logEvent;
    nouveau->indice = indiceActuel;

    /* Insertion de l'élément au début de la liste */
    nouveau->suivant = liste->premier;
    liste->premier = nouveau;
}


void ListeChainee_supprDernierElem(Liste *liste)
{
    if (liste == NULL){
        exit(EXIT_FAILURE);
    }

    if (liste->premier != NULL){
        Element *aSupprimer = liste->premier;
        liste->premier = liste->premier->suivant;
        free(aSupprimer);
    }
}


void ListeChainee_reset(Liste * liste){
    Element *actuel = liste->premier;

    while (actuel->indice > INDICE_INITIAL)
    {
        actuel = liste->premier;
        ListeChainee_supprDernierElem(liste);
        actuel = actuel->suivant;
    }
}









