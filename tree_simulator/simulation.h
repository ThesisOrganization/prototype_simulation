#ifndef SIMULATION_H
#define SIMULATION_H

#include "./simulation_functions/init_event/init_functions.h"
#include "./simulation_functions/arrive_event/arrive_functions.h"
#include "./simulation_functions/finish_event/finish_functions.h"
#include "./simulation_functions/setup_protocol/setup_protocol.h"

#include <math.h>

#define RANDOM_START 500
#define TRANSITION_TIME_LIMIT 300000
#define MAX_SIMULATION_TIME 100000000

#define NEXT_UPDATE_TIMESTAMP 100000
#define THRESHOLD 3 ///percentage

#define MIN_NUMBER_OF_EVENTS_ALL 50
#define MIN_NUMBER_OF_EVENTS_DISK 20

#define SHIFT_EVENT_GEN_TRANSITION 0.1
#define SHIFT_EVENT_GEN_TELEMETRY 0.1
#define SHIFT_EVENT_WAN 0.1
/*
#define SHIFT_EVENT_TELEMETRY 2.0
#define SHIFT_EVENT_TRANSITION 2.0
#define SHIFT_EVENT_COMMAND 2.0
#define SHIFT_EVENT_BATCH 2.0
#define SHIFT_EVENT_REPLY 2.0
*/
#define SHIFT_EVENT_DISK 0.1
#define SHIFT_EVENT 0.1


#define SHIFT_EVENT_FIRST 0.1
#define SHIFT_EVENT_SECOND 0.2
#define SHIFT_EVENT_THIRD 0.3
#define SHIFT_EVENT_FOURTH 0.4
#define SHIFT_EVENT_FIFTH 0.5
#define SHIFT_EVENT_SIXTH 0.6

#endif /* SIMULATION_H */
