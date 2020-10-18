#include "finish_functions.h"



static void get_random_actuator(int num_types, int * num_per_types, double * prob_actuators, int * pt_type, int * pt_actuator){

    double sum_prob_numbers = 0;
    int i;
    for(i = 0; i < num_types; i++){
        //printf("%d, %f\n", num_per_types[i], prob_actuators[i]);
        sum_prob_numbers += num_per_types[i] * prob_actuators[i];
    }

    double random_value = Random();

    double temp = random_value;
    double prob_type_i;
    for(i = 0; i < num_types; i++){
        prob_type_i = num_per_types[i]*prob_actuators[i] / sum_prob_numbers;
        if (temp - prob_type_i <= 0)
            break;
        temp = temp - prob_type_i;
    }

    int type = i;
    double remain_random = temp;
    //printf("%d, %f\n", i, temp);
    double prob_single_act = prob_actuators[type] / sum_prob_numbers;

    int actuator = remain_random / prob_single_act;
    //printf("Final: %d, %d\n", type, actuator);
    *pt_type = type;
    *pt_actuator = actuator;

}

static void fill_info_to_send(job_info * info_to_send, job_type type, int sender, int destination){

    info_to_send->type = REAL_TIME;
    info_to_send->payload = NULL;
    info_to_send->job_type = type;
    info_to_send->lp_sender = sender; //-1 of not used
    info_to_send->lp_destination = destination; //-1 of not used


}

static void send_reply(unsigned int me, simtime_t now, lp_state * state, int sender, double delay){

    job_info info_to_send;
    fill_info_to_send(&info_to_send, REPLY, sender, -1);

    int destination = state->info.node->id_wan_down;

    //printf("%d\n", state->info.node->type);

    ScheduleNewEvent(destination, now + delay, ARRIVE, &info_to_send, sizeof(job_info));

}

static void send_command(unsigned int me, simtime_t now, lp_state * state, int id_selected_actuator, double delay_down){

    int * next_hop_list = GET_ACTUATOR_PATHS_INDEX(state->topology, me);
    int next_hop = next_hop_list[id_selected_actuator];
    
    job_info info_to_send;
    fill_info_to_send(&info_to_send, COMMAND, -1, id_selected_actuator);


    if(state->info.node->type != LOCAL)
        ScheduleNewEvent(next_hop, now + delay_down, ARRIVE, &info_to_send, sizeof(job_info));
    else
        ScheduleNewEvent(next_hop, now, ARRIVE, &info_to_send, sizeof(job_info));

}

static int get_id_random_actuator(unsigned int me, lp_state * state){

    int num_types = state->num_acts_types;

    int * num_per_types = GET_ACT_TYPE(state->topology, me);
    //for(int i = 0; i < num_types; i++)
    //    printf("%d: %d\n", me, num_per_types[i]);

    //type, selected_actuator = get_random_actuator(num_types, num_per_types);
    int type, selected_actuator;
    get_random_actuator(num_types, num_per_types, state->prob_actuators, &type, &selected_actuator);

    int * list_actuators_by_type = GET_LIST_ACTUATORS_BY_TYPE(state->topology, me, type);

    //printf("%d\n", list_actuators_by_type[0]);
    //printf("%d\n", list_actuators_by_type[1]);

    int id_selected_actuator = list_actuators_by_type[selected_actuator];


    return id_selected_actuator;

}

static void update_metrics(simtime_t now, queue_state * queue_state, job_info * info){

    queue_state->num_jobs_in_queue--;

    //printf("%d\n", queue_state->C);
    //printf("%d\n", info->job_type);

    queue_state->C[info->job_type]++;
    queue_state->B[info->job_type] += now - queue_state->start_processing_timestamp;
    queue_state->W[info->job_type] += now - info->arrived_in_node_timestamp;

    if(info->job_type == REPLY){
        
        queue_state->B[TRANSITION] += now - queue_state->start_processing_timestamp;
        queue_state->W[TRANSITION] += now - info->arrived_in_node_timestamp;

    }

}

static job_info ** schedule_next_job(unsigned int me, simtime_t now, queue_state * queue_state, double * service_rates, lan_direction * direction, int size_lan_direction, events_type event_to_trigger){

    job_info ** info_arr = schedule_out(queue_state->queues);
    queue_state->current_job = info_arr[0];

    if(queue_state->current_job != NULL){
        double rate = service_rates[queue_state->current_job->job_type];
        simtime_t ts_finish = now + Expent(rate);
        queue_state->start_processing_timestamp = now;
        ScheduleNewEvent(me, ts_finish, event_to_trigger, direction, size_lan_direction);
    }

    return info_arr;

}

static void send_to_up_node(unsigned int me, simtime_t now, lp_state * state, double delay, job_info * info){

    int up_node = GET_UPPER_NODE(state->topology, me);
    if(up_node != -1)
        ScheduleNewEvent(up_node, now + delay, ARRIVE, info, sizeof(job_info));

}

static void save_data_on_disk(unsigned int me, simtime_t now, job_type type){

    job_info info_to_send;
    fill_info_to_send(&info_to_send, type, -1, -1);

    ScheduleNewEvent(me, now, ARRIVE_DISK, &info_to_send, sizeof(job_info));

}

