#include <stdio.h>
#include "../utils/application_datatypes.h"

void funzione_stampa(int intero){
    
    printf("intero: %d\n", intero);

}

void send_msg(void* state){

    device_state* ds = (device_state*) state;
		if(ds->info.actuator->queue_state->C[COMMAND] % 100 == 1)
			printf("Num command received= %d\n", ds->info.actuator->queue_state->C[COMMAND]);

}
