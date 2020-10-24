#ifndef FINISH_FUNCTIONS_H
#define FINISH_FUNCTIONS_H

#include "../common_functions.h"

void finish_node(unsigned int me, simtime_t now, lp_state * state);
void finish_actuator(unsigned int me, simtime_t now, lp_state * state);
void finish_lan(unsigned int me, simtime_t now, lp_state * state, lan_direction direction);

void finish_disk(unsigned int me, simtime_t now, lp_state * state);


#endif /* FINISH_FUNCTIONS_H */
