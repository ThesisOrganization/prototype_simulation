#include "init_functions.h"

static double get_min_service_time(double* service_rates){
	
	double min_value = -1;
	memset(service_rates,-1,NUM_OF_JOB_TYPE);
	for(int i = 0; i < NUM_OF_JOB_TYPE - 1; i++){

		if(service_rates[i] > 0 && (min_value == -1 || service_rates[i] < min_value))
			min_value = service_rates[i];
		
	}
	
	return min_value;
	
}


static queue_conf** create_new_queues(int num_queues){


    queue_conf** queues = malloc(sizeof(queue_conf *) * num_queues);
    for(int i = 0; i < num_queues; i++){
        queues[i] = malloc(sizeof(queue_conf));
        queues[i]->queue = create_queue();
        queues[i]->type = i;
				queues[i]->prio = REAL_TIME;
        queues[i]->enqueue = enqueue;
        queues[i]->dequeue = dequeue;
        queues[i]->check_presence = check_presence;
        queues[i]->check_full = NULL;
				queues[i]->peek = queue_peek;
    }

    return queues;

}

static void init_metrics(queue_state * queue_state, int num_cores){
	
	queue_state->global_actual_timestamp = VALUE_NOT_SET;
	queue_state->global_actual_timestamp_stable = VALUE_NOT_SET;
	queue_state->global_start_timestamp = VALUE_NOT_SET;
	
	queue_state->current_jobs = malloc(sizeof(job_info)*num_cores);
	queue_state->start_processing_timestamp = malloc(sizeof(simtime_t)*num_cores);
	
	for(int j=0; j < num_cores; j++){
		queue_state->current_jobs[j].job_type = INVALID_JOB;
		queue_state->start_processing_timestamp[j] = VALUE_NOT_SET;
	}
	
	//queue_state->B_global = 0.0;
	//queue_state->B_global_stable = VALUE_NOT_SET;
	
	queue_state->C = malloc(sizeof(int)*NUM_OF_JOB_TYPE);
	queue_state->A = malloc(sizeof(int)*NUM_OF_JOB_TYPE);
	queue_state->A_post = malloc(sizeof(int)*NUM_OF_JOB_TYPE);
	queue_state->W = malloc(sizeof(double)*NUM_OF_JOB_TYPE);
	queue_state->B = malloc(sizeof(double)*NUM_OF_JOB_TYPE);
	queue_state->start_timestamp = malloc(sizeof(simtime_t)*NUM_OF_JOB_TYPE);
	queue_state->actual_timestamp = malloc(sizeof(simtime_t)*NUM_OF_JOB_TYPE);
	queue_state->old_response_times = malloc(sizeof(simtime_t)*NUM_OF_JOB_TYPE);
	
	queue_state->C_stable = malloc(sizeof(int)*NUM_OF_JOB_TYPE);
	queue_state->A_stable = malloc(sizeof(int)*NUM_OF_JOB_TYPE);
	queue_state->W_stable = malloc(sizeof(double)*NUM_OF_JOB_TYPE);
	queue_state->B_stable = malloc(sizeof(double)*NUM_OF_JOB_TYPE);
	queue_state->actual_timestamp_stable = malloc(sizeof(simtime_t)*NUM_OF_JOB_TYPE);
	
	queue_state->W2 = 0.0;
	
	for(int i=0; i < NUM_OF_JOB_TYPE; i++){
	
	    queue_state->C[i] = 0;
	    queue_state->A[i] = 0;
	    queue_state->A_post[i] = 0;
	    queue_state->W[i] = 0.0;
	    queue_state->B[i] = 0.0;
	    queue_state->start_timestamp[i] = 0.0;
	    queue_state->actual_timestamp[i] = 0.0;
	    queue_state->old_response_times[i] = -1.0;
	
			queue_state->C_stable[i] = -1;
	    queue_state->A_stable[i] = -1;
	    queue_state->W_stable[i] = -1.0;
	    queue_state->B_stable[i] = -1.0;
	    queue_state->actual_timestamp_stable[i] = -1.0;
	
	}

}

