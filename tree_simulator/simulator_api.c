#include <stdio.h>
#include "../utils/application_datatypes.h"

void funzione_stampa(int intero){
    
    printf("intero: %d\n", intero);

}

void send_msg(void* state){

    device_state* ds = (device_state*) state;
    //printf("Num acts types= %d\n", ds->type);

}
