#include "arrive_functions.h"


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

static void start_lan(unsigned int me, simtime_t now, queue_state * queue_state, double * service_rates, job_info * info, lan_direction direction){

    queue_state->current_job = info;
    queue_state->start_processing_timestamp = now;

    double rate = service_rates[info->job_type];
    simtime_t ts_finish = now + Expent(rate);
    ScheduleNewEvent(me, ts_finish, FINISH, &direction, sizeof(lan_direction));

}

//static void start_device()

static void update_metrics(queue_state * queue_state, job_info * info){

    queue_state->num_jobs_in_queue++;

    queue_state->A[info->job_type]++;
}

void arrive_node(unsigned int me, simtime_t now, lp_state * state, job_info * info){
    //printf("NODE\n");
    update_metrics(state->info.node->queue_state, info);

    if(state->info.node->queue_state->current_job == NULL)
        start_node(me, now, state, info);
    else
        schedule_in(state->info.node->queue_state->queues, info);

}

void arrive_actuator(unsigned int me, simtime_t now, lp_state * state, job_info * info){
    update_metrics(state->info.actuator->queue_state, info);

    if(state->info.actuator->queue_state->current_job == NULL)
        start_actuator(me, now, state, info);
    else
        schedule_in(state->info.actuator->queue_state->queues, info);
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
    update_metrics(queue_state, info);

    if(queue_state->current_job == NULL)
        start_lan(me, now, queue_state, service_rates, info, direction);
    else
        schedule_in(queue_state->queues, info);

}

void arrive_wan(unsigned int me, simtime_t now, lp_state * state, job_info* info){

    //printf("WAN: Message received\n");

    int up_node;
    double delay = state->info.wan->delay;

    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        up_node = getUpperNode(state->topology, me);
        ScheduleNewEvent(up_node, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");
        up_node = getUpperNode(state->topology, me);
        ScheduleNewEvent(up_node, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == COMMAND){

        //printf("COMMAND received!!!!\n");
        int * next_hop_list = getActuatorPathsIndex(state->topology, me);
        int next_hop = next_hop_list[info->lp_destination];

        ScheduleNewEvent(next_hop, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == BATCH_DATA){

        up_node = getUpperNode(state->topology, me);
        ScheduleNewEvent(up_node, now + delay, ARRIVE, info, sizeof(job_info));

    }
    else if(info->job_type == REPLY){

        int next = info->lp_sender;
        ScheduleNewEvent(next, now + delay, ARRIVE, info, sizeof(job_info));

    }


}
