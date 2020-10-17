#include "../common_functions.h"

#ifndef INIT_FUNCTIONS_H
#define INIT_FUNCTIONS_H

void init_node(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_sensor(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_actuator(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_lan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_wan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);

#endif /* INIT_FUNCTIONS_H */
