#include "arrive_functions.h"

static void start_device(unsigned int me, simtime_t now, queue_state * queue_state, double * service_rates, job_info * info, lan_direction * direction, int size_lan_direction, events_type event_to_trigger){

    if(queue_state->current_job == NULL){

        queue_state->current_job = info;
        queue_state->start_processing_timestamp = now;

        double rate = service_rates[info->job_type];
        simtime_t ts_finish = now + Expent(rate);
        ScheduleNewEvent(me, ts_finish, event_to_trigger, direction, size_lan_direction);

    }
    else
        schedule_in(queue_state->queues, info);


}

static void update_metrics(simtime_t now, queue_state * queue_state, job_info * info){

    queue_state->num_jobs_in_queue++;

    //queue_state->A[info->job_type]++;

    job_type type = info->job_type;

    if(queue_state->start_timestamp[type] <= TRANSITION_TIME_LIMIT && now > TRANSITION_TIME_LIMIT)
        queue_state->start_timestamp[type] = now;

    
    if(queue_state->start_timestamp[type] > TRANSITION_TIME_LIMIT){

        queue_state->A_post[type]++;

    }
}

void arrive_node(unsigned int me, simtime_t now, lp_state * state, job_info * info){
    //printf("NODE\n");
    update_metrics(now, state->info.node->queue_state, info);

    start_device(me, now, state->info.node->queue_state, state->info.node->service_rates, info, NULL, 0, FINISH);
}

void arrive_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info){
    update_metrics(now, state->info.actuator->queue_state, info);

    double service_rates[NUM_OF_JOB_TYPE]; //meh
    service_rates[COMMAND] = state->info.actuator->service_rate_command;
    start_device(me, now, state->info.actuator->queue_state, service_rates, info, NULL, 0, FINISH);
}

void arrive_lan(unsigned int me, simtime_t now, lp_state * state, job_info* info){

    queue_state * queue_state;
    double * service_rates;
    lan_direction direction;

    if(info->job_type == TELEMETRY){

        queue_state = state->info.lan->queue_state_out;
        service_rates = state->info.lan->service_rates_out;
        direction = LAN_OUT;

    }
    else if(info->job_type == TRANSITION){

        queue_state = state->info.lan->queue_state_out;
        service_rates = state->info.lan->service_rates_out;
        direction = LAN_OUT;

    }
    else if(info->job_type == COMMAND){

        queue_state = state->info.lan->queue_state_in;
        service_rates = state->info.lan->service_rates_in;
        direction = LAN_IN;

    }
    else{

        printf("ERROR: lan received a data not permitted\n");
        exit(EXIT_FAILURE);

    }
    update_metrics(now, queue_state, info);

    start_device(me, now, queue_state, service_rates, info, &direction, sizeof(lan_direction), FINISH);
}

void arrive_wan(unsigned int me, simtime_t now, lp_state * state, job_info* info){

    //printf("WAN: Message received\n");

    int up_node;
    double delay = state->info.wan->delay;

    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        up_node = GET_UPPER_NODE(state->topology);
        ScheduleNewEvent(up_node, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");
        up_node = GET_UPPER_NODE(state->topology);
        ScheduleNewEvent(up_node, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == COMMAND){

        //printf("COMMAND received!!!!\n");
        int * next_hop_list = GET_ACTUATOR_PATHS_INDEX(state->topology);
        int next_hop = next_hop_list[info->lp_destination];

        ScheduleNewEvent(next_hop, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == BATCH_DATA){

        up_node = GET_UPPER_NODE(state->topology);
        ScheduleNewEvent(up_node, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == REPLY){

        int next = info->lp_sender;
        ScheduleNewEvent(next, now + delay, ARRIVE, info, sizeof(job_info));

    }


}


void arrive_disk(unsigned int me, simtime_t now, lp_state * state, job_info * info){

    //PRINT("Message arrived in the disk");
    //PRINT_VALUE(me);

    update_metrics(now, state->info.node->disk_state, info);

    start_device(me, now, state->info.node->disk_state, GET_DISK_SERVICES(state->topology), info, NULL, 0, FINISH_DISK);

}

