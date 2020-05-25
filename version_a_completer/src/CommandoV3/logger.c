//
// Created by gwendal on 25/05/2020.
//
#include "util.h"
#include "logger.h"
#include "robot.h"



static int loggerCounter = 0;


/*----------------------- STATIC FUNCTIONS PROTOTYPES -----------------------*/

static void Logger_polling(Logger *this);

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

static void Action_NOP(Logger * this);

static void Action_POLL(Logger * this);


/*--------------------Prototypes des fonctions event-------------------------*/


static void Logger_EventStartPolling(Logger * this);

static void Logger_EventStopPolling(Logger * this);

static void Logger_EventAfter250ms(Logger * this);



/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @brief Fonction run de la classe Logger
 */
static void Logger_Run(Logger * this);

typedef void (*f_ptr)();

static const f_ptr actions_tab[ACTION_NB] = {
        &Action_NOP,
        &Action_POLL
};

/*----------------------- EVENT FUNCTIONS -----------------------*/

extern void Logger_EventStartPolling(Logger * this) {
    Msg msg = {
            .event = E_START_POLLING,
    };

    Wrapper wrapper = {
            .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void Logger_EventStopPolling(Logger * this) {
    Msg msg = {
            .event = E_STOP_POLLING,
    };

    Wrapper wrapper = {
            .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void Logger_EventAfter250ms(Logger * this) {
    Msg msg = {
            .event = E_AFTER_250MS,
    };

    Wrapper wrapper = {
            .data = msg
    };

    mailboxSendMsg(this->mailbox,wrapper.toString);
}

/* ----------------------- RUN FUNCTION ----------------------- */

static void Logger_Run(Logger * this) {
    TransitionAction action = A_NOP;
    State state = S_IDLE;
    Wrapper wrapper;

    while (state != S_DEATH) {
        mailboxReceive(this->mailbox,wrapper.toString); ///< On reçoit un message de la mailbox

        if(wrapper.data.event == E_KILL){
            this->myState = S_DEATH;
        }
        else{
            action = transition_tab[state][wrapper.data.event].action_to_perform;
            TRACE("Action %s\n", TransitionAction_toString[action])

            state = transition_tab[state][wrapper.data.event].next_state;
            TRACE("State %s\n", State_toString[state])

            if(state != S_FORGET){
                this->message = wrapper.data;
                actions_tab[action](this);
                this->myState = state;
            }
        }
    }
}

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

static void Action_NOP(Logger * this){
    //ne rien faire
}

static void Action_POLL(Logger * this) {
    /*Watchdog *watchdog;
    WatchdogCallback callback={watchdog, &Logger_polling};
    watchdog=WatchdogConstruct(250,*callback, this);
    WatchdogStart(watchdog);
    WatchdogCancel(watchdog);
    WatchdogDestroy(watchdog);
     */
    //TODO je comprends pas le watchdog donc à refaire
}

/* ----------------------- NEW START STOP FREE -----------------------*/

extern Logger *  Logger_new() {
    loggerCounter++;
    Logger * this = (Logger *) malloc(sizeof(Logger));
    this->mailbox = mailboxInit("Pilot",loggerCounter,sizeof(Msg));
    this->eventList=initialisation();

    return this;
}

extern void Logger_start(Logger * this) {
    pthread_create(&(this->threadId),NULL,(void *)Logger_Run, this);

}

extern void Logger_stop(Logger * this) {
    pthread_join(this->threadId,NULL);

}

extern void Logger_free(Logger * this) {
    mailboxClose(this->mailbox);
    free(this);
}

/* ----------------------- STATIC FUNCTION DEFINITION -----------------------*/


static void Logger_polling(Logger *this){
    this->sens=Logger_getSensorState(this);
    this->speed=Logger_getRobotSpeed(this);
    LogEvent logEvent={this->sens,this->speed};
    insertion(this->eventList,logEvent);
    Logger_Run(this);
}

/* ----------------------- FUNCTION DEFINITION -----------------------*/

extern SensorState Logger_getSensorState(Logger * this){
    SensorState sens=Robot_getSensorState();
    return sens;
}

extern int Logger_getRobotSpeed(Logger * this){
    int speed=Robot_getRobotSpeed();
    return speed;
}

extern void Logger_signalES(Logger *this){
    this->myState=S_DEATH;
    Logger_Run(this);
}

extern LogEvent* Logger_getEvents(int from, int to,LogEvent *logEventToReturn,Logger * this){
    int count=0;
    //LogEvent logEventToReturn[(to-from)+1];

    Element *actuel = this->eventList->premier;

    while (actuel != NULL)
    {
        if(count>=from && count<=to){
            logEventToReturn[count]=actuel->logEvent;
            count++;

        }
        actuel = actuel->suivant;
    }
    return logEventToReturn;
}

extern int Logger_getEventsCount(Logger * this){
    int count=0;

    Element *actuel = this->eventList->premier;

    while (actuel != NULL)
    {
        count++;
        actuel = actuel->suivant;
    }
    return count;
}


extern void Logger_clearEvents(Logger * this){
    suppression(this->eventList);
}




