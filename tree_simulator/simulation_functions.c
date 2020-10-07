#include "simulation_functions.h"

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


void init_node(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){
    printf("node\n");

    state->info.node = malloc(sizeof(node_state));
    state->info.node->queue_state = malloc(sizeof(queue_state));
    state->info.node->queue_state->current_job = NULL;
    state->info.node->queue_state->num_jobs_in_queue = 0;

    init_metrics(state->info.node->queue_state);
    //state->info.node->queue_state->num_jobs_arrived = 0;
    //state->info.node->queue_state->last_arrived_in_node_timestamp = 0.0;
    //state->info.node->queue_state->sum_all_service_time = 0.0;
    //state->info.node->queue_state->sum_all_time_between_arrivals = 0.0;
    //state->info.node->queue_state->sum_all_response_time = 0.0;
    int num_queues = 1;
    state->info.node->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);

    state->info.node->service_rates = getServiceRates(state->topology, me);

    state->info.node->type = infos->node_type;

    state->info.node->aggregation = infos->aggregation_rate;
    state->info.node->num_telemetry_aggregated = 0;

}

void init_sensor(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("sensor\n");
    
    state->info.sensor = malloc(sizeof(sensor_state));
    int sensor_type = infos->sensor_type;
    //printf("%d\n", sensor_type);
    double * sensor_rate = getSensorRatesForOneSensorType(state->topology, sensor_type);
    double rate_transition = sensor_rate[0];
    double rate_telemetry = sensor_rate[1];
    //double ** rates = getSensorRatesByType(state->topology);
    //double rate_transition = rates[sensor_type][0];
    //double rate_telemetry = rates[sensor_type][1];
    state->info.sensor->rate_transition = rate_transition;
    state->info.sensor->rate_telemetry = rate_telemetry;

    //schedule generate for all sensors
    simtime_t ts_generate = now + Expent(rate_transition);
    ScheduleNewEvent(me, ts_generate, GENERATE_TRANSITION, NULL, 0);
    ts_generate = now + Expent(rate_telemetry);
    ScheduleNewEvent(me, ts_generate, GENERATE_TELEMETRY, NULL, 0);
}

void init_actuator(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("actuator\n");

    state->info.actuator = malloc(sizeof(node_state));
    state->info.actuator->queue_state = malloc(sizeof(queue_state));
    state->info.actuator->queue_state->current_job = NULL;
    state->info.actuator->queue_state->num_jobs_in_queue = 0;
    
    init_metrics(state->info.actuator->queue_state);
    //state->info.actuator->queue_state->num_jobs_arrived = 0;
    //state->info.actuator->queue_state->last_arrived_in_node_timestamp = 0.0;
    //state->info.actuator->queue_state->sum_all_service_time = 0.0;
    //state->info.actuator->queue_state->sum_all_time_between_arrivals = 0.0;
    //state->info.actuator->queue_state->sum_all_response_time = 0.0;
    int num_queues = 1;
    state->info.actuator->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);
    
    state->info.actuator->service_rate_command = infos->serviceTimeCommand; 

    double rate_transition = infos->rateTransition;
    state->info.actuator->rate_transition = rate_transition;
    
    //schedule generate for all actuators
    simtime_t ts_generate = now + Expent(rate_transition);
    ScheduleNewEvent(me, ts_generate, GENERATE_TRANSITION, NULL, 0);
}

void init_lan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("lan\n");

    state->info.lan = malloc(sizeof(node_state));
    state->info.lan->queue_state = malloc(sizeof(queue_state));
    state->info.lan->queue_state->current_job = NULL;
    state->info.lan->queue_state->num_jobs_in_queue = 0;
    
    init_metrics(state->info.lan->queue_state);
    //state->info.lan->queue_state->num_jobs_arrived = 0;
    //state->info.lan->queue_state->last_arrived_in_node_timestamp = 0.0;
    //state->info.lan->queue_state->sum_all_service_time = 0.0;
    //state->info.lan->queue_state->sum_all_time_between_arrivals = 0.0;
    //state->info.lan->queue_state->sum_all_response_time = 0.0;
    int num_queues = 1;
    state->info.lan->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);
    

    int lan_type = infos->lan_type;
    state->info.lan->service_rates = getLANServiceTimesForOneLANType(state->topology, lan_type);

}

void init_wan(unsigned int me, simtime_t now, lp_state * state, lp_infos * infos){

    printf("wan\n");

    state->num_jobs_processed = TOTAL_NUMBER_OF_EVENTS + 1;
}


static void start_node(unsigned int me, simtime_t now, lp_state * state, job_info * info){

    state->info.node->queue_state->current_job = info;
    state->info.node->queue_state->start_processing_timestamp = now;
    
    double rate = state->info.node->service_rates[info->job_type];
    simtime_t ts_finish = now + Expent(rate);
    ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);

}

