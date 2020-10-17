#include "../common_functions.h"

#ifndef ARRIVE_FUNCTIONS_H
#define ARRIVE_FUNCTIONS_H

void arrive_node(unsigned int me, simtime_t now, lp_state * state, job_info * info);
void arrive_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info);
void arrive_lan(unsigned int me, simtime_t now, lp_state * state, job_info * info);
void arrive_wan(unsigned int me, simtime_t now, lp_state * state, job_info * info);


#endif /* ARRIVE_FUNCTIONS_H */
