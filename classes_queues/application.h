#ifndef APPLICATION_H
#define APPLICATION_H


#include "../utils/priority_scheduler/priority_datatypes.h"
#include "../utils/partop/header.h"

typedef enum {
    NODE = 0,
    SENSOR
} state_type;

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
		simtime_t ts;
} node_state;

typedef union {
    sensor_state * sensor;
    node_state * node;
} state_info;

typedef struct _state {
    int num_jobs_processed;
    state_type type;
    topology * topology;
    state_info info;
} lp_state;

typedef struct _lp_infos {
    state_type lp_type;
    prio_type type_job;
} lp_infos;

typedef struct _processing_info {
    double start_processing_timestamp;
} processing_info;


#endif /* APPLICATION_H */