static void start_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info){

    state->info.actuator->queue_state->current_job = info;
    state->info.actuator->queue_state->start_processing_timestamp = now;
    
    double rate = state->info.actuator->service_rate_command;
    simtime_t ts_finish = now + Expent(rate);
    ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);
}

static void start_lan(unsigned int me, simtime_t now, lp_state * state, job_info * info){

    state->info.lan->queue_state->current_job = info;
    state->info.lan->queue_state->start_processing_timestamp = now;

    double rate = state->info.lan->service_rates[info->job_type];
    simtime_t ts_finish = now + Expent(rate);
    ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);

}

void arrive_node(unsigned int me, simtime_t now, lp_state * state, job_info * info){
    //printf("NODE\n");

    state->info.node->queue_state->num_jobs_in_queue++;

    state->info.node->queue_state->A[info->job_type]++;
    //state->info.node->queue_state->num_jobs_arrived++;
    //state->info.node->queue_state->sum_all_time_between_arrivals += now - state->info.lan->queue_state->last_arrived_in_node_timestamp;
    //state->info.node->queue_state->last_arrived_in_node_timestamp = now;

    if(state->info.node->queue_state->current_job == NULL)
        start_node(me, now, state, info);
    else
        schedule_in(state->info.node->queue_state->queues, info);

}

void arrive_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info){

    state->info.actuator->queue_state->num_jobs_in_queue++;
    
    state->info.actuator->queue_state->A[info->job_type]++;
    //state->info.actuator->queue_state->num_jobs_arrived++;
    //state->info.actuator->queue_state->sum_all_time_between_arrivals += now - state->info.lan->queue_state->last_arrived_in_node_timestamp;
    //state->info.actuator->queue_state->last_arrived_in_node_timestamp = now;

    if(state->info.actuator->queue_state->current_job == NULL)
        start_actuator(me, now, state, info);
    else
        schedule_in(state->info.actuator->queue_state->queues, info);
}

void arrive_lan(unsigned int me, simtime_t now, lp_state * state, job_info* info){

    state->info.lan->queue_state->num_jobs_in_queue++;
    
    state->info.lan->queue_state->A[info->job_type]++;
    //state->info.lan->queue_state->num_jobs_arrived++;
    //state->info.lan->queue_state->sum_all_time_between_arrivals += now - state->info.lan->queue_state->last_arrived_in_node_timestamp;
    //state->info.lan->queue_state->last_arrived_in_node_timestamp = now;

    if(state->info.lan->queue_state->current_job == NULL)
        start_lan(me, now, state, info);
    else
        schedule_in(state->info.lan->queue_state->queues, info);

}

static void send_reply(unsigned int me, simtime_t now, lp_state * state, job_info * info){

    job_info info_to_send;
    info_to_send.type = REAL_TIME;
    info_to_send.payload = NULL;
    info_to_send.job_type = REPLY;
    info_to_send.sender = me; //not necessarly
    int sender = info->sender;

    //printf("%d\n", state->info.node->type);

    ScheduleNewEvent(sender, now, ARRIVE, &info_to_send, sizeof(job_info));

}

