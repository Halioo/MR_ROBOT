//
// Created by gwendal on 25/05/2020.
//
#include "util.h"
#include "logger.h"
#include "robot.h"
#include "remoteui.h"

/**
 * @def Name of the task. Each instance will have this name,
 * followed by the number of the instance
 */
#define NAME_TASK "LoggerTask%d"
#define POLLING_REFRESH_RATE (250000)

static int loggerCounter = 0;
static pthread_mutex_t mutexLockEventsCount;


/**STATE
 * Liste des différents états possibles du logger
 * FORGET:aucune action n'est effectuée
 * IDLE: en attente du démarage du logger
 * POLLING: recolte les LogEvent
 */
ENUM_DECL(STATE,
          S_FORGET=0,
          S_IDLE,
          S_POLLING,
          S_DEATH        ///< Etat de mort du robot, utilisé lorsqu'on veut l'arrêter
)

/**
 * Liste des event possibles
 */

ENUM_DECL(EVENT,
          E_START_POLLING,      // Evenement pour commencer à scruter le robot
          E_STOP_POLLING,       // Evenement pour arrêter de scruter le robot
          E_TO_POLL,            // Evenement appelé à expiration du timer de 250ms
          E_KILL                ///< Détruit la MaE
)

/**
 * Liste des différentes actions possibles
 */

ENUM_DECL(ACTION,
          A_NOP,                ///< Ne rien faire
          A_START_POLLING,      ///< On lance le timer
          A_STOP_POLLING,       ///< On reset le timer - sert d'action pour quitter
          A_POLL               ///< On relève l'état du robot
)

/**
 * @brief Structure de transition de la MaE
 */
typedef struct {
    STATE nextState; ///< Prochain State de la MaE
    ACTION action;   ///< Action réalisée avant d'aller dans le prochain State
} Transition;


/**
 * @brief Structure d'un message ajouté à la BAL
 */
typedef struct {
    EVENT event;
    LogEvent logEvent; ///< Paramètre event du message
} Msg;

/**
 * @brief Wrapper enum. Utilisé pour envoyer des Msg dans une BAL
 */
wrapperOf(Msg)

/**
 * @brief Structure de l'objet Logger
 */
struct Logger_t {
    pthread_t threadId; ///< Pthread identifier for the active function of the class.
    Mailbox * mailbox;
    Watchdog * watchdogPoll;
    Msg message; ///< Structure utilisée pour passer les donnés de la BAL aux pointeurs de fonction
    STATE myState; ///< Etat actuel de la MaE
    Liste * myEvents;
    int indiceEvents;
    pthread_mutex_t mutexLockEvents;

};

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

static void ActionNop(Logger * this);

static void ActionStartPolling(Logger * this);
static void ActionStopPolling(Logger * this);

static void ActionPoll(Logger * this);


/* ----------------------- EVENT FUNCTIONS ----------------------- */

static void Logger_EventTOPoll(Logger * this);


/*----------------------- OTHER FUNCTIONS PROTOTYPES -----------------------*/

/**
 * @brief initialise la liste chainée d'events
 */
static Liste * Logger_initEventList();

/**
 * @brief ajoute un event à la liste d'events
 */
static void Logger_appendEvent(LogEvent logEvent, Logger * this);

/**
 * @brief supprime la liste d'events en mémoire
 */
static void Logger_resetEventList(Liste * liste);

/**
 * @brief supprime le dernier event ajoute
 */
static void Logger_removeEvent(Liste * liste);

/**quit
 * @brief Fonction run de la classe Logger
 */
static void Logger_Run(Logger * this);

static void Logger_setTO(Logger * this);

static void Logger_resetTO(Logger * this);

static void Logger_TOHandle(void * this);

/*----------------------- STATE MACHINE DECLARATION -----------------------*/

/**
 * @def Pointeur de fonction pour les fonction d'Action de la MaE
 */
typedef void (*ActionPtr)(Logger *);

/**
 * @brief Tableau de pointeur de fonctions pour stocker les fonctions correspondantes aux actions dans le même ordre qu'elles ont été définies
 */
static const ActionPtr actionPtrTab[NB_ACTION]={
        &ActionNop,
        &ActionPoll,
        &ActionStartPolling,
        &ActionStopPolling,
};

/**
 * Tableau des transitions
 * [etat_courant][type_event]
 */
static Transition stateMachine[NB_STATE][NB_EVENT] ={
        [S_IDLE][E_START_POLLING] = {S_POLLING, A_START_POLLING},
        [S_POLLING][E_STOP_POLLING] = {S_IDLE, A_STOP_POLLING},
        [S_POLLING][E_TO_POLL] = {S_POLLING, A_POLL},
        [S_IDLE][E_KILL] = {S_DEATH, A_NOP},
        [S_POLLING][E_KILL] = {S_DEATH, A_STOP_POLLING}
};


