#ifndef SIMULATION_H
#define SIMULATION_H

#include "./simulation_functions/init_event/init_functions.h"
#include "./simulation_functions/arrive_event/arrive_functions.h"
#include "./simulation_functions/finish_event/finish_functions.h"
#include "./simulation_functions/setup_protocol/setup_protocol.h"

#define RANDOM_START 500
#define TRANSITION_TIME_LIMIT 300000
#define MAX_SIMULATION_TIME 100000000

#define NEXT_UPDATE_TIMESTAMP 100000
#define THRESHOLD 3 ///percentage

#define MIN_NUMBER_OF_EVENTS_ALL 50
#define MIN_NUMBER_OF_EVENTS_DISK 20

#endif /* SIMULATION_H */