void generate_next_job(unsigned int id_device, simtime_t now, double rate_transition, double random_value, events_type type, unsigned int id_lp){
		if(rate_transition>0){
			double time_between_arrivals = 1/rate_transition;
			//simtime_t ts_generate = now + Expent(time_between_arrivals);
			simtime_t ts_generate = now + random_value + Expent(time_between_arrivals);
			message_generate msg;
			msg.header.element_id = id_device;
			ScheduleNewEvent(id_lp, ts_generate, type, &msg, sizeof(message_generate));
		}

}

void init_node(unsigned int id_device, device_state* state){
    //printf("node\n");

    state->info.node = malloc(sizeof(node_state));
    state->info.node->queue_state = malloc(sizeof(queue_state));
		//state->info.node->queue_state->current_job.job_type = INVALID_JOB;
    state->info.node->queue_state->num_jobs_in_queue = 0;
		
		state->info.node->queue_state->num_running_jobs = 0;
		state->info.node->queue_state->num_cores = GET_NUMBER_OF_CORES(state->topology);

    init_metrics(state->info.node->queue_state, state->info.node->queue_state->num_cores);

    int num_queues = NUM_OF_JOB_TYPE;
#if SCHEDULING_ROUND_ROBIN == 1
    state->info.node->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_RR);
#else
    state->info.node->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_PRIO_FIFO);
#endif

    state->info.node->service_rates = GET_SERVICE_TIMES_NODES(state->topology);

    //state->info.node->service_rates[TRANSITION] *= 2; //to delete

    state->info.node->type = GET_NODE_TYPE(state->topology);

    state->info.node->telemetry_aggregation = GET_AGGREGATION_RATE(state->topology)[TELEMETRY];
    state->info.node->num_telemetry_aggregated = 0;

    state->info.node->up_delay = GET_DELAY_UPPER_ROUTER(state->topology);
    state->info.node->down_delay = GET_DELAY_LOWER_ROUTER(state->topology);

    state->info.node->id_wan_down = GET_WAN_DOWN(state->topology);

    state->info.node->batch_aggregation = GET_AGGREGATION_RATE(state->topology)[BATCH_DATA];
    state->info.node->num_batch_aggregated = 0;

    //int node_type = state->info.node->type;
    //state->info.node->prob_cmd = state->topology->probNodeCommandArray[node_type];
    state->info.node->prob_cmd = GET_PROB_COMMAND(state->topology);
		
		state->info.node->queue_state->time_slice = get_min_service_time(state->info.node->service_rates);

    ///init disk queue

    if(GET_NODE_TYPE(state->topology) == CENTRAL){
        state->info.node->disk_state = malloc(sizeof(queue_state));
        //state->info.node->disk_state->current_job.job_type = INVALID_JOB;
        state->info.node->disk_state->num_jobs_in_queue = 0;
				
				state->info.node->disk_state->num_running_jobs = 0;
				state->info.node->disk_state->num_cores = 1;
				
				state->info.node->disk_state->time_slice = get_min_service_time(GET_DISK_SERVICES(state->topology));

        init_metrics(state->info.node->disk_state, state->info.node->disk_state->num_cores);

        num_queues = NUM_OF_JOB_TYPE;
#if SCHEDULING_ROUND_ROBIN == 1
        state->info.node->disk_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_RR);
#else
        state->info.node->disk_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_PRIO_FIFO);
#endif
    }

}

void init_sensor(unsigned int id_device, simtime_t now, device_state * state, unsigned int id_lp){

    //printf("sensor\n");

    state->info.sensor = malloc(sizeof(sensor_state));
    //int sensor_type = GET_SENSOR_TYPE(state->topology);
    //printf("%d\n", sensor_type);
    double * sensor_rate = GET_SENSOR_TYPE_RATES(state->topology);
    double rate_transition = sensor_rate[TRANSITION];
    double rate_telemetry = sensor_rate[TELEMETRY];

    state->info.sensor->rate_transition = rate_transition;
    state->info.sensor->rate_telemetry = rate_telemetry;

    //schedule generate for all sensors
    if(rate_transition>0){
			generate_next_job(id_device, now, rate_transition, Random()*RANDOM_START, GENERATE_TRANSITION, id_lp);
		}
    if(rate_telemetry>0){
			generate_next_job(id_device, now, rate_telemetry, Random()*RANDOM_START, GENERATE_TELEMETRY, id_lp);
		}

}

