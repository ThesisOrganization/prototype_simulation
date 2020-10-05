#include "../utils/priority_scheduler/priority_scheduler.h"
#include "../utils/partop/header.h"
#include "../utils/priority_queue/priority_queue.h"
#include "../utils/application_datatypes.h"

#ifndef SIMULATION_FUNCTIONS_H
#define SIMULATION_FUNCTIONS_H

void init_node(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_sensor(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_actuator(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_lan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);
void init_wan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos);

void arrive_node(unsigned int me, simtime_t now, lp_state * state, job_info * info);
void arrive_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info);
void arrive_lan(unsigned int me, simtime_t now, lp_state * state, job_info * info);

#endif /* SIMULATION_FUNCTIONS_H */
