#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "priority_scheduler.h"
#include "../priority_queue/priority_queue.h"

/**
 * Print the content of an array of queues.
 * \param[in] array The array of queues.
 * \param[in] size The number of queues in the array.
 */
void print_array(queue_conf** array, int size) {
	int k;
	for (k = 0; k < size; k++)
		printf(" %d: %d %s\n",k,array[k]->type, (char*)array[k]->queue);
	printf("\n");
}

///Creating 2 RT queues and a BATCH queue not in order and giving them to the scheduler, to test the sorting algorithm.
void sort_test(){
	queue_conf *i1=malloc(sizeof(queue_conf)), *i2=malloc(sizeof(queue_conf)), *i3=malloc(sizeof(queue_conf));
	queue_conf** arr1=malloc(sizeof(queue_conf)*3);

	printf("sorting test:\n");
	i1->type=QUEUE_BATCH;
	i1->queue=malloc(sizeof(char)*strlen("first\0"));
	sprintf(i1->queue, "first");
	i2->type=QUEUE_RT;
	i2->queue=malloc(sizeof(char)*strlen("second\0"));
	sprintf(i2->queue, "second");
	i3->type=QUEUE_RT;
	i3->queue=malloc(sizeof(char)*strlen("third\0"));
	sprintf(i3->queue, "third");

	arr1[0]=i1;
	arr1[1]=i2;
	arr1[2]=i3;

	print_array(arr1,3);

	priority_scheduler* sched=new_prio_scheduler(arr1, NULL, 3, 0, 0, UPGRADE_PRIO);

	print_array(arr1,3);

}

///Testing the sorting algorithm inside the scheduler and the scheduling with 3 input queues, 1 output queues and 10 jobs.
int main(int argc, char** argv) {

	sort_test();

	queue_conf *i1=malloc(sizeof(queue_conf)), *i2=malloc(sizeof(queue_conf)), *i3=malloc(sizeof(queue_conf)), *o=malloc(sizeof(queue_conf));
	queue_conf** arr=malloc(sizeof(queue_conf*)*3);

	printf("schedule test with 3 input queue and 1 output queue:\n");
	i1->type=QUEUE_RT;
	i1->queue=create_queue();
	i1->enqueue=enqueue;
	i1->dequeue=dequeue;
	i1->check_presence=check_presence;
	i1->check_full=NULL;
	i2->type=QUEUE_LOSSY;
	i2->queue=create_queue();
	i2->enqueue=enqueue;
	i2->dequeue=dequeue;
	i2->check_presence=check_presence;
	i2->check_full=NULL;
	i3->type=QUEUE_BATCH;
	i3->queue=create_queue();
	i3->enqueue=enqueue;
	i3->dequeue=dequeue;
	i3->check_presence=check_presence;
	i3->check_full=NULL;
	o->type=QUEUE_BATCH;
	o->queue=create_queue();
	o->enqueue=enqueue;
	o->dequeue=dequeue;
	o->check_presence=check_presence;
	o->check_full=NULL;

	printf("created queues\n");

	arr[0]=i1;
	arr[1]=i2;
	arr[2]=i3;

	printf("put queues into an array\n");

	priority_scheduler* sched=new_prio_scheduler(arr, &o, 3, 1, 0, UPGRADE_PRIO);

	int i;
	int payload[10];

	printf("enqueuing jobs\n");

	for(i=0;i<10;i++){
		payload[i]=rand();
		enqueue(i1->queue,rand(),&payload[i]);
		enqueue(i2->queue,rand(),&payload[i]);
		enqueue(i3->queue,rand(),&payload[i]);
	}
	printf("queues before scheduling\n");
	for(i=0;i<3;i++){
		printf("input queue %d\n type: %d, items: %d\n contents:\n",i,arr[i]->type,(arr[i]->check_presence)(arr[i]->queue));
		print_queue(arr[i]->queue);
	}
	printf("output queue \n type: %d, items: %d\n contents:\n",o->type,(o->check_presence)(o->queue));
	print_queue(o);


	printf("scheduling jobs\n");
	for(i=0;i<10;i++){
		schedule(sched);
	}

	printf("queues after scheduling\n");
	for(i=0;i<3;i++){
		printf("input queue %d\n type: %d, items: %d\n contents:\n",i,arr[i]->type,(arr[i]->check_presence)(arr[i]->queue));
		print_queue(arr[i]->queue);
	}
	printf("output queue \n type: %d, items: %d\n contents:\n",o->type,(o->check_presence)(o->queue));
	print_queue(o->queue);


	return EXIT_SUCCESS;
}