/*----------------------- EVENT FUNCTIONS -----------------------*/

extern void Logger_startPolling(Logger * this) {

    Wrapper wrapper = {
            .data.event = E_START_POLLING
    };
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

extern void Logger_stopPolling(Logger * this) {

    Wrapper wrapper = {
            .data.event = E_STOP_POLLING
    };
    mailboxSendMsg(this->mailbox,wrapper.toString);
}


extern void Logger_EventTOPoll(Logger * this) {

    Wrapper wrapper = {
            .data.event = E_TO_POLL
    };
    mailboxSendMsg(this->mailbox,wrapper.toString);
}

/* ----------------------- RUN FUNCTION ----------------------- */

static void Logger_Run(Logger * this) {
    ACTION action;
    STATE state = S_IDLE;
    Wrapper wrapper;

    while (this->myState != S_DEATH) {
        mailboxReceive(this->mailbox,wrapper.toString); ///< On reçoit un message de la mailbox

        if(wrapper.data.event == E_KILL){
            this->myState = S_DEATH;
        }
        else{
            action = stateMachine[state][wrapper.data.event].action;
            TRACE("Action %s\n", ACTION_toString[action])

            state = stateMachine[state][wrapper.data.event].nextState;
            TRACE("State %s\n", STATE_toString[state])

            if(state != S_FORGET){
                this->message = wrapper.data;
                actionPtrTab[action](this);
                this->myState = state;
            }
        }
    }
}

/* ----------------------- ACTIONS FUNCTIONS ----------------------- */

static void ActionNop(Logger * this){
    //ne rien faire
}

static void ActionPoll(Logger * this) {

    SensorState sens = Robot_getSensorState();
    int speed = Robot_getRobotSpeed();
    LogEvent logEvent = {.speed = speed,
                         .sens = sens};
    Logger_appendEvent(logEvent,this);
}

static void ActionStartPolling(Logger * this){
    Logger_setTO(this);
}

static void ActionStopPolling(Logger * this){
    Logger_resetTO(this);
}


/* ----------------------- NEW START STOP FREE -----------------------*/

extern Logger * Logger_new() {
    loggerCounter++;
    Logger * this = (Logger *) malloc(sizeof(Logger));
    this->mailbox = mailboxInit("Logger",loggerCounter,sizeof(Msg));
    this->watchdogPoll = WatchdogConstruct(POLLING_REFRESH_RATE,&Logger_TOHandle,this);
    this->myEvents = Logger_initEventList();
    this->indiceEvents = INDICE_INITIAL;
    this->myState = S_IDLE;

    return this;
}

extern void Logger_start(Logger * this) {
    pthread_create(&(this->threadId),NULL,(void *)Logger_Run, this);

}

extern void Logger_stop(Logger * this) {
    Wrapper wrapper;
    wrapper.data.event = E_KILL;
    mailboxSendMsg(this->mailbox,wrapper.toString);

    WatchdogCancel(this->watchdogPoll);
    pthread_join(this->threadId,NULL);
}

extern void Logger_free(Logger * this) {
    mailboxClose(this->mailbox);
    WatchdogDestroy(this->watchdogPoll);
    free(this);
}

/* ----------------------- GESTION DE LA LISTE CHAINEE -----------------------*/

static Liste * Logger_initEventList(){
    Liste * liste = malloc(sizeof(*liste));
    Element *element = malloc(sizeof(*element));

    if (liste == NULL || element == NULL){
        perror("Erreur d'initialisation de la liste d'events en mémoire");
        exit(EXIT_FAILURE);
    }

    element->logEvent.speed = 0;
    element->logEvent.sens.collision_f = 0;
    element->logEvent.sens.luminosity = 0;
    element->indice = INDICE_INITIAL;

    liste->premier = element;

    return liste;
}

static void Logger_appendEvent(LogEvent logEvent, Logger * this){
    ++this->indiceEvents;

    /* Création du nouvel élément */
    Element *nouveau = malloc(sizeof(*nouveau));
    if (this->myEvents == NULL || nouveau == NULL)
    {
        exit(EXIT_FAILURE);
    }
    nouveau->logEvent = logEvent;
    nouveau->indice = this->indiceEvents;

    /* Insertion de l'élément au début de la liste */
    nouveau->suivant = this->myEvents->premier;
    this->myEvents->premier = nouveau;

}

static void Logger_resetEventList(Liste * liste){

    Element *actuel = liste->premier;

    while (actuel->indice > INDICE_INITIAL)
    {
        actuel = liste->premier;
        Logger_removeEvent(liste);
        actuel = actuel->suivant;
    }
}

static void Logger_removeEvent(Liste * liste){
    if (liste == NULL){
        exit(EXIT_FAILURE);
    }

    if (liste->premier != NULL){
        Element *aSupprimer = liste->premier;
        liste->premier = liste->premier->suivant;
        free(aSupprimer);
    }
}


extern void Logger_signalES(Logger *this){

}

extern int Logger_getEventsCount(Liste * myList){
    pthread_mutex_lock(&mutexLockEventsCount);
    int count=0;
    Element *actuel = myList->premier;

    while (actuel->indice > INDICE_INITIAL)
    {
        count++;
        actuel = actuel->suivant;
    }
    pthread_mutex_unlock(&mutexLockEventsCount);

    return count ; //-1 car il faut ignorer l'élément ajouté à l'initailisation
}

extern Liste * Logger_getEvents(int from, int to, Logger * this){
    pthread_mutex_lock(&(this->mutexLockEvents));

    Liste * listePartielle = Logger_initEventList();
    Element *actuel = this->myEvents->premier;
    while(actuel->indice > INDICE_INITIAL){
        if((actuel->indice >= from) && (actuel->indice <= to)){
            ListeChainee_ajout(listePartielle, actuel->logEvent);
        }
        actuel = actuel->suivant;
    }

    pthread_mutex_unlock(&(this->mutexLockEvents));

    return listePartielle;
}

extern void Logger_askEvents(int from,int to, Logger * this){
    Liste * retour = Logger_getEvents(from,to,this);
    RemoteUI_setEvents(retour);

}

extern void Logger_askEventsCount(Liste * liste){
    int nbEvent = Logger_getEventsCount(liste);
    RemoteUI_setEventsCount(nbEvent);
}


extern void Logger_clearEvents(Logger * this){
    Logger_resetEventList(this->myEvents);
}

static void Logger_setTO(Logger * this){
    WatchdogStart(this->watchdogPoll);
}

static void Logger_resetTO(Logger * this){
    WatchdogCancel(this->watchdogPoll);
}

static void Logger_TOHandle(void * this){

    Wrapper wrapper = {
            .data.event = E_TO_POLL
    };
    mailboxSendMsg(((Logger*)this)->mailbox,wrapper.toString);
}


extern void Logger_test(Logger * this)
{
    LogEvent aled0 = {
            .sens.luminosity = 0,
            .sens.collision_f = DOWN,
            .speed = 0
    };

    LogEvent aled1 = {
            .sens.luminosity = 1,
            .sens.collision_f = UP,
            .speed = 1
    };

    LogEvent aled2 = {
            .sens.luminosity = 2,
            .sens.collision_f = DOWN,
            .speed = 2
    };

    LogEvent aled3 = {
            .sens.luminosity = 3,
            .sens.collision_f = UP,
            .speed = 3
    };

    LogEvent aled4 = {
            .sens.luminosity = 1,
            .sens.collision_f = DOWN,
            .speed = 4
    };

    LogEvent aled5 = {
            .sens.luminosity = 2,
            .sens.collision_f = UP,
            .speed = 5
    };

    LogEvent aled6 = {
            .sens.luminosity = 3,
            .sens.collision_f = DOWN,
            .speed = 6
    };

    LogEvent aled7 = {
            .sens.luminosity = 4,
            .sens.collision_f = UP,
            .speed = 7
    };

    Logger_appendEvent(aled0,this);
    Logger_appendEvent(aled1,this);
    Logger_appendEvent(aled2,this);
    Logger_appendEvent(aled3,this);
    Logger_appendEvent(aled4,this);
    Logger_appendEvent(aled5,this);
    Logger_appendEvent(aled6,this);
    Logger_appendEvent(aled7,this);

    int count = Logger_getEventsCount(this->myEvents);

    int from = 3;
    int to = 6;

    Liste * maListe = Logger_getEvents(from,to,this);
    Element * actuel = maListe->premier;
    while (actuel->indice > INDICE_INITIAL){
        printf("%d\n",actuel->logEvent.speed);
        actuel = actuel->suivant;
    }

    Logger_resetEventList(maListe);
    printf("Apré : %d\n",maListe->premier->logEvent.speed);

    Logger_appendEvent(aled0,this);
    Logger_appendEvent(aled1,this);
    Logger_appendEvent(aled2,this);
    Logger_appendEvent(aled3,this);
    Logger_appendEvent(aled4,this);
    Logger_appendEvent(aled5,this);
    Logger_appendEvent(aled6,this);
    Logger_appendEvent(aled7,this);

    maListe = Logger_getEvents(2,4,this);

    actuel = maListe->premier;
    while (actuel->indice > INDICE_INITIAL){
        printf("%d\n",actuel->logEvent.speed);
        actuel = actuel->suivant;
    }
}