void init_actuator(unsigned int id_device, simtime_t now, device_state * state, unsigned int id_lp){

    //printf("actuator\n");

    state->info.actuator = malloc(sizeof(actuator_state));
    state->info.actuator->queue_state = malloc(sizeof(queue_state));
    //state->info.actuator->queue_state->current_job.job_type = INVALID_JOB;
    state->info.actuator->queue_state->num_jobs_in_queue = 0;

		state->info.actuator->queue_state->num_running_jobs = 0;
		state->info.actuator->queue_state->num_cores = 1;
		
    init_metrics(state->info.actuator->queue_state, state->info.actuator->queue_state->num_cores);

    int num_queues = NUM_OF_JOB_TYPE;
#if SCHEDULING_ROUND_ROBIN == 1
    state->info.actuator->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_RR);
#else
    state->info.actuator->queue_state->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_PRIO_FIFO);
#endif

    state->info.actuator->service_rate_command = GET_SERVICE_COMMAND(state->topology);

    double rate_transition = GET_RATE_TRANSITION(state->topology);
    state->info.actuator->rate_transition = rate_transition;
		
		
		double service_rates[NUM_OF_JOB_TYPE]; //meh
		memset(service_rates,0,sizeof(double)*NUM_OF_JOB_TYPE);
		service_rates[COMMAND] = state->info.actuator->service_rate_command;
		state->info.actuator->queue_state->time_slice = get_min_service_time(service_rates);

    //schedule generate for all actuators
		if(rate_transition>0){
			generate_next_job(id_device, now, rate_transition, Random()*RANDOM_START, GENERATE_TRANSITION, id_lp);
		}
}

void init_lan(unsigned int id_device, device_state * state){

    //printf("lan\n");

    state->info.lan = malloc(sizeof(lan_state));
    state->info.lan->queue_state_in = malloc(sizeof(queue_state));
    state->info.lan->queue_state_out = malloc(sizeof(queue_state));

    //state->info.lan->queue_state_in->current_job.job_type = INVALID_JOB;
    //state->info.lan->queue_state_out->current_job.job_type = INVALID_JOB;

    state->info.lan->queue_state_in->num_jobs_in_queue = 0;
    state->info.lan->queue_state_out->num_jobs_in_queue = 0;

		state->info.lan->queue_state_in->num_running_jobs = 0;
		state->info.lan->queue_state_in->num_cores = 1;
		state->info.lan->queue_state_out->num_running_jobs = 0;
		state->info.lan->queue_state_out->num_cores = 1;
		
    init_metrics(state->info.lan->queue_state_in, state->info.lan->queue_state_in->num_cores);
    init_metrics(state->info.lan->queue_state_out, state->info.lan->queue_state_out->num_cores);

    int num_queues = NUM_OF_JOB_TYPE;
#if SCHEDULING_ROUND_ROBIN == 1
    state->info.lan->queue_state_in->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_RR);
#else
    state->info.lan->queue_state_in->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_PRIO_FIFO);
#endif
		
#if SCHEDULING_ROUND_ROBIN == 1
    state->info.lan->queue_state_out->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_RR);
#else
    state->info.lan->queue_state_out->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO, SCHED_PRIO_FIFO);
#endif


    //int lan_type = GET_LAN_TYPE(state->topology);
    state->info.lan->service_rates_in = GET_LAN_IN_TYPE_SERVICE(state->topology);
    state->info.lan->service_rates_out = GET_LAN_OUT_TYPE_SERVICE(state->topology);
		
		state->info.lan->queue_state_in->time_slice = get_min_service_time(state->info.lan->service_rates_in);
		state->info.lan->queue_state_out->time_slice = get_min_service_time(state->info.lan->service_rates_out);

}

void init_wan(unsigned int id_device, device_state * state){

    //printf("wan\n");
    state->info.wan = malloc(sizeof(wan_state));

    state->info.wan->delay = GET_DELAY(state->topology);

}