void finish_node(unsigned int me, simtime_t now, lp_state * state){

    job_info * info = state->info.node->queue_state->current_job;

    job_info ** info_arr = schedule_out(state->info.node->queue_state->queues);
    state->info.node->queue_state->current_job = info_arr[0];
    
    double rate = state->info.node->service_rates[info->job_type];
    simtime_t ts_finish = now + Expent(rate);
    if(state->info.node->queue_state->current_job != NULL){
        state->info.node->queue_state->start_processing_timestamp = now;
        ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);
    }
    
    state->info.node->queue_state->num_jobs_in_queue--;
    
    state->info.node->queue_state->C[info->job_type]++;
    state->info.node->queue_state->B[info->job_type] += now - state->info.node->queue_state->start_processing_timestamp;
    state->info.node->queue_state->W[info->job_type] += now - info->arrived_in_node_timestamp;
    //double service_time = now - (state->info.node->queue_state->start_processing_timestamp);
    //double response_time = now - info->arrived_in_node_timestamp;

    //state->info.node->queue_state->sum_all_service_time += service_time;
    //state->info.node->queue_state->sum_all_response_time += response_time;

    int up_node;

    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        
        int actual_aggr = state->info.node->num_telemetry_aggregated++;
        //printf("%d, %d\n", me, actual_aggr);

        if(actual_aggr >= state->info.node->aggregation){
            
            //send aggregated data
            up_node = getUpperNode(state->topology, me);
            if(up_node != -1)
                ScheduleNewEvent(up_node, now, ARRIVE, info, sizeof(job_info));
            
            //restart buffer of telemetry
            state->info.node->num_telemetry_aggregated = 0;
        }

    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");

        if(state->info.node->type != LOCAL)
            send_reply(me, now, state, info);

        //GENERATE COMMAND
        int * num_types = getActType(state->topology, me);
        
        int type = 0; //random
        int * list_actuators_by_type = getListActuatorsByType(state->topology, me, type);

        int selected_actuator = 0; //random
        int id_next_hop = list_actuators_by_type[selected_actuator];

        int * next_hop = getActuatorPathsIndex(state->topology, me);
        printf("%d\n", id_next_hop);

        int next_device = next_hop[id_next_hop];
        
        printf("GENERATING A COMMAND\n");
        job_info info_to_send;
        info_to_send.type = REAL_TIME;
        info_to_send.payload = NULL;
        info_to_send.job_type = COMMAND;

        ScheduleNewEvent(next_device, now, ARRIVE, &info_to_send, sizeof(job_info));

        //forward transition
        info->sender = me;
        up_node = getUpperNode(state->topology, me);
        if(up_node != -1)
            ScheduleNewEvent(up_node, now, ARRIVE, info, sizeof(job_info));
        

    }
    else if(info->job_type == COMMAND){

    }
    else if(info->job_type == BATCH_DATA){


    }
    else if(info->job_type == REPLY){

        //printf("REPLY RECEIVED\n");
        //printf("%f, %d, %d\n", now, me, info->sender);

    }

    free(info_arr); //liberi l'array dell'attuale job!
    free(info); //liberi il vecchio job
}
void finish_actuator(unsigned int me, simtime_t now, lp_state * state){

    job_info * info = state->info.actuator->queue_state->current_job;

    job_info ** info_arr = schedule_out(state->info.actuator->queue_state->queues);
    state->info.actuator->queue_state->current_job = info_arr[0];
    
    double rate = state->info.actuator->service_rate_command;
    simtime_t ts_finish = now + Expent(rate);
    if(state->info.actuator->queue_state->current_job != NULL){
        state->info.actuator->queue_state->start_processing_timestamp = now;
        ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);
    }
    
    state->info.actuator->queue_state->num_jobs_in_queue--;
    
    state->info.actuator->queue_state->C[info->job_type]++;
    state->info.actuator->queue_state->B[info->job_type] += now - state->info.actuator->queue_state->start_processing_timestamp;
    state->info.actuator->queue_state->W[info->job_type] += now - info->arrived_in_node_timestamp;
    //double service_time = now - (state->info.actuator->queue_state->start_processing_timestamp);
    //double response_time = now - info->arrived_in_node_timestamp;

    //state->info.actuator->queue_state->sum_all_service_time += service_time;
    //state->info.actuator->queue_state->sum_all_response_time += response_time;

    int up_node;

    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        printf("WARNING: actuator received a telemetry data\n");

    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");
        printf("WARNING: actuator received a transition data\n");

    }
    else if(info->job_type == COMMAND){

        printf("COMMAND received!!!!\n");

    }
    else if(info->job_type == BATCH_DATA){

        printf("WARNING: actuator received a batch data\n");

    }
    else if(info->job_type == REPLY){

        printf("WARNING: actuator received a reply data\n");
    }

    free(info_arr); //liberi l'array dell'attuale job!
    free(info); //liberi il vecchio job
}

void finish_lan(unsigned int me, simtime_t now, lp_state * state){

    job_info * info = state->info.lan->queue_state->current_job;

    job_info ** info_arr = schedule_out(state->info.lan->queue_state->queues);
    state->info.lan->queue_state->current_job = info_arr[0];
    
    double rate = state->info.lan->service_rates[info->job_type];
    simtime_t ts_finish = now + Expent(rate);
    if(state->info.lan->queue_state->current_job != NULL){
        state->info.lan->queue_state->start_processing_timestamp = now;
        ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);
    }
    
    state->info.lan->queue_state->num_jobs_in_queue--;
    
    state->info.lan->queue_state->C[info->job_type]++;
    state->info.lan->queue_state->B[info->job_type] += now - state->info.lan->queue_state->start_processing_timestamp;
    state->info.lan->queue_state->W[info->job_type] += now - info->arrived_in_node_timestamp;
    //double service_time = now - (state->info.lan->queue_state->start_processing_timestamp);
    //double response_time = now - info->arrived_in_node_timestamp;

    //state->info.lan->queue_state->sum_all_service_time += service_time;
    //state->info.lan->queue_state->sum_all_response_time += response_time;

    int up_node;

    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        up_node = getUpperNode(state->topology, me);
        ScheduleNewEvent(up_node, now, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");
        up_node = getUpperNode(state->topology, me);
        ScheduleNewEvent(up_node, now, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == COMMAND){

        int destination = info->destination; //you should use the function of the topology
        ScheduleNewEvent(destination, now, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == BATCH_DATA){

        printf("WARNING: lan received a batch data\n");

    }
    else if(info->job_type == REPLY){

        printf("WARNING: lan received a reply data\n");
    }

    free(info_arr); //liberi l'array dell'attuale job!
    free(info); //liberi il vecchio job

}