static void send_aggregated_data(unsigned int me, simtime_t now, lp_state * state, double delay_up, job_type type, int * num_aggregated, int max_aggregated){

    int actual_aggr = (*num_aggregated)++;

    if(actual_aggr >= max_aggregated){

        job_info info_to_send;
        fill_info_to_send(&info_to_send, type, -1, -1);

        send_to_up_node(me, now, state, delay_up, &info_to_send);

        *num_aggregated = 0;

        //##################################################
        //SAVE DATA TO DISK
        if(GET_NODE_TYPE(state->topology, me) == CENTRAL)
            save_data_on_disk(me, now, type);
    }
}

void finish_node(unsigned int me, simtime_t now, lp_state * state){

    job_info * info = state->info.node->queue_state->current_job;

    //Update metrics
    update_metrics(now, state->info.node->queue_state, info);

    //Schedule the next job if present
    job_info ** info_arr = schedule_next_job(me, now, state->info.node->queue_state, state->info.node->service_rates, NULL, 0, FINISH);

    double delay_up = state->info.node->up_delay;
    double delay_down = state->info.node->down_delay;

    //int up_node;

    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        send_aggregated_data(me, now, state, delay_up, TELEMETRY, &state->info.node->num_telemetry_aggregated, state->info.node->telemetry_aggregation);


    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");
        //###################################################
        //SEND REPLY
        if(state->info.node->type != LOCAL)
            send_reply(me, now, state, info->lp_sender, delay_down);

        //###################################################
        //GENERATE COMMAND
        double random_prob = Random();
        double prob_cmd = state->info.node->prob_cmd;
        if (random_prob <= prob_cmd){
            int id_selected_actuator = get_id_random_actuator(me, state);
            send_command(me, now, state, id_selected_actuator, delay_down);

            //###################################################
            //SEND BATCH_DATA
            send_aggregated_data(me, now, state, delay_up, BATCH_DATA, &state->info.node->num_batch_aggregated, state->info.node->batch_aggregation);

        }

        //###################################################
        //FORWARD TRANSITION
        info->lp_sender = me;
        send_to_up_node(me, now, state, delay_up, info);

        //##################################################
        //SAVE DATA TO DISK
        if(GET_NODE_TYPE(state->topology, me) == CENTRAL)
            save_data_on_disk(me, now, TRANSITION);

    }
    else if(info->job_type == COMMAND){

        send_command(me, now, state, info->lp_destination, delay_down);

    }
    else if(info->job_type == BATCH_DATA){

        send_aggregated_data(me, now, state, delay_up, BATCH_DATA, &state->info.node->num_batch_aggregated, state->info.node->batch_aggregation);

    }
    else if(info->job_type == REPLY){

        //printf("REPLY RECEIVED\n");
        //printf("%f, %d, %d\n", now, me, info->lp_sender);

    }

    free(info_arr); //liberi l'array dell'attuale job!
    free(info); //liberi il vecchio job
}

void finish_actuator(unsigned int me, simtime_t now, lp_state * state){

    job_info * info = state->info.actuator->queue_state->current_job;

    //Update metrics
    update_metrics(now, state->info.actuator->queue_state, info);

    //Schedule the next job if present
    double service_rates[NUM_OF_JOB_TYPE]; //meh
    service_rates[COMMAND] = state->info.actuator->service_rate_command;
    job_info ** info_arr = schedule_next_job(me, now, state->info.actuator->queue_state, service_rates, NULL, 0, FINISH);

    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        printf("WARNING: actuator received a telemetry data\n");

    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");
        printf("WARNING: actuator received a transition data\n");

    }
    else if(info->job_type == COMMAND){

        //printf("COMMAND received!!!!\n");

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

void finish_lan(unsigned int me, simtime_t now, lp_state * state, lan_direction direction){

    queue_state * queue_state;
    double * service_rates;

    if(direction == LAN_OUT){

        queue_state = state->info.lan->queue_state_out;
        service_rates = state->info.lan->service_rates_out;

    }
    else if(direction == LAN_IN){

        queue_state = state->info.lan->queue_state_in;
        service_rates = state->info.lan->service_rates_in;

    }
    else{

        printf("ERROR: lan received a direction not permitted\n");
        exit(EXIT_FAILURE);

    }

    job_info * info = queue_state->current_job;

    //Update metrics
    update_metrics(now, queue_state, info);

    //Schedule the next job if present
    job_info ** info_arr = schedule_next_job(me, now, queue_state, service_rates, &direction, sizeof(lan_direction), FINISH);


    if(info->job_type == TELEMETRY){
        //printf("TELEMETRY\n");
        send_to_up_node(me, now, state, 0, info);

    }
    else if(info->job_type == TRANSITION){
        //printf("TRANSITION\n");
        send_to_up_node(me, now, state, 0, info);

    }
    else if(info->job_type == COMMAND){

        int destination = info->lp_destination; //you should use the function of the topology
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


void finish_disk(unsigned int me, simtime_t now, lp_state * state){

    //PRINT("Finish event in the disk");
    //PRINT_VALUE(me);

    job_info * info = state->info.node->disk_state->current_job;

    //Update metrics
    update_metrics(now, state->info.node->disk_state, info);

    job_info ** info_arr = schedule_next_job(me, now, state->info.node->disk_state, GET_DISK_SERVICES(state->topology, me), NULL, 0, FINISH_DISK);

    free(info_arr);
    free(info);

}

