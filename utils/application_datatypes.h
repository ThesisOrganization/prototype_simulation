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
		void* payload; ///< The actual job data.
    //the following may be included in the payload:
    job_type job_type;
    int lp_sender; //used for the reply
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

typedef struct _node_topology{
  int node_type;
  int scheduler;
  int disk_type;
  double * diskServices;
  int * aggregation_rate;
  float delay_upper_router;
  float delay_lower_router;
  double * service_time;
  float probCommandResponse;
  int id_WAN_up;
  int id_WAN_down;
  int numberOfBelowActuators;
  int * actuatorsTypesBelow;
  int numberOfBelowSensors;
  int * sensorsTypesBelow;
  int * ListSensorsByType;
  int * ListActuatorsByType; //[x][y] element è il y-esimo elemento nella lista di attuatori di tipo x
} node_topology;

typedef struct _sensor_topology{
  int type_job;
  int sensor_type;
  int measure_type;
  int id_LAN_up;
  double * sensorRates;
} sensor_topology;

typedef struct _actuator_topology{
  int type_job;
  int actuator_type;
  double rateTransition;
  double serviceTimeCommand;
  int measure_type;
  int id_LAN_up;
} actuator_topology;

typedef struct _wan_topology{
  int wan_type;
  float delay;
  int numberOfBelowActuators;
  int * actuatorsTypesBelow;
  int numberOfBelowSensors;
  int * sensorsTypesBelow;
  int * ListSensorsByType;
  int * ListActuatorsByType; //[x][y] element è il y-esimo elemento nella lista di attuatori di tipo x

} wan_topology;

typedef struct _lan_topology{
  int lan_type;
  float delay;
  double * LANsINserviceTimes;
  double * LANsOUTserviceTimes;
  int numberOfBelowActuators;
  int * actuatorsTypesBelow;
  int numberOfBelowSensors;
  int * sensorsTypesBelow;
  int * ListSensorsByType;
  int * ListActuatorsByType; //[x][y] element è il y-esimo elemento nella lista di attuatori di tipo x

} lan_topology;

//struct topology node specific
typedef union {
    sensor_topology * sensor;
    node_topology * node;
    actuator_topology * actuator;
    lan_topology * lan;
    wan_topology * wan;
} specific_topology;


typedef struct _general_topology{
  int total_nodes;
  int sensor_nodes;
  int actuator_nodes;
  int numberOfTotalLANs;
  int numberOfTotalWANs;
  int numberOfActTypes;
  int numberOfSensTypes;
  int numberOfLANsTypes;
  double * probOfActuators;
} general_topology;

typedef struct _Element_topology{
  int lp_type;
  int upperNode;
  int numberOfLowerElements;
  int * lowerElements;
  int numberOfLANS;
  int * connectedLans;
  int * actuatorPaths;
  specific_topology spec_top;
} Element_topology;

typedef struct _total_topology{
  general_topology * gn;
  Element_topology ** lpt;
} total_topology;



//#############################################
//APPLICATION DATA
//#############################################


#define TOTAL_NUMBER_OF_EVENTS 100
#define PROB_CMD 0.4
//#define DELAY_MEAN 1
//#define ARRIVE_RATE 50
//#define FINISH_RATE 5
//#define LEN_QUEUE 50

//#define RANGE_TIMESTAMP 10

//#define NUM_QUEUES 3

typedef enum { //INIT should be 0
    ARRIVE = 1,
    FINISH,
    GENERATE_TRANSITION,
    GENERATE_TELEMETRY
} events_type;


typedef struct _queue_state {
    simtime_t start_processing_timestamp;
    job_info * current_job;
    int num_jobs_in_queue;
    //METRICS, from slide 4, single queue modeling, CP
    int * A; //number of jobs arrived (arrivals)
    int * C; //number of jobs completed (completions)
    double * B; //busy time, time in which the queue is computing
    double * W; //time spent in the system by all arrivals
    /*
    int * num_jobs_arrived;
    //int num_lossy_jobs_rejected;
    //int num_rt_jobs_rejected;
    //int num_batch_jobs_rejected;
    double * last_arrived_in_node_timestamp;
    double * sum_all_service_time;
    double * sum_all_time_between_arrivals;
    double * sum_all_response_time;
    */
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
    queue_state * disk_queue;
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
	simtime_t start_timestamp; //usefull to compute T
    simtime_t actual_timestamp;
    state_type type;
    //general infos
    int num_acts_types;
    double * prob_actuators;
    //specific infos
    state_info info;
} lp_state;

/*
typedef struct _processing_info {
    double start_processing_timestamp;
} processing_info;
*/

#endif /* APPLICATION_DATATYPES_H */
