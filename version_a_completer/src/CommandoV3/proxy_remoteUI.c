//
// Created by gwendal on 26/05/2020.
//

#include "../../lib/include/remoteui.h"
#include "postmanCommando.h"
#include "../../lib/include/robocom.h"
#include "../../lib/include/liste_chainee.h"

/* ----------------------- FUNCTIONS ----------------------- */

extern void RemoteUI_setEvents(Liste * myEvents){
    RQ_data msgToSend ={
            .rq_type = RQ_SET_EVENTS
    };

    while(myEvents->premier->indice > INDICE_INITIAL){
        msgToSend.logEvent = myEvents->premier->logEvent;
        sendNwk(PostmanCommando_getSocketComm(),msgToSend);
        myEvents->premier = myEvents->premier->suivant;
    }
    msgToSend.rq_type = RQ_END_SET_EVENT;
    sendNwk(PostmanCommando_getSocketComm(),msgToSend);
}

extern void RemoteUI_setEventsCount(int nbEvents){
    RQ_data msgToSend = {
            .rq_type = RQ_SET_EVENT_NB
    };
    msgToSend.eventsCount = nbEvents;
    sendNwk(PostmanCommando_getSocketComm(),msgToSend);
}


