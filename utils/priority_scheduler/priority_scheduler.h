#ifndef PRIORITY_SCHEDULER_H
#define PRIORITY_SCHEDULER_H
/// We failed to schedule a job
#define SCHEDULE_FAIL -1
/// We have scheduled a job successfully
#define SCHEDULE_DONE 0

/** \file priority_scheduler.h
 * \brief APIs for the usage of a priority scheduler.
 * This header file contains the structures and the methods of a simple priority scheduler, which will take k events
 * from n input queues that have different priorities and schedule them on m output queues.
 */

#include "priority_scheduler_datatypes.h"

/// This macro specifies that jobs can have their priority upgraded.
#define UPGRADE_PRIO 1

///\brief This enum represents the available scheduling algorithms.
typedef enum _scheduling_algorithm{
	SCHED_PRIO_FIFO=0, ///< Fist In First Out scheduling according to the priority and type of the job
	SCHED_RR ///< Round Robin scheduling between the differen priorities (preempted job are inserted at the end of the queue)
} scheduling_algorithm;

/** \brief The object that represents the scheduler.
 * The scheduler is composed by a set of input queues and a set of output queues, each set with its own priority,
 * defined by the index of the element in the array. (Lowest -> Max Priority).
 * The scheduler can also be configured to have 0 output queues, and it will return the events t be scheduled to the
 * caller.
 * __Note:__ Queues are required use the `::job_info` struct to charachterize the priority and timestamp of single jobs.
*/
typedef struct _priority_scheduler{
	queue_conf** input_queues;	///< Array that holds the input queues.
	queue_conf** output_queues;	///< Array that holds the output queues.
	int num_input_queues;	///< Number of input queues
	int num_output_queues;	///< Number of output queues
	int events_to_schedule;	///< Number to events to schedule at each call.
																		///< If this number is 0 we move events from the input queues to the output queues
																		///< until the input queues are all empty or the output queues are all full.
																		///< If there are no output queues this parameter cannot be 0.
	int mix_prio; ///< If the priority of an event can be upgraded in output set this to ::UPGRADE_PRIO, otherwise set to 0.
	scheduling_algorithm sched_algo;	///< This parameter tells the scheduler algorithm to use when scheduling the out jobs.
								///< than one output queue. See `::UPGRADE_PRIO`.
	int scheduler_timestamp;	///< The scheduler timestamp, used to schedule events in the output queues instead of the
														///< real timestamp so the order of events in and between input queues is respected in the
														///< output queues.
	job_type last_schedule_out;
} priority_scheduler;

/** \brief Creates and returns a new priority_scheduler.
 * \param[in] input Array that holds the input queues.
 * \param[in] output Array that holds the output queues.
 * \param[in] num_input Number of input queues.
 * \param[in] num_output Number of output queues.
 * \param[in] events Number of events to be processes at each call.
 * \param[in] prio If the priority of events in output can be upgraded.
 * \param[in] algo priority handling algorithm.
 * \returns A configured priority scheduler.
 */
priority_scheduler* new_prio_scheduler(queue_conf** input, queue_conf** output, int num_input, int num_output, unsigned int events, int prio,scheduling_algorithm algo);

/** \brief Schedules events from the input queues to the output queues.
 * \param[in] sched The scheduler which must be used to schedule events.
 * \param[in] jobs The arrat where the jobs scheduled out will returned if there are no output queues.
 * \param[in] num_jobs The length of the jobs array that will be filled.
 * \returns `NULL` if there is at least one output queue, an array of `events_to_schedule` elements if there are no output queues. The returned array can also have less than `events_to_schedule` elements depending on the input queue status, its length will be given by the num_jobs parameters.
 */
int schedule_out(priority_scheduler* sched,job_info* jobs,int num_jobs);

/** \brief Schedules a job to one of the input queues.
 * \param[in] sched The scheduler which must be used to schedule events.
 * \param[in] job The event to be inserted in an input queue.
 * \returns ::SCHEDULE_FAIL if the job has been rejected or ::SCHEDULE_DONE otherwise.
 */
int schedule_in(priority_scheduler* sched,job_info job);
#endif
