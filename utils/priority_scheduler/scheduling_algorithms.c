/** \file scheduling_algorithms.c
 * This file exports the datastructres used to select the algorithms that will govern the scheduler.
 */
#include "scheduling_algorithms.h"
#include "../priority_queue/priority_queue.h"

/** \brief selects the firsts non-empty queue according to the priority
 * \param[in] num_input_queues The number of input queues.
 * \param[in] input_queues The queue array.
 * \returns The queue index or ::SCHEDULE_FAIL
 */
static int prio_fifo_alogrithm(int num_input_queues, queue_conf** input_queues){
	double min=SCHEDULE_FAIL;
	int i=0,min_index=SCHEDULE_FAIL;
	prio_type prio;
	//we start from the highest priority
	for(prio=REAL_TIME;prio<NUM_OF_PRIO_TYPES && min_index==SCHEDULE_FAIL;prio++){
		//we select the minimum timestamp for that priority
		//since the queues are ordered according to priority we can skip to the next priority if we did not found a queue for the current priority
		//printf("checking prio %d\n",prio);
		for(i=0;i<num_input_queues;i++){
			if(input_queues[i]->prio==prio){
				if((input_queues[i]->check_presence)(input_queues[i]->queue)>0){
					if((input_queues[i]->peek)(input_queues[i]->queue)<min || min<0){
						min=(input_queues[i]->peek)(input_queues[i]->queue);
						min_index=i;
					}
				}
			}
		}
	}
	//if we found a suitable queue for the current priority we return its index
	if(min!=SCHEDULE_FAIL){
		return min_index;
	}else{
		return SCHEDULE_FAIL;
	}
}

/** \brief Selects the output queue based on priority, type and the last scheduled element.
 * \param[in] num_input_queues The number of input queues.
 * \param[in] input_queues The queue array.
 * \param[in] last_scheduled The type of the last scheduled job.
 * \returns The queue index or ::SCHEDULE_FAIL
 */
static int round_robin_algorithm(int num_input_queues, queue_conf** input_queues, job_type last_scheduled){
	int queue=SCHEDULE_FAIL;
	int i=0;
	prio_type prio;
	job_type type;
// 	printf("last scheduled: %d\n",last_scheduled);
	//we start from the highest priority, exploiting the fact that queues are ordered by priority
	type=(last_scheduled+1)% NUM_OF_JOB_TYPE;
	for(prio=REAL_TIME;prio<NUM_OF_PRIO_TYPES && queue==SCHEDULE_FAIL ;prio++){
// 		printf("checkgin prio %d\n",prio);
// 		printf("type: %d\n",(last_scheduled+1)%NUM_OF_JOB_TYPE);
		//we start searching from the type of data which is next from the last scheduled data
		for(int j=0; j<NUM_OF_JOB_TYPE && queue==SCHEDULE_FAIL; j++){
// 			printf("checking type %d\n",type);
			//we search all the queues with the matching type and priority
			for(i=0;i<num_input_queues && queue==SCHEDULE_FAIL;i++){
// 				printf("checking queue: %d,  typeod of queue: %d\n",i,input_queues[i]->type);
				if(input_queues[i]->type==type &&(input_queues[i]->check_presence)(input_queues[i]->queue)>0){
// 					printf("queue non empty\n");
// 					printf("output queue \n priority%d type: %d, items: %d\n contents:\n",input_queues[i]->prio,input_queues[i]->type,(input_queues[i]->check_presence)(input_queues[i]->queue));
// 					print_queue(input_queues[i]->queue);
					//if we get the right type
					queue=i;
				}
			}
		type=(type+1)%NUM_OF_JOB_TYPE;
		}
	}
	//if(queue == SCHEDULE_FAIL){
	//	printf("output queue \n priority%d type: %d, items: %d\n contents:\n",o->prio,o->type,(o->check_presence)(o->queue));
	//	print_queue(o->queue);
	//}
// 	printf("queue:%d\n",queue);
	return queue;
}

int select_queue(priority_scheduler* sched){
	int queue=SCHEDULE_FAIL;
	switch(sched->sched_algo){
		case SCHED_PRIO_FIFO:
			queue=prio_fifo_alogrithm(sched->num_input_queues,sched->input_queues);
			break;
		case SCHED_RR:
			queue=round_robin_algorithm(sched->num_input_queues,sched->input_queues,sched->last_schedule_out);
			break;
	}
	
	return queue;
}
