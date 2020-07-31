#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

typedef void priority_queue;

priority_queue * create_queue();
void enqueue(priority_queue * pq, int timestamp, void * payload);
void * dequeue(priority_queue * pq);
int check_presence(priority_queue * pq);
void delete_queue(priority_queue * pq);
void print_queue(priority_queue * pq);

#endif /* PRIORITY_QUEUE_H */
