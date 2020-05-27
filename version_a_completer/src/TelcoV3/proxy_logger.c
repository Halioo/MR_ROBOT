
#include "../../lib/include/robocom.h"
#include "../../lib/include/logger.h"
#include "postmanTelco.h"


/* ----------------------- FUNCTIONS ----------------------- */

extern void Logger_askEvents(int from,int to, Logger * this){
    RQ_data msgToSend = {
            .from = from,
            .to = to,
            .rq_type = RQ_ASK_EVENTS
    };
    sendNwk(PostmanTelco_getSocketComm(),msgToSend);
}

extern void Logger_askEventsCount(Logger * this){
    RQ_data msgToSend = {
            .rq_type = RQ_ASK_EVENTS_NB
    };
    sendNwk(PostmanTelco_getSocketComm(),msgToSend);
}

