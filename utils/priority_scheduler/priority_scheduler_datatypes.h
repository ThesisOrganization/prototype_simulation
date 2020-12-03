#ifndef PRIORITY_SCHEDULER_DATATYPES_H
#define PRIORITY_SCHEDULER_DATATYPES_H

//#############################################
//SCHEDULER DATA
//#############################################

/// Type of priority (Lower -> higher priority)
typedef enum _prio_type {
	REAL_TIME = 0, ///< Real time events
	LOSSY, ///< Events with a non strict deadline (meaningless after the deadline has expired)
	BATCH, ///< Events with no deadline
	NUM_OF_PRIO_TYPES ///< this must be at the end of the enum because denote the number of types
} prio_type;

typedef enum _job_type {
	INVALID_JOB = -1,
	TELEMETRY = 0,
	TRANSITION,
	COMMAND,
	BATCH_DATA,
	REPLY,
	NUM_OF_JOB_TYPE //IT'S NOT A TYPE, this must be at the end
} job_type;

/// Metadata used to charhterize a job type and priority.
typedef struct _job_info {
	prio_type prio; ///< The type of job.
	double arrived_in_node_timestamp;
	double deadline; ///< The deadline at which the job must be completed.
	//void* payload; ///< The actual job data. (can't be sent on rootsim)
	job_type job_type;
	int device_sender; //used for the reply
	double busy_time_transition; //used with the reply and transition
	double waiting_time_transition; //used with the reply and transition
	int device_destination; //used for command
} job_info;

/** \brief How a queue must be handled by the scheduler.
 * The `enqueue` and `dequeue` function pointers determine how elements must be added or removed from the queue.
 * The `type` parameter is used to determine the type of the queue.
 */
typedef struct queue_conf{
	void* queue; ///< The queue object.
	prio_type prio; ///< The queue priority, see `::QUEUE_BATCH`, `::QUEUE_LOSSY`, `::QUEUE_RT`.
	job_type type; ///< The queue type if is ::INVALID_JOB than the queue accepts jobs on any type;
	void (*enqueue)(void*,double,job_info); ///< The enqueue operation used for this queue.
	job_info (*dequeue)(void*); ///< The dequeue operation used for this queue.
	int (*check_presence)(void*); ///< Check is the queue is empty.
	int (*check_full)(void*); ///< Check if the queue is full, this function can be NULL if the queue is infinite.
	double (*peek)(void*); ///< returns the timestamp of the first element in the queue.
} queue_conf;

#endif
