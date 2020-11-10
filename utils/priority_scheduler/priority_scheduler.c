/** \file priority_scheduler.c
 * \brief Implementation of the priority scheduler
 */

#include "priority_scheduler.h"
#include "queue_quick_sort.h"
#include <stdlib.h>
#include <string.h>

/** Allocates a new `::priority_scheduler` struct with the given parameters
 * If the order of the queue of the same type is meaningful (usually when `::UPDATE_PRIO` is not used) the array must be already sorted since the sorting algorithm will mess up the relative position of elements of the same type.
 * A `NULL` pointer and 0 can be used to configure a scheduler with no available output queues, it will return to the caller jobs extracted from the input queues.
 */
priority_scheduler* new_prio_scheduler(queue_conf** input, queue_conf** output, int num_input, int num_output, unsigned int events, int prio){

	//we sort the input and output queues by their type
	queue_quicksort(input,num_input);
	queue_quicksort(output,num_output);

	//we allocate data for our scheduler
	priority_scheduler* sched = (priority_scheduler*) malloc(sizeof(priority_scheduler));
	sched->input_queues=input;
	sched->num_input_queues=num_input;
	sched->num_output_queues=num_output;
	if(num_output>0){
		sched->output_queues=output;
	} else{
		sched->output_queues=NULL;
	}
	if(events<=0 && num_output==0){
		sched->events_to_schedule=1;
	} else {
		sched->events_to_schedule=events;
	}
	sched->mix_prio=prio;
	sched->scheduler_timestamp=0;
	return sched;
}

/** \brief Check is an output queue is infinite or has still space for jobs
 * \param[in] output The output queue to be checked
 */
int check_queues(queue_conf* queue){
	int infinite_queue,queue_full=0;
	//check if the queue is infinite
	infinite_queue= queue->check_full==NULL;
	if(!infinite_queue){
		//check if the queue is full
		queue_full=(queue->check_full)(queue->queue);
	}
	return infinite_queue || !queue_full;
}

/**
 * To schedule the jobs on the output queues we extract them in order from the input queues, according to the type of queue.
 * The queue in which a job is placed is chosen by considering the order in which the output queues are placed, with the
 * possibility to upgrade the priority of a job (See `::UPGRADE_PRIO`) if there is a suitable queue available.
 * __NOTE:__ It is possible that some jobs can be not scheduled depending on the output queue characteristics and on the usage of `::UPDATE_PRIO`.
 * Before scheduling jobs, if the output queues are all empty the `scheduler_timestamp` is reset, to try avoiding overflows.
 * Real time jobs that don't meet the deadline are scheduled regardless (they shoul always meet the deadline).
 * If there were no output queues supplied, the function will return up to `events_to_schedule` jobs, according to the scheduler configuration.
 */
job_info** schedule_out(priority_scheduler* sched){
	int i,output_index=0,output_empty=1,job_index=0;
	job_info *job, **jobs=NULL;

	if(sched->num_output_queues>0){
		// we check if all output queues are empty
		for(i=0;i<sched->num_output_queues && output_empty;i++){
			output_empty= output_empty && (sched->output_queues[i]->check_presence)(sched->output_queues[i]->queue)==0;
		}
	} else{
		jobs=malloc(sizeof(job_info*)*(sched->events_to_schedule+1));
		memset(jobs,0,sizeof(job_info*)*(sched->events_to_schedule+1));
	}

	//if all output queues are empty we can reset the scheduler timestamp
	if(output_empty || sched->num_output_queues==0){
		sched->scheduler_timestamp=0;
	}

	// we schedule events from the input queues (sorted by type and priority) to the output queues (sorted by type and priority)
	for(i=0;i<sched->num_input_queues && (job_index<sched->events_to_schedule || sched->events_to_schedule==0);i++){
		//we schedule jobs from the input queue to the output queues, according to the configured number of events to be scheduled
		for(;(job_index<sched->events_to_schedule || sched->events_to_schedule==0) && (sched->input_queues[i]->check_presence)(sched->input_queues[i]->queue)>0;job_index++){
			if(sched->num_output_queues>0){
				//we find a suitable output queue
				if(sched->mix_prio==UPGRADE_PRIO){
					//if the priority of a job can be upgraded we choose the first queue which is not full
					while(!check_queues(sched->output_queues[output_index])){
						output_index++;
						//if we don't have any other output queues we cannot schedule jobs
						if(output_index>=sched->num_output_queues){
							return jobs;
						}
					}
				} else {
					// if the job priority must not be upgraded, we try to schedule jobs from the ith input queue to the ith output queue
					if(check_queues(sched->output_queues[i])){
						output_index=i;
					} else {
						// we cannot schedule an event from the ith input queue, so we jump to the next input queue
						break;
					}
				}
			}
			//we can now schedule the job from the ith input queue to a suitable output queue
			//we dequeue the job from the ith input queue
			job=(sched->input_queues[i]->dequeue)(sched->input_queues[i]->queue);

				if(sched->num_output_queues>0){
					//we enqueue the job on the chosen output queue
					(sched->output_queues[output_index]->enqueue)(sched->output_queues[output_index]->queue,sched->scheduler_timestamp,job);
				} else {
					//we add it to the job array if we have no output queues configured
					jobs[job_index]=job;
				}
				//we increment the timestamp of the scheduler since we have scheduled one more job
				sched->scheduler_timestamp++;
		}
	}
	return jobs;
}

/**
 * Schedule a new event in the first appropriate queue which has free space.
 */
int schedule_in(priority_scheduler* sched, job_info* job){
	int i, done=SCHEDULE_FAIL;
	//we find a suitable queue
	for(i=0;i<sched->num_input_queues && !done;i++){
		if(sched->input_queues[i]->type==job->type && check_queues(sched->input_queues[i])){
			//the queue has the same type of the event and is not full so we can add the job in it
			(sched->input_queues[i]->enqueue)(sched->input_queues[i]->queue,job->deadline,job);
			done=SCHEDULE_DONE;
		}
	}
	return done;
}
