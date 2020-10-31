#ifndef FINISH_FUNCTIONS_H
#define FINISH_FUNCTIONS_H

#include "../common_functions.h"

void finish_node(unsigned int id_device, simtime_t now, device_state * state, unsigned int id_lp);
void finish_actuator(unsigned int id_device, simtime_t now, device_state  * state, unsigned int id_lp);
void finish_lan(unsigned int id_device, simtime_t now, device_state  * state, lan_direction direction, unsigned int id_lp);

void finish_disk(unsigned int id_device, simtime_t now, device_state  * state, unsigned int id_lp);


#endif /* FINISH_FUNCTIONS_H */
