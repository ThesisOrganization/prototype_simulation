#ifndef APPLICATION_DATATYPES_H
#define APPLICATION_DATATYPES_H

#include <ROOT-Sim.h>
//#############################################
//SCHEDULER DATA
//#############################################

/** \file application_datatypes.h
 * Some datatpyes that are neded when handling models, using the contantes of the utils folder.
 */

/// Type of priority (Lower -> higher priority)
typedef enum {
    REAL_TIME = 0, ///< Real time events
    LOSSY, ///< Events with a non strict deadline (meaningless after the deadline has expired)
    BATCH, ///< Events with no deadline
    NUM_OF_PRIO_TYPES ///< this must be at the end of the enum because denote the number of types
} prio_type;

typedef enum {
    TELEMETRY = 0,
    TRANSITION,
    COMMAND,
    BATCH_DATA,
    REPLY,
    NUM_OF_JOB_TYPE //IT'S NOT A TYPE, this must be at the end
} job_type;

/// Metadata used to charhterize a job type and priority.
typedef struct _job_info {
    prio_type type; ///< The type of job.
    double arrived_in_node_timestamp;
    double deadline; ///< The deadline at which the job must be completed.
		void* payload; ///< The actual job data. (can't be sent on rootsim)
    job_type job_type;
    int lp_sender; //used for the reply
    double busy_time_transition; //used with the reply and transition
    double waiting_time_transition; //used with the reply and transition
    int lp_destination; //used for command
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
    ACTUATOR,
    WAN,
    LAN
} state_type;

typedef enum {
    SCHEDULER1 = 0,
    SCHEDULER2,
    SCHEDULER3
} scheduler_type;

typedef enum {
    SENSOR_TYPE0 = 0,
    SENSOR_TYPE1
} sensor_type;

typedef enum {
    ACTUATOR_TYPE0 = 0,
    ACTUATOR_TYPE1
} actuator_type;

typedef enum {
    CENTRAL = 0,
    REGIONAL,
    LOCAL
} node_type;

typedef enum {
    MEASURE0 = 0,
    MEASURE1,
    MEASURE2
} measure_type;

typedef enum {
    WAN_TYPE0 = 0,
    WAN_TYPE1
} wan_type;

typedef enum {
    LAN_TYPE0 = 0,
    LAN_TYPE1,
    LAN_TYPE2
} lan_type;

typedef enum {
    RAID1 = 0,
    RAID2,
    RAID3
} disk_type;
//temp struct, to make things uniform with application.c
//and check integrity in this main file
typedef struct _lp_infos
{
int lp_type;

int sensor_type;
int actuator_type;
int type_job;
int measure_type;
int id_LAN_up;

double rateTransition;
double serviceTimeCommand;

int wan_type;
int lan_type;
float delay;

int node_type;
double * service_time;
int scheduler;
int numberOfBelowActuators;
int * actuatorsTypesBelow;
int id_WAN_up;
int id_WAN_down;
int numberOfBelowSensors;
int * sensorsTypesBelow;
int * aggregation_rate;
float delay_upper_router;
float delay_lower_router;
float probCommandResponse;
int disk_type;
double * diskServices;
} lp_infos;

typedef struct _topArray
{
int upperNode;
int numberOfLowerElements;
int * lowerElements;
int numberOfLANS;
int * connectedLans;
void * info;
} topArray;

typedef struct _topology{
  int total_nodes;
  int sensor_nodes;
  int actuator_nodes;
  int numberOfTotalLANs;
  int numberOfTotalWANs;
  int numberOfActTypes;
  int numberOfSensTypes;
  int numberOfLANsTypes;
  int ** actuatorPaths;
  double ** sensorRatesByType;
  double ** LANsINserviceTimes;
  double ** LANsOUTserviceTimes;
  double * probOfActuators;
  int *** ListSensorsByType;
  int *** ListActuatorsByType;
  topArray ** topArr; //array of poiters to topArray
} topology;


//#############################################
//APPLICATION DATA
//#############################################


//#define TOTAL_NUMBER_OF_EVENTS 100
//#define PROB_CMD 0.4
//#define DELAY_MEAN 1
//#define ARRIVE_RATE 50
//#define FINISH_RATE 5
//#define LEN_QUEUE 50
#define RANDOM_START 500
#define TRANSITION_TIME_LIMIT 300000
#define MAX_SIMULATION_TIME 800000

#define NEXT_UPDATE_TIMESTAMP 100000

//#define RANGE_TIMESTAMP 10

//#define NUM_QUEUES 3

typedef enum { //INIT should be 0
    ARRIVE = 1,
    ARRIVE_DISK,
    FINISH,
    FINISH_DISK,
    GENERATE_TRANSITION,
    GENERATE_TELEMETRY,
    UPDATE_TIMESTAMP
} events_type;


typedef struct _queue_state {
    simtime_t start_processing_timestamp;
    job_info * current_job;
    int num_jobs_in_queue;
    //METRICS, from slide 4, single queue modeling, CP
    //note that these values are computed with the values after the last finish event before the termination
    int * A; //number of jobs arrived (arrivals)
    int * A_post; //number of jobs arrived (arrivals), after the last finish event
    int * C; //number of jobs completed (completions)
    double * B; //busy time, time in which the queue is computing
    double * W; //time spent in the system by all arrivals
    simtime_t * actual_timestamp;
    simtime_t * start_timestamp;
    void * queues;
} queue_state;

typedef struct _sensor_state {
    prio_type job_generated;
    double rate_telemetry;
    double rate_transition;
} sensor_state;

typedef struct _node_state {
    queue_state * queue_state;
    double * service_rates;
    node_type type;
    int telemetry_aggregation;
    int num_telemetry_aggregated;
    double up_delay;
    double down_delay;
    int id_wan_down;
    double prob_cmd;
    int batch_aggregation;
    int num_batch_aggregated;
    queue_state * disk_state;
} node_state;

typedef struct _actuator_state {
    queue_state * queue_state;
    double rate_transition;
    double service_rate_command;
} actuator_state;


typedef enum { //INIT should be 0
    LAN_IN = 0,
    LAN_OUT
} lan_direction;

typedef struct _lan_state {
    queue_state * queue_state_in;
    queue_state * queue_state_out;
    double * service_rates_in;
    double * service_rates_out;
} lan_state;

typedef struct _wan_state {
    double delay;
} wan_state;

typedef union {
    sensor_state * sensor;
    node_state * node;
    actuator_state * actuator;
    lan_state * lan;
    wan_state * wan;
} state_info;

typedef struct _state {
    //int num_jobs_processed;
    int lp_enabled; //1 lp enabled, 0 lp disabled
	//simtime_t start_timestamp; //usefull to compute T
    //simtime_t actual_timestamp;
    simtime_t device_timestamp;
    state_type type;
    //general infos
    topology * topology;
    int num_acts_types;
    double * prob_actuators;
    //specific infos
    state_info info;
} lp_state;


#endif /* APPLICATION_DATATYPES_H */
