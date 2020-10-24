#ifndef INIT_FUNCTIONS_H
#define INIT_FUNCTIONS_H

#include "../common_functions.h"

void init_node(unsigned int me, lp_state * state);
void init_sensor(unsigned int me, simtime_t now, lp_state * state);
void init_actuator(unsigned int me, simtime_t now, lp_state * state);
void init_lan(unsigned int me, lp_state * state);
void init_wan(unsigned int me, lp_state * state);

void generate_next_job(unsigned int me, simtime_t now, double rate_transition, double random_value, job_type type);

#endif /* INIT_FUNCTIONS_H */
