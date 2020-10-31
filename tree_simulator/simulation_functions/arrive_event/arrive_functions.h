#ifndef ARRIVE_FUNCTIONS_H
#define ARRIVE_FUNCTIONS_H

#include "../common_functions.h"

void arrive_node(unsigned int id_device, simtime_t now, device_state * state, job_info * info, unsigned int id_lp);
void arrive_actuator(unsigned int id_device, simtime_t now, device_state * state, job_info * info, unsigned int id_lp);
void arrive_lan(unsigned int id_device, simtime_t now, device_state * state, job_info * info, unsigned int id_lp);
void arrive_wan(unsigned int id_device, simtime_t now, device_state * state, job_info * info);

void arrive_disk(unsigned int id_device, simtime_t now, device_state * state, job_info * info, unsigned int id_lp);


#endif /* ARRIVE_FUNCTIONS_H */
