#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "heap/heap.h"
#include "priority_queue.h"

void print_help() {
    printf("Richiesto argomento: {min_heap, array, sort, change}\n");
}

void print_array(int * array, int size) {
    int k;
    for (k = 0; k < size; k++)
        printf(" %d", array[k]);
    printf("\n");
}

int main(int argc, char** argv) {


    priority_queue * pq = create_queue();

    int payload1 = 32;
    int payload2 = 21;

    enqueue(pq, 1, &payload1);
    enqueue(pq, 3, &payload2);

    print_queue(pq);

    int out = *((int *) dequeue(pq));

    printf("%d\n", out);

    print_queue(pq);

    printf("%d\n", check_presence(pq) );

    delete_queue(pq);


    
    return EXIT_SUCCESS;
}
