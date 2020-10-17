#include "../common_functions.h"

#ifndef FINISH_FUNCTIONS_H
#define FINISH_FUNCTIONS_H

void finish_node(unsigned int me, simtime_t now, lp_state * state);
void finish_actuator(unsigned int me, simtime_t now, lp_state * state);
void finish_lan(unsigned int me, simtime_t now, lp_state * state, lan_direction direction);


#endif /* FINISH_FUNCTIONS_H */
