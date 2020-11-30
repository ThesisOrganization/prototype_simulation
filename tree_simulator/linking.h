#ifndef LINKING_H
#define LINKING_H

#include <stdlib.h>
#include "../utils/application_datatypes.h"
#include "../tests_topology/state_machines/state_machine_f/src-gen/state_machine.h"
#include "../tests_topology/state_machines/state_machine_f/src/sc_types.h"

typedef enum _element_type{
	P_CENTRAL = 0,
	P_REGIONAL,
	P_LOCAL,
	P_SENSOR,
	P_ACTUATOR,
	P_LAN,
	NUM_OF_ELEMENTS_TYPES
} element_type;

void fill_init_ptr( void(*** ref_init_ptr)() );
void fill_raise_ptr( void(**** ref_raise_ptr)() );

#endif /* LINKING_H */
