#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "../application_datatypes.h"
#include "heap/heap.h"

typedef void priority_queue;

priority_queue * create_queue(void);
void enqueue(priority_queue * pq, double timestamp, job_info payload);
job_info dequeue(priority_queue * pq);
int check_presence(priority_queue * pq);
void delete_queue(priority_queue * pq);
void print_queue(priority_queue * pq);

#endif /* PRIORITY_QUEUE_H */
