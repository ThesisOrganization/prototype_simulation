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

	queue_conf *i1=malloc(sizeof(queue_conf)*NUM_OF_JOB_TYPE), *i2=malloc(sizeof(queue_conf)*NUM_OF_JOB_TYPE), *i3=malloc(sizeof(queue_conf)*NUM_OF_JOB_TYPE);
	int k,i;
	queue_conf *o=malloc(sizeof(queue_conf));
	queue_conf **arr=malloc(sizeof(queue_conf*)*NUM_OF_JOB_TYPE*3);

	for(k=TELEMETRY;k<NUM_OF_JOB_TYPE;k++){
		i1[k].prio=REAL_TIME;
		i1[k].queue=create_queue();
		i1[k].enqueue=enqueue;
		i1[k].dequeue=dequeue;
		i1[k].check_presence=check_presence;
		i1[k].check_full=NULL;
		i1[k].type=k;
		i1[k].peek=queue_peek;
		i2[k].prio=LOSSY;
		i2[k].queue=create_queue();
		i2[k].enqueue=enqueue;
		i2[k].dequeue=dequeue;
		i2[k].check_presence=check_presence;
		i2[k].check_full=NULL;
		i2[k].type=k;
		i2[k].peek=queue_peek;
		i3[k].prio=BATCH;
		i3[k].type=k;
		i3[k].queue=create_queue();
		i3[k].enqueue=enqueue;
		i3[k].dequeue=dequeue;
		i3[k].check_presence=check_presence;
		i3[k].check_full=NULL;
		i3[k].peek=queue_peek;
	}
	o->type=INVALID_JOB;
	o->prio=BATCH;
	o->queue=create_queue();
	o->enqueue=enqueue;
	o->dequeue=dequeue;
	o->check_presence=check_presence;
	o->check_full=NULL;

	printf("created queues\n");
	i=0;
	for(k=TELEMETRY;k<NUM_OF_JOB_TYPE;k++){
		arr[i]=&i1[k];
		arr[i+1]=&i2[k];
		arr[i+2]=&i3[k];
		i+=3;
	}

	printf("put queues into an array\n");

	//priority_scheduler* sched=new_prio_scheduler(arr, &o, 3, 1, 2, UPGRADE_PRIO);
	priority_scheduler* sched=new_prio_scheduler(arr, NULL, 3*NUM_OF_JOB_TYPE, 0, 1, UPGRADE_PRIO,SCHED_RR);

	int j,res;
	job_info tmp;

	printf("scheduling jobs on the input queues\n");
	for(i=0;i<10*NUM_OF_JOB_TYPE;i++){
		printf("scheduling job %d ",i);
		tmp.job_type=rand()%NUM_OF_JOB_TYPE;
		tmp.prio=rand() % 3; // we choose a random priority between REAL_TIME, LOSSY and BATCH
		tmp.deadline=drand48()*9+1;
		tmp.job_type=lrand48()%NUM_OF_JOB_TYPE;
		res=schedule_in(sched,tmp);
		printf("schedule result: %d\n",res);
	}
	printf("\n");
	printf("queues before scheduling\n");
	for(i=0;i<3*NUM_OF_JOB_TYPE;i++){
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
	for(i=0;i<10*NUM_OF_JOB_TYPE;i++){
		printf("job %d ",i);
		res=schedule_out(sched,jobs,num_jobs);
		printf("time: %d\n",i);
		if(jobs!=NULL){
			for(j=0;j<num_jobs;j++){
				printf("job priority: %d, type: %d, deadline %f, result:%d\n",jobs[j].prio,jobs[j].job_type,jobs[j].deadline,res);
			}
		}
		printf("time++\n");
	}
	free(jobs);

	printf("input queues after scheduling\n");
	for(i=0;i<3*NUM_OF_JOB_TYPE;i++){
		printf("input queue %d\n priority:%d type: %d, items: %d\n contents:\n",i,arr[i]->prio,arr[i]->type,(arr[i]->check_presence)(arr[i]->queue));
		print_queue(arr[i]->queue);
	}

	printf("extracting elements from the output queue and printing them:\n");
	while((o->check_presence)(o->queue)){
		printf("output queue \n priority%d type: %d, items: %d\n contents:\n",o->prio,o->type,(o->check_presence)(o->queue));
		print_queue(o->queue);
		tmp=dequeue(o->queue);
		printf("job priority%d, type: %d, deadline %f\n",tmp.prio,tmp.job_type,tmp.deadline);
	}
	printf("output queue \n priority:%d type: %d, items: %d\n contents:\n",o->prio,o->type,(o->check_presence)(o->queue));
	print_queue(o->queue);
	// deallocating queues
	for(i=0;i<3*NUM_OF_JOB_TYPE;i++){
		delete_queue(arr[i]->queue);
	}
	free(i1);
	free(i2);
	free(i3);
	free(arr);
	delete_queue(o->queue);
	free(o);
	free(sched);
	return EXIT_SUCCESS;
}
