/** Generated by YAKINDU Statechart Tools code generator. */

#ifndef NOMEMIO_H_
#define NOMEMIO_H_

#ifdef __cplusplus
extern "C" { 
#endif

/*!
* Forward declaration for the Nomemio state machine.
*/
typedef struct Nomemio Nomemio;

/*!
* Forward declaration of the data structure for the NomemioIface interface scope.
*/
typedef struct NomemioIface NomemioIface;

#ifdef __cplusplus
}
#endif

#include "../src/sc_types.h"
#include "../simulator_api.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*! \file Header of the state machine 'Nomemio'.
*/

#ifndef NOMEMIO_EVENTQUEUE_BUFFERSIZE
#define NOMEMIO_EVENTQUEUE_BUFFERSIZE 20
#endif
#ifndef NOMEMIO_IN_EVENTQUEUE_BUFFERSIZE
#define NOMEMIO_IN_EVENTQUEUE_BUFFERSIZE NOMEMIO_EVENTQUEUE_BUFFERSIZE
#endif
#ifndef SC_INVALID_EVENT_VALUE
#define SC_INVALID_EVENT_VALUE 0
#endif
/*! Define number of states in the state enum */
#define NOMEMIO_STATE_COUNT 2

/*! Define dimension of the state configuration vector for orthogonal states. */
#define NOMEMIO_MAX_ORTHOGONAL_STATES 1

/*! Define indices of states in the StateConfVector */
#define SCVI_NOMEMIO_MAIN_REGION_STATEA 0
#define SCVI_NOMEMIO_MAIN_REGION_STATEB 0


/*
 * Enum of event names in the statechart.
 */
typedef enum  {
	Nomemio_invalid_event = SC_INVALID_EVENT_VALUE,
	Nomemio_myEvent
} NomemioEventID;

/*
 * Struct that represents a single event.
 */
typedef struct {
	NomemioEventID name;
} nomemio_event;

/*
 * Queue that holds the raised events.
 */
typedef struct nomemio_eventqueue_s {
	nomemio_event *events;
	sc_integer capacity;
	sc_integer pop_index;
	sc_integer push_index;
	sc_integer size;
} nomemio_eventqueue;

/*! Enumeration of all states */ 
typedef enum
{
	Nomemio_last_state,
	Nomemio_main_region_StateA,
	Nomemio_main_region_StateB
} NomemioStates;


/*! Type declaration of the data structure for the NomemioIface interface scope. */
struct NomemioIface
{
	sc_boolean myEvent_raised;
};






/*! 
 * Type declaration of the data structure for the Nomemio state machine.
 * This data structure has to be allocated by the client code. 
 */
struct Nomemio
{
	NomemioStates stateConfVector[NOMEMIO_MAX_ORTHOGONAL_STATES];
	sc_ushort stateConfVectorPosition; 
	NomemioIface iface;
	sc_boolean isExecuting;
	nomemio_eventqueue in_event_queue;
	nomemio_event in_buffer[NOMEMIO_IN_EVENTQUEUE_BUFFERSIZE];
};



/*! Initializes the Nomemio state machine data structures. Must be called before first usage.*/
extern void nomemio_init(Nomemio* handle);


/*! Activates the state machine. */
extern void nomemio_enter(Nomemio* handle);

/*! Deactivates the state machine. */
extern void nomemio_exit(Nomemio* handle);




/*! Raises the in event 'myEvent' that is defined in the default interface scope. */ 
extern void nomemio_raise_myEvent(Nomemio* handle);

/*!
 * Checks whether the state machine is active (until 2.4.1 this method was used for states).
 * A state machine is active if it was entered. It is inactive if it has not been entered at all or if it has been exited.
 */
extern sc_boolean nomemio_is_active(const Nomemio* handle);

/*!
 * Checks if all active states are final. 
 * If there are no active states then the state machine is considered being inactive. In this case this method returns false.
 */
extern sc_boolean nomemio_is_final(const Nomemio* handle);

/*! Checks if the specified state is active (until 2.4.1 the used method for states was called isActive()). */
extern sc_boolean nomemio_is_state_active(const Nomemio* handle, NomemioStates state);


#ifdef __cplusplus
}
#endif 

#endif /* NOMEMIO_H_ */
