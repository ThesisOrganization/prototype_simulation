#include "linking.h"

void init_P_ACTUATOR_0(device_state* state){
	State_machine* sm = malloc(sizeof(State_machine));
	state->state_machine = sm;
	state_machine_init(sm);
	state_machine_set_state(sm, state);
	state_machine_enter(sm);
}

void fill_init_ptr( void(*** ref_init_ptr)() ){

	ref_init_ptr[P_CENTRAL] = NULL;

	ref_init_ptr[P_REGIONAL] = NULL;

	ref_init_ptr[P_LOCAL] = NULL;

	ref_init_ptr[P_SENSOR] = NULL;

	ref_init_ptr[P_ACTUATOR] = malloc(sizeof( void (*)() ) * 1);
	ref_init_ptr[P_ACTUATOR][0] = init_P_ACTUATOR_0;

	ref_init_ptr[P_LAN] = NULL;

}

void fill_raise_ptr( void(**** ref_raise_ptr)() ){

	ref_raise_ptr[P_CENTRAL] = NULL;

	ref_raise_ptr[P_REGIONAL] = NULL;

	ref_raise_ptr[P_LOCAL] = NULL;

	ref_raise_ptr[P_SENSOR] = NULL;

	ref_raise_ptr[P_ACTUATOR] = malloc(sizeof( void (**)() ) * 1);
	ref_raise_ptr[P_ACTUATOR][0] = malloc(sizeof( void (*)() ) * 1);
	ref_raise_ptr[P_ACTUATOR][0][0] = state_machine_raise_myEvent;

	ref_raise_ptr[P_LAN] = NULL;

}

