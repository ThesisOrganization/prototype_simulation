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
    void * queues;
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

/*

typedef struct _state_type {
    //int executed_events;
    int *queue;
    int index_insert;
    int index_remove;
    int num_clients;
    //info for statistics and termination
    int tot_num_clients;
    int sum_all_queue_number;
    int num_of_sums;
    int max_queue_len;
} state_type;
*/


#endif /* APPLICATION_H */
