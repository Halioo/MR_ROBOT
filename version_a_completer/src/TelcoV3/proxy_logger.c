

#include "proxy_logger.h"
#include "robocom.h"


/*----------------------- STATIC VARIABLES -----------------------*/
/**
 * @var int myCurrentSocket
 * @brief Current socket to which the next message will be sent
 */

LogEvent * myEvents; // TODO : définir la taille max de mstatic
int myCurrentSocket;


/* ----------------------- FUNCTIONS ----------------------- */

extern void ProxyLogger_new(int socket){
    myCurrentSocket = socket;
}

extern void askEvents(int from, int to) {
    RQ_data data={
            .arg1=from,
            .arg2=to,
            .command=C_EVENTS
            //.rq_type=RQ_GET
    };
    sendNwk(myCurrentSocket,data);
    TRACE("to : Logger // msg: askEvents \n")
}

extern void askEventsCount() {
    RQ_data data={
            .command=C_EVENTSCOUNT
            //.rq_type=RQ_GET
    };
    sendNwk(myCurrentSocket,data);
    TRACE("to : Logger // msg: askEventsCount")
}

extern void clearEvents(){
    RQ_data data={
            .command=C_CLEAR
            //.rq_type=RQ_DELETE
    };
    sendNwk(myCurrentSocket,data);
}

extern void signalES(bool s){
    boolES=s;
    RQ_data data;
    if (s==true){
        data.command=C_ES;
        //data.rq_type=RQ_ES;
    }
    else{
        data.command=C_NOP;
        //data.rq_type=RQ_ES;
    }

    sendNwk(myCurrentSocket,data);
}
