#include "../../../utils/priority_scheduler/priority_scheduler.h"
#include "../../../utils/partop/header.h"
#include "../../../utils/priority_queue/priority_queue.h"
#include "../../../utils/application_datatypes.h"

#ifndef FINISH_FUNCTIONS_H
#define FINISH_FUNCTIONS_H

void finish_node(unsigned int me, simtime_t now, lp_state * state);
void finish_actuator(unsigned int me, simtime_t now, lp_state * state);
void finish_lan(unsigned int me, simtime_t now, lp_state * state, lan_direction direction);


#endif /* FINISH_FUNCTIONS_H */
