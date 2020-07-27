/** \file priority_scheduler.c
 * \brief Implementation of the priority scheduler
 */

#include "priority_scheduler.h"
#include "quick_sort.h"
#include <stdlib.h>

/** Allocates a new `::priority_scheduler` struct with the given parameters
 * If the order of the queue of the same type is meaningful (usually when `::UPDATE_PRIO` is not used) the array must be already sorted since the sorting algorithm will mess up the relative position of elements of the same type.
 */
priority_scheduler* new_prio_scheduler(queue_conf** input, queue_conf** output, int num_input, int num_output, unsigned int events, int prio){

	//we sort the input and output queues by their type
	quicksort(input,num_input);
	quicksort(output,num_output);

	//we allocate data for our scheduler
	priority_scheduler* sched = (priority_scheduler*) malloc(sizeof(priority_scheduler));
	sched->input_queues=input;
	sched->output_queues=output;
	sched->num_input_queues=num_input;
	sched->num_output_queues=num_output;
	sched->events_to_schedule=events;
	sched->mix_prio=prio;

	return sched;
}

/** \brief Check is an output queue is infinite or has still space for jobs
 * \param[in] output The output queue to be checked
 */
int check_queues(queue_conf* output){
	int infinite_queue,queue_full=0;
	//check if the queue is infinite
	infinite_queue= output->check_full==NULL;
	if(!infinite_queue){
		//check if the queue is full
		queue_full=(output->check_full)(output->queue);
	}
	return infinite_queue || !queue_full;
}

/**
 * To schedule the jobs extract them in order from the input queues, according to the type of queue.
 * The queue in which a job is placed is chosen by considering the order in which the output queues are placed, with the
 * possibility to upgrade the priority of a job (See `::UPGRADE_PRIO`) if there is a suitable queue available.
 * __NOTE:__ It is possible that some jobs can be not scheduled depending on the output queue characteristics and on the usage of `::UPDATE_PRIO`.
 */
void schedule(priority_scheduler* sched){
	int i,output_index=0;
	int job_index=0;
	void* job;
	// we schedule events from the input queues (sorted by type and priority) to the output queues (sorted by type and priority)
	for(i=0;i<sched->num_input_queues;i++){
		//we schedule jobs from the input queue to the output queues, according to the configured number of events to be scheduled
		for(;job_index<sched->events_to_schedule || (sched->events_to_schedule==0 && (sched->input_queues[i]->check_presence)(sched->input_queues[i]->queue));job_index++){
			//we find a suitable output queue
			if(sched->mix_prio==UPGRADE_PRIO){
				//if the priority of a job can be upgraded we choose the first queue which is not full
				while(!check_queues(sched->output_queues[output_index])){
					output_index++;
					//if we don't have any other output queues we cannot schedule jobs
					if(output_index>=sched->num_output_queues){
						/// \todo what happens when we cannot schedule jobs from an RT queue?
						return;
					}
				}
			} else {
				// if the job priority must not be upgraded, we try to schedule jobs from the ith input queue to the ith output queue
				if(check_queues(sched->output_queues[i])){
					output_index=i;
				} else {
					// we cannot schedule an event from the ith input queue, so we jump to the next input queue
					/// \todo see if an error must be raised if an RT queue is jumped
					break;
				}
			}
			//we can now schedule the job from the ith input queue to a suitable output queue
			//we dequeue the job from the ith input queue
			job=(sched->input_queues[i]->dequeue)(sched->input_queues[i]->queue);
			//we enqueue the job on the chosen output queue
			(sched->output_queues[output_index]->enqueue)(sched->output_queues[output_index]->queue,job_index,job);
		}
	}
}
