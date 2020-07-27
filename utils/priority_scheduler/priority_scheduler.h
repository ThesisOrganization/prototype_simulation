#pragma once
/** \file priority_scheduler.h
 * \brief APIs for the usage of a priority scheduler.
 * This header file contains the structures and the methods of a simple priority scheduler, which will take k events
 * from n input queues that have different priorities and schedule them on m output queues.
 */

/// This macro specifies that jobs from the ith input queue can be placed inside the jth queue when j < i.
#define UPGRADE_PRIO 1

/// \todo check if an enum is a better solution to handle the type of queue.

/// Real time queue type: contains jobs that must be processed strictly before the deadline.
#define QUEUE_RT 0

/// Lossy queue type: contains jobs that are discarded if the deadline is missed.
#define QUEUE_LOSSY 1

/// Batch queue type: contains jobs that must be processed eventually
#define QUEUE_BATCH 2

/** \brief How a struct it must be handled by the scheduler.
 * The `enqueue` and `dequeue` function pointers determine how elements must be added or removed from the queue.
 * The `type` parameter is used to determine the type of the queue.
 */
typedef struct queue_conf{
	void* queue; ///< The queue object
	int type; ///< The queue type, see `::QUEUE_BATCH`, `::QUEUE_LOSSY`, `::QUEUE_RT`.
	void (*enqueue)(void*,int,void*); ///< The enqueue operation used for this queue.
	void* (*dequeue)(void*); ///< The dequeue operation used for this queue.
	int (*check_presence)(void*); ///< Check is the queue is empty.
	int (*check_full)(void*); ///< Check if the queue is full, this function can be NULL if the queue is infinite.
} queue_conf;

/** \brief The object that represents the scheduler.
 * The scheduler is composed by a set of input queues and a set of output queues, each set with its own priority,
 * defined by the index of the element in the array. (Lowest -> Max Priority).
*/
typedef struct priority_scheduler{
	queue_conf** input_queues;	///< Array that holds the input queues.
	queue_conf** output_queues;	///< Array that holds the output queues.
	int num_input_queues;	///< Number of input queues
	int num_output_queues;	///< Number of output queues
	int events_to_schedule;	///< Number to events to schedule at each call.
																		///< If this number is 0 we move events from the input queues to the output queues
																		///< until the input queues are all empty or the output queues are all full.
	int mix_prio;	///< This parameter tells the scheduler if the events can change priority when are scheduled with more
								///< than one output queue. See `::UPGRADE_PRIO`.
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
 */
void schedule(priority_scheduler* sched);
