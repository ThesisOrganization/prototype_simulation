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

/**Testing the sorting algorithm inside the scheduler and the scheduling with 3 input queues, 1 output queues and 10 jobs.
 * The jobs have a random timestamp between 0 and 9 so it is possible that some job in the lossy queue can be discarded
 */
int main(int argc, char** argv) {

	queue_conf *i1=malloc(sizeof(queue_conf)), *i2=malloc(sizeof(queue_conf)), *i3=malloc(sizeof(queue_conf));
	queue_conf *o=malloc(sizeof(queue_conf));
	queue_conf **arr=malloc(sizeof(queue_conf*)*3);

	printf("schedule test with 3 input queue and 1 output queue:\n");
	i1->type=REAL_TIME;
	i1->queue=create_queue();
	i1->enqueue=enqueue;
	i1->dequeue=dequeue;
	i1->check_presence=check_presence;
	i1->check_full=NULL;
	i2->type=LOSSY;
	i2->queue=create_queue();
	i2->enqueue=enqueue;
	i2->dequeue=dequeue;
	i2->check_presence=check_presence;
	i2->check_full=NULL;
	i3->type=BATCH;
	i3->queue=create_queue();
	i3->enqueue=enqueue;
	i3->dequeue=dequeue;
	i3->check_presence=check_presence;
	i3->check_full=NULL;
	o->type=BATCH;
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

	//priority_scheduler* sched=new_prio_scheduler(arr, &o, 3, 1, 2, UPGRADE_PRIO);
	priority_scheduler* sched=new_prio_scheduler(arr, NULL, 3, 0, 1, UPGRADE_PRIO);

	int i=0,j,res;
	job_info tmp;
	tmp.job_type=INVALID_JOB;

	printf("scheduling jobs on the input queues\n");
	for(i=0;i<10;i++){
		tmp.type=rand() % 3; // we choose a random priority between REAL_TIME, LOSSY and BATCH
		tmp.deadline=drand48()*9+1;
		tmp.job_type=lrand48()%NUM_OF_JOB_TYPE;
		res=schedule_in(sched,tmp);
		printf("schedule result: %d ",res);
	}
	printf("\n");
	printf("queues before scheduling\n");
	for(i=0;i<3;i++){
		printf("input queue %d\n type: %d, items: %d\n contents:\n",i,arr[i]->type,(arr[i]->check_presence)(arr[i]->queue));
		print_queue(arr[i]->queue);
	}
	printf("output queue \n type: %d, items: %d\n contents:\n",o->type,(o->check_presence)(o->queue));
	print_queue(o);

	printf("elements in o %d\n",o->check_presence(o->queue));

	job_info* jobs=malloc(sizeof(job_info)*1);
	memset(jobs,0,sizeof(job_info)*1);
	int num_jobs=1;
	printf("scheduling out jobs\n");
	for(i=0;i<10;i++){
		res=schedule_out(sched,jobs,num_jobs);
		printf("time: %d\n",i);
		if(jobs!=NULL){
			for(j=0;j<num_jobs;j++){
				printf("job type: %d, deadline %f, result:%d\n",jobs[j].type,jobs[j].deadline,res);
			}
		}
		printf("time++\n");
	}
	free(jobs);

	printf("input queues after scheduling\n");
	for(i=0;i<3;i++){
		printf("input queue %d\n type: %d, items: %d\n contents:\n",i,arr[i]->type,(arr[i]->check_presence)(arr[i]->queue));
		print_queue(arr[i]->queue);
	}

	printf("extracting elements from the output queue and printing them:\n");
	while((o->check_presence)(o->queue)){
		printf("output queue \n type: %d, items: %d\n contents:\n",o->type,(o->check_presence)(o->queue));
		print_queue(o->queue);
		tmp=dequeue(o->queue);
		printf("job type: %d, deadline %f\n",tmp.type,tmp.deadline);
	}
	printf("output queue \n type: %d, items: %d\n contents:\n",o->type,(o->check_presence)(o->queue));
	print_queue(o->queue);
	// deallocating queues
	for(i=0;i<3;i++){
		delete_queue(arr[i]->queue);
		free(arr[i]);
	}
	free(arr);
	delete_queue(o->queue);
	free(o);
	free(sched);
	return EXIT_SUCCESS;
}
