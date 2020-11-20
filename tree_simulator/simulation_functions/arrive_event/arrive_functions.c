#include "arrive_functions.h"

static void start_device(unsigned int id_device, simtime_t now, queue_state * queue_state, double * service_rates, job_info * info, lan_direction direction, events_type event_to_trigger, unsigned int id_lp){

	if(queue_state->num_running_jobs < queue_state->num_cores){
	//if(queue_state->current_job.job_type == INVALID_JOB){

		queue_state->current_jobs[queue_state->num_running_jobs] = *info; //usa indice num_current_jobs
		queue_state->start_processing_timestamp[queue_state->num_running_jobs] = now; //usa indice num_current_jobs

		double rate = service_rates[info->job_type];
		simtime_t ts_finish = now + Expent(rate);
		message_finish msg;
		msg.header.element_id = id_device;
		msg.core = queue_state->num_running_jobs;
		msg.direction = direction;
		ScheduleNewEvent(id_lp, ts_finish, event_to_trigger, &msg, sizeof(message_finish));
		
		queue_state->num_running_jobs++;

	}
	else
		schedule_in(queue_state->queues, *info);


}

static void update_metrics(simtime_t now, queue_state * queue_state, job_info * info){
	

	//queue_state->A[info->job_type]++;

	job_type type = info->job_type;

	if(queue_state->start_timestamp[type] <= TRANSITION_TIME_LIMIT && now > TRANSITION_TIME_LIMIT)
		queue_state->start_timestamp[type] = now;


	if(queue_state->start_timestamp[type] > TRANSITION_TIME_LIMIT){

		queue_state->A_post[type]++;
		queue_state->W2 += (now - queue_state->last_update_ts) * queue_state->num_jobs_in_queue;

	}
	queue_state->num_jobs_in_queue++;
	queue_state->last_update_ts = now;
}

void arrive_node(unsigned int id_device, simtime_t now, device_state* state, job_info * info, unsigned int id_lp){
	//printf("NODE\n");
	update_metrics(now, state->info.node->queue_state, info);

	start_device(id_device, now, state->info.node->queue_state, state->info.node->service_rates, info, 0, FINISH, id_lp);
}

void arrive_actuator(unsigned int id_device, simtime_t now, device_state * state, job_info * info, unsigned int id_lp){
	update_metrics(now, state->info.actuator->queue_state, info);

	double service_rates[NUM_OF_JOB_TYPE]; //meh
	service_rates[COMMAND] = state->info.actuator->service_rate_command;
	start_device(id_device, now, state->info.actuator->queue_state, service_rates, info, 0, FINISH, id_lp);
}

void arrive_lan(unsigned int id_device, simtime_t now, device_state * state, job_info* info, unsigned int id_lp){

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

	start_device(id_device, now, queue_state, service_rates, info, direction, FINISH, id_lp);
}

static void schedule_wan(unsigned int next_lp, simtime_t now, double delay, job_info* info, unsigned int next_device){

	message_arrive msg;
	msg.header.element_id = next_device;
	msg.info = *info;
	
	ScheduleNewEvent(next_lp, now + delay, ARRIVE, &msg, sizeof(message_arrive));

}

void arrive_wan(unsigned int id_device, simtime_t now, device_state * state, job_info* info){

	//printf("WAN: Message received\n");

	int up_node;
	double delay = state->info.wan->delay;
	int up_lp;

	if(info->job_type == TELEMETRY){
		//printf("TELEMETRY\n");
		up_node = GET_UPPER_NODE(state->topology);
		up_lp = CONVERT_ELEMENT_TO_LP(state->topology, up_node);
		schedule_wan(up_lp, now, delay, info, up_node);

	}
	else if(info->job_type == TRANSITION){
		//printf("TRANSITION\n");
		up_node = GET_UPPER_NODE(state->topology);
		up_lp = CONVERT_ELEMENT_TO_LP(state->topology, up_node);
		schedule_wan(up_lp, now, delay, info, up_node);

	}
	else if(info->job_type == COMMAND){

		//printf("COMMAND received!!!!\n");
		int next_hop = GET_ACTUATOR_PATHS_INDEX(state->topology,info->device_destination);
		up_lp = CONVERT_ELEMENT_TO_LP(state->topology, next_hop);

		schedule_wan(up_lp, now, delay, info, next_hop);

	}
	else if(info->job_type == BATCH_DATA){

		up_node = GET_UPPER_NODE(state->topology);
		up_lp = CONVERT_ELEMENT_TO_LP(state->topology, up_node);

		schedule_wan(up_lp, now, delay, info, up_node);

	}
	else if(info->job_type == REPLY){

		int next = info->device_sender;
		up_lp = CONVERT_ELEMENT_TO_LP(state->topology, next);

		schedule_wan(up_lp, now, delay, info, next);

	}

	//free(info);


}


void arrive_disk(unsigned int id_device, simtime_t now, device_state * state, job_info * info, unsigned int id_lp){

	//PRINT("Message arrived in the disk");
	//PRINT_VALUE(me);

	update_metrics(now, state->info.node->disk_state, info);

	start_device(id_device, now, state->info.node->disk_state, GET_DISK_SERVICES(state->topology), info, 0, FINISH_DISK, id_lp);

}

