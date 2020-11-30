#ifndef INIT_FUNCTIONS_H
#define INIT_FUNCTIONS_H

#include "../common_functions.h"

#define VALUE_NOT_SET -1.0


void init_node(unsigned int id_device, device_state * state);
void init_sensor(unsigned int id_device, simtime_t now, device_state * state, unsigned int id_lp);
void init_actuator(unsigned int id_device, simtime_t now, device_state * state, unsigned int id_lp);
void init_lan(unsigned int id_device, device_state * state);
void init_wan(unsigned int id_device, device_state * state);

void generate_next_job(unsigned int id_device, simtime_t now, double rate_transition, double random_value, events_type type, unsigned int id_lp);

#endif /* INIT_FUNCTIONS_H */
