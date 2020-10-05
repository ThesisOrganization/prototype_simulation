#include "simulation_functions.h"

queue_conf** create_new_queues(int num_queues){


    queue_conf** queues = malloc(sizeof(queue_conf *) * num_queues);
    for(int i = 0; i < num_queues; i++){
        queues[i] = malloc(sizeof(queue_conf));
        queues[i]->queue = create_queue();
        queues[i]->type = i;
        queues[i]->enqueue = enqueue;
        queues[i]->dequeue = dequeue;
        queues[i]->check_presence = check_presence;
        queues[i]->check_full = NULL;
    }

    return queues;

}

void init_node(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){
    printf("node\n");

    state->info.node = malloc(sizeof(node_state));
    state->info.node->queue_state = malloc(sizeof(queue_state));
    state->info.node->queue_state->current_job = NULL;
    state->info.node->queue_state->num_jobs_in_queue = 0;
    state->info.node->queue_state->num_jobs_arrived = 0;
    state->info.node->queue_state->last_arrived_in_node_timestamp = 0.0;
    state->info.node->queue_state->sum_all_service_time = 0.0;
    state->info.node->queue_state->sum_all_time_between_arrivals = 0.0;
    state->info.node->queue_state->sum_all_response_time = 0.0;
    int num_queues = 1;
    state->info.node->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);



}

void init_sensor(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("sensor\n");
    
    state->info.sensor = malloc(sizeof(sensor_state));


    //schedule generate for all sensors
    simtime_t ts_generate = now + Expent(ARRIVE_RATE);
    ScheduleNewEvent(me, ts_generate, GENERATE_TRANSITION, NULL, 0);
    ts_generate = now + Expent(ARRIVE_RATE);
    ScheduleNewEvent(me, ts_generate, GENERATE_TELEMETRY, NULL, 0);
}

void init_actuator(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("actuator\n");

    state->info.actuator = malloc(sizeof(node_state));
    state->info.actuator->queue_state = malloc(sizeof(queue_state));
    state->info.actuator->queue_state->current_job = NULL;
    state->info.actuator->queue_state->num_jobs_in_queue = 0;
    state->info.actuator->queue_state->num_jobs_arrived = 0;
    state->info.actuator->queue_state->last_arrived_in_node_timestamp = 0.0;
    state->info.actuator->queue_state->sum_all_service_time = 0.0;
    state->info.actuator->queue_state->sum_all_time_between_arrivals = 0.0;
    state->info.actuator->queue_state->sum_all_response_time = 0.0;
    int num_queues = 1;
    state->info.actuator->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);
}

void init_lan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("lan\n");

    state->info.lan = malloc(sizeof(node_state));
    state->info.lan->queue_state = malloc(sizeof(queue_state));
    state->info.lan->queue_state->current_job = NULL;
    state->info.lan->queue_state->num_jobs_in_queue = 0;
    state->info.lan->queue_state->num_jobs_arrived = 0;
    state->info.lan->queue_state->last_arrived_in_node_timestamp = 0.0;
    state->info.lan->queue_state->sum_all_service_time = 0.0;
    state->info.lan->queue_state->sum_all_time_between_arrivals = 0.0;
    state->info.lan->queue_state->sum_all_response_time = 0.0;
    int num_queues = 1;
    state->info.lan->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);
}

void init_wan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("wan\n");

    state->num_jobs_processed = TOTAL_NUMBER_OF_EVENTS + 1;
}


static void start_node(unsigned int me, simtime_t now, lp_state * state, job_info * info){

}

static void start_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info){

}

static void start_lan(unsigned int me, simtime_t now, lp_state * state, job_info * info){

    state->info.lan->queue_state->current_job = info;
    state->info.lan->queue_state->start_processing_timestamp = now;

    simtime_t ts_finish = now + Expent(FINISH_RATE);
    ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);

}

void arrive_node(unsigned int me, simtime_t now, lp_state * state, job_info * info){

}

void arrive_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info){

}

void arrive_lan(unsigned int me, simtime_t now, lp_state * state, job_info* info){

    schedule_in(state->info.lan->queue_state->queues, info);

    state->info.lan->queue_state->num_jobs_in_queue++;
    state->info.lan->queue_state->num_jobs_arrived++;
    state->info.lan->queue_state->sum_all_time_between_arrivals += now - state->info.lan->queue_state->last_arrived_in_node_timestamp;

    if(state->info.lan->queue_state->current_job == NULL)
        start_lan(me, now, state, info);

}

