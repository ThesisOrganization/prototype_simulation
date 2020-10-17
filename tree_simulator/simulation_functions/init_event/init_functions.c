#include "init_functions.h"


static queue_conf** create_new_queues(int num_queues){


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

static void init_metrics(queue_state * queue_state){

    queue_state->C = malloc(sizeof(int)*NUM_OF_JOB_TYPE);
    queue_state->A = malloc(sizeof(int)*NUM_OF_JOB_TYPE);
    queue_state->W = malloc(sizeof(double)*NUM_OF_JOB_TYPE);
    queue_state->B = malloc(sizeof(double)*NUM_OF_JOB_TYPE);

    for(int i=0; i < NUM_OF_JOB_TYPE; i++){

        queue_state->C[i] = 0;
        queue_state->A[i] = 0;
        queue_state->W[i] = 0.0;
        queue_state->B[i] = 0.0;

    }

}

void init_node(unsigned int me, lp_state * state){
    //printf("node\n");

    state->info.node = malloc(sizeof(node_state));
    state->info.node->queue_state = malloc(sizeof(queue_state));
    state->info.node->queue_state->current_job = NULL;
    state->info.node->queue_state->num_jobs_in_queue = 0;

    init_metrics(state->info.node->queue_state);

    int num_queues = 1;
    state->info.node->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);

    state->info.node->service_rates = GET_SERVICE_RATES(state->topology, me);

    //state->info.node->service_rates[TRANSITION] *= 2; //to delete

    state->info.node->type = GET_NODE_TYPE(state->topology, me);

    state->info.node->telemetry_aggregation = GET_AGGREGATION_RATE(state->topology, me)[TELEMETRY];
    state->info.node->num_telemetry_aggregated = 0;

    state->info.node->up_delay = GET_DELAY_UPPER_ROUTER(state->topology, me);
    state->info.node->down_delay = GET_DELAY_LOWER_ROUTER(state->topology, me);

    state->info.node->id_wan_down = GET_WAN_DOWN(state->topology, me);

    state->info.node->batch_aggregation = GET_AGGREGATION_RATE(state->topology, me)[BATCH];
    state->info.node->num_batch_aggregated = 0;

    //int node_type = state->info.node->type;
    //state->info.node->prob_cmd = state->topology->probNodeCommandArray[node_type];
    state->info.node->prob_cmd = GET_PROB_COMMAND(state->topology, me);

    /*
    ///init disk queue
    state->info.node->disk_queue = malloc(sizeof(queue_state));
    state->info.node->disk_queue->current_job = NULL;
    state->info.node->disk_queue->num_jobs_in_queue = 0;

    init_metrics(state->info.node->disk_queue);

    num_queues = 1;
    state->info.node->disk_queue->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);
*/
}

void init_sensor(unsigned int me, simtime_t now, lp_state * state){

    //printf("sensor\n");

    state->info.sensor = malloc(sizeof(sensor_state));
    int sensor_type = GET_SENSOR_TYPE(state->topology, me);
    //printf("%d\n", sensor_type);
    double * sensor_rate = GET_SENSOR_TYPE_RATES(state->topology, sensor_type);
    double rate_transition = sensor_rate[TRANSITION];
    double rate_telemetry = sensor_rate[TELEMETRY];

    state->info.sensor->rate_transition = rate_transition;
    state->info.sensor->rate_telemetry = rate_telemetry;

    //schedule generate for all sensors
    double time_between_arrivals = 1/rate_transition;
    simtime_t ts_generate = now + Expent(time_between_arrivals);
    ScheduleNewEvent(me, ts_generate, GENERATE_TRANSITION, NULL, 0);

    time_between_arrivals = 1/rate_telemetry;
    ts_generate = now + Expent(time_between_arrivals);
    ScheduleNewEvent(me, ts_generate, GENERATE_TELEMETRY, NULL, 0);

}

void init_actuator(unsigned int me, simtime_t now, lp_state * state){

    //printf("actuator\n");

    state->info.actuator = malloc(sizeof(actuator_state));
    state->info.actuator->queue_state = malloc(sizeof(queue_state));
    state->info.actuator->queue_state->current_job = NULL;
    state->info.actuator->queue_state->num_jobs_in_queue = 0;

    init_metrics(state->info.actuator->queue_state);

    int num_queues = 1;
    state->info.actuator->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);

    state->info.actuator->service_rate_command = GET_SERVICE_COMMAND(state->topology, me);

    double rate_transition = GET_RATE_TRANSITION(state->topology, me);
    state->info.actuator->rate_transition = rate_transition;

    //schedule generate for all actuators
    double time_between_arrivals = 1/rate_transition;
    simtime_t ts_generate = now + Expent(time_between_arrivals);
    ScheduleNewEvent(me, ts_generate, GENERATE_TRANSITION, NULL, 0);
}

void init_lan(unsigned int me, lp_state * state){

    //printf("lan\n");

    state->info.lan = malloc(sizeof(lan_state));
    state->info.lan->queue_state_in = malloc(sizeof(queue_state));
    state->info.lan->queue_state_out = malloc(sizeof(queue_state));

    state->info.lan->queue_state_in->current_job = NULL;
    state->info.lan->queue_state_out->current_job = NULL;

    state->info.lan->queue_state_in->num_jobs_in_queue = 0;
    state->info.lan->queue_state_out->num_jobs_in_queue = 0;

    init_metrics(state->info.lan->queue_state_in);
    init_metrics(state->info.lan->queue_state_out);

    int num_queues = 1;
    state->info.lan->queue_state_in->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);
    state->info.lan->queue_state_out->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);


    int lan_type = GET_LAN_TYPE(state->topology, me);
    state->info.lan->service_rates_in = GET_LAN_IN_TYPE_SERVICE(state->topology, lan_type);
    state->info.lan->service_rates_out = GET_LAN_OUT_TYPE_SERVICE(state->topology, lan_type);

}

void init_wan(unsigned int me, lp_state * state){

    //printf("wan\n");
    state->info.wan = malloc(sizeof(wan_state));

    state->info.wan->delay = GET_DELAY(state->topology, me);

}
