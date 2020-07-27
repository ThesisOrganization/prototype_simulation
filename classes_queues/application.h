#ifndef APPLICATION_H
#define APPLICATION_H


typedef enum {
    REAL_TIME = 0,
    DISCARD,
    BATCH
} job_type;

typedef struct _job_info {
    job_type type;
    int timestamp;
} job_info;

typedef struct _sensor_state {
    job_type job_generated;
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
    state_info info;
} state;

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
