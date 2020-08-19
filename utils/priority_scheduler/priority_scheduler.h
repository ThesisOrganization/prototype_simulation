#pragma once

/// We failed to schedule in a job
#define SCHEDULE_FAIL 0
/// We have scheduled a job in successfully
#define SCHEDULE_DONE 1

/** \file priority_scheduler.h
 * \brief APIs for the usage of a priority scheduler.
 * This header file contains the structures and the methods of a simple priority scheduler, which will take k events
 * from n input queues that have different priorities and schedule them on m output queues.
 */

#include "../application_datatypes.h"

/// This macro specifies that jobs from the ith input queue can be placed inside the jth queue when j < i.
#define UPGRADE_PRIO 1

/** \brief The object that represents the scheduler.
 * The scheduler is composed by a set of input queues and a set of output queues, each set with its own priority,
 * defined by the index of the element in the array. (Lowest -> Max Priority).
 * The scheduler can also be configured to have 0 output queues, and it will return the events t be scheduled to the
 * caller.
 * __Note:__ Queues are required use the `::job_info` struct to charachterize the priority and timestamp of single jobs.
*/
typedef struct priority_scheduler{
	queue_conf** input_queues;	///< Array that holds the input queues.
	queue_conf** output_queues;	///< Array that holds the output queues.
	int num_input_queues;	///< Number of input queues
	int num_output_queues;	///< Number of output queues
	int events_to_schedule;	///< Number to events to schedule at each call.
																		///< If this number is 0 we move events from the input queues to the output queues
																		///< until the input queues are all empty or the output queues are all full.
																		///< If there are no output queues this parameter cannot be 0.
	int mix_prio;	///< This parameter tells the scheduler if the events can change priority when are scheduled with more
								///< than one output queue. See `::UPGRADE_PRIO`.
	int scheduler_timestamp;	///< The scheduler timestamp, used to schedule events in the output queues instead of the
														///< real timestamp so the order of events in and between input queues is respected in the
														///< output queues.
} priority_scheduler;

/** \brief Creates and returns a new priority_scheduler.
 * \param[in] input Array that holds the input queues.
 * \param[in] output Array that holds the output queues.
 * \param[in] num_input Number of input queues.
 * \param[in] num_output Number of output queues.
 * \param[in] events Number of events to be processes at each call.
 * \param[in] prio priority handling scheme.
 * \returns A configured priority scheduler.
 */
priority_scheduler* new_prio_scheduler(queue_conf** input, queue_conf** output, int num_input, int num_output, unsigned int events, int prio);

/** \brief Schedules events from the input queues to the output queues.
 * \param[in] sched The scheduler which must be used to schedule events.
 * \returns `NULL` if there is at least one output queue, an array of `events_to_schedule` elements if there are no output queues. The returned array can also have less than `events_to_schedule` elements depending on the input queue status, its last element is always NULL.
 */
job_info** schedule_out(priority_scheduler* sched);

/** \brief Schedules a job to one of the input queues.
 * \param[in] sched The scheduler which must be used to schedule events.
 * \param[in] job The event to be inserted in an input queue.
 * \returns ::SCHEDULE_FAIL if the job has been rejected or ::SCHEDULE_DONE otherwise.
 */
int schedule_in(priority_scheduler* sched,job_info* job);
