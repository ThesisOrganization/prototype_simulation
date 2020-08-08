#ifndef APPLICATION_DATATYPES_H
#define APPLICATION_DATATYPES_H

//#############################################
//SCHEDULER DATA
//#############################################

/** \file priority_dataypes.h
 * Some datatpyes that are useful when handling events and queues with priority.
 */

/// Type of priority (Lower -> higher priority)
typedef enum {
    REAL_TIME = 0, ///< Real time events
    LOSSY, ///< Events with a non strict deadline (meaningless after the deadline has expired)
    BATCH ///< Events with no deadline
    NUM_OF_PRIO_TYPES ///< this must be at the end of the enum because denote the number of types
} prio_type;

/// Metadata used to charhterize a job type and priority.
typedef struct _job_info {
    prio_type type; ///< The type of job.
    double arrived_in_node_timestamp;
    double deadline; ///< The deadline at which the job must be completed.
		void* payload; ///< The actual job data.
} job_info;

/** \brief How a queue must be handled by the scheduler.
 * The `enqueue` and `dequeue` function pointers determine how elements must be added or removed from the queue.
 * The `type` parameter is used to determine the type of the queue.
 */
typedef struct queue_conf{
	void* queue; ///< The queue object.
	prio_type type; ///< The queue type, see `::QUEUE_BATCH`, `::QUEUE_LOSSY`, `::QUEUE_RT`.
	void (*enqueue)(void*,double,void*); ///< The enqueue operation used for this queue.
	void* (*dequeue)(void*); ///< The dequeue operation used for this queue.
	int (*check_presence)(void*); ///< Check is the queue is empty.
	int (*check_full)(void*); ///< Check if the queue is full, this function can be NULL if the queue is infinite.
} queue_conf;

//#############################################
//PARTOP DATA
//#############################################

typedef enum {
    NODE = 0,
    SENSOR,
    ACTUATOR
} state_type;

typedef enum {
    SCHEDULER1 = 101,
    SCHEDULER2,
    SCHEDULER3
} scheduler_type;

typedef enum {
    SENSOR_TYPE0 = 301,
    SENSOR_TYPE1
} sensor_type;

typedef enum {
    ACTUATOR_TYPE0 = 401,
    ACTUATOR_TYPE1
} actuator_type;

typedef enum {
    CENTRAL = 501,
    REGIONAL,
    NETWORK
} node_type;

typedef enum {
    MEASURE0 = 601,
    MEASURE1,
    MEASURE2
} measure_type;

//temp struct, to make things uniform with application.c
//and check integrity in this main file
typedef struct _parsingStruct
{
int lp_type;
int type_job;
int scheduler;
double response_time;
int node_type;
int sensor_type;
int actuator_type;
int measure_type;
} parsingStruct;

typedef struct _topArray
{
int numberOfReceivers;
int * receiver;
void * info;
} topArray;

typedef struct _topology{
  int total_nodes;
  int sensor_nodes;
  topArray ** topArr; //array of poiters to topArray
} topology;


//#############################################
//APPLICATION DATA
//#############################################

#define TOTAL_NUMBER_OF_EVENTS 100
#define DELAY_MEAN 1
#define ARRIVE_RATE 30
#define FINISH_RATE 5
#define LEN_QUEUE 50

#define RANGE_TIMESTAMP 10

//#define NUM_QUEUES 3

typedef enum { //INIT should be 0
    ARRIVE = 1
    FINISH,
    GENERATE
} events_type;

typedef struct _sensor_state {
    prio_type job_generated;
} sensor_state;

typedef struct _node_state {
    int num_jobs_in_queue;
    int num_jobs_arrived;
    double last_arrived_in_node_timestamp;
    double sum_all_service_time;
    double sum_all_time_between_arrivals;
    double sum_all_response_time;
    void * queues;
} node_state;

typedef union {
    sensor_state * sensor;
    node_state * node;
} state_info;

typedef struct _state {
    int num_jobs_processed;
	simtime_t ts;
    state_type type;
    topology * topology;
    state_info info;
} lp_state;

typedef struct _processing_info {
    double start_processing_timestamp;
} processing_info;


#endif /* APPLICATION_DATATYPES_H */
