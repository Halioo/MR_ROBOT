//
// Created by gwendal on 26/05/2020.
//

#include "remoteui.h"
#include "postmanCommando.h"
#include "robocom.h"
#include "liste_chainee.h"

/* ----------------------- FUNCTIONS ----------------------- */

extern void RemoteUI_setEvents(Liste * myEvents){
    RQ_data msgToSend ={
            .rq_type = RQ_SET
    };

    while(myEvents->premier->indice > INDICE_INITIAL){
        msgToSend.logEvent = myEvents->premier->logEvent;
        sendNwk(PostmanCommando_getSocketComm(),msgToSend);
        myEvents->premier = myEvents->premier->suivant;
    }
    msgToSend.rq_type = RQ_END;
    sendNwk(PostmanCommando_getSocketComm(),msgToSend);
}

extern void RemoteUI_setEventsCount(int nbEvents){
    RQ_data msgToSend = {
            .rq_type = RQ_SET
    };
    msgToSend.eventsCount = nbEvents;
    sendNwk(PostmanCommando_getSocketComm(),msgToSend);
}


