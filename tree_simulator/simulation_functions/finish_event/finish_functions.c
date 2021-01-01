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

	if(actuator == num_per_types[type]) //if remain_random == prob_type_i (i.e. num_per_types[type] * prob_actuators[type]) you get an index that is too higher (rare)
	    actuator--;

	//printf("Final: %d, %d\n", type, actuator);
	*pt_type = type;
	*pt_actuator = actuator;

}
/*
static void fill_info_to_send(job_info * info_to_send, job_type type, int sender, int destination){

	info_to_send->type = REAL_TIME;
	info_to_send->job_type = type;
	info_to_send->lp_sender = sender; //-1 of not used
	info_to_send->lp_destination = destination; //-1 of not used


}
*/
static void send_reply(unsigned int id_device, simtime_t now, device_state * state, int sender, double delay, double busy_time_transition, double waiting_time_transition, double order_shift){

	job_info info_to_send;
	//fill_info_to_send(&info_to_send, REPLY, sender, -1);
	fill_job_info(&info_to_send, -1.0, -1.0, REPLY, sender, busy_time_transition, waiting_time_transition, -1);

	//info_to_send.busy_time_transition = busy_time_transition;
	//info_to_send.waiting_time_transition = waiting_time_transition;

	int destination = state->info.node->id_wan_down;
	int next_lp = CONVERT_ELEMENT_TO_LP(state->topology, destination);

	message_arrive msg;
	msg.header.element_id = destination;
	msg.info = info_to_send;

	ScheduleNewEvent(next_lp, order_shift + now + delay, ARRIVE, &msg, sizeof(message_arrive));

}

static void send_command(unsigned int id_device, simtime_t now, device_state  * state, int id_selected_actuator, double delay_down, double order_shift){

	//if id_selected_actuator == -1, exit?
	if(id_selected_actuator == -1)
		return;
	int next_hop = GET_ACTUATOR_PATHS_INDEX(state->topology,id_selected_actuator);
	if(next_hop == -1){
		printf("WARNING: Actuator not found\n");
		return;
	}
	int next_lp = CONVERT_ELEMENT_TO_LP(state->topology, next_hop);

	job_info info_to_send;
	//fill_info_to_send(&info_to_send, COMMAND, -1, id_selected_actuator);
	fill_job_info(&info_to_send, -1.0, -1.0, COMMAND, -1, -1.0, -1.0, id_selected_actuator);

	message_arrive msg;
	msg.header.element_id = next_hop;
	msg.info = info_to_send;

	if(state->info.node->type != LOCAL)
		ScheduleNewEvent(next_lp, order_shift + now + delay_down, ARRIVE, &msg, sizeof(message_arrive));
	else
		ScheduleNewEvent(next_lp, order_shift + now, ARRIVE, &msg, sizeof(message_arrive));

}

static int get_id_random_actuator(unsigned int id_device, device_state  * state){

	int num_types = state->num_acts_types;
	if(num_types == 0)
		return -1; ///actuator can't be selected

	int * num_per_types = GET_ACT_TYPE_BELOW_LIST(state->topology);
	//for(int i = 0; i < num_types; i++)
	//    printf("%d: %d\n", me, num_per_types[i]);

	//type, selected_actuator = get_random_actuator(num_types, num_per_types);
	int type, selected_actuator;
	get_random_actuator(num_types, num_per_types, state->prob_actuators, &type, &selected_actuator);

	int * list_actuators_by_type = GET_LIST_ACTUATORS_BY_TYPE(state->topology, type);

	//printf("%d\n", list_actuators_by_type[0]);
	//printf("%d\n", list_actuators_by_type[1]);

	int id_selected_actuator = list_actuators_by_type[selected_actuator];

	return id_selected_actuator;

}

static void update_metrics(simtime_t now, queue_state * queue_state, job_info * info, int core){


	//printf("%d\n", queue_state->C);
	//printf("%d\n", info->job_type);


	job_type type = info->job_type;

	if(queue_state->start_timestamp[type] > TRANSITION_TIME_LIMIT){
		
		queue_state->global_actual_timestamp = now;

		//queue_state->B_global += (now - queue_state->last_timestamp_B_global) * queue_state->num_running_jobs;
		//queue_state->last_timestamp_B_global = now;
		
		queue_state->W2 += (now - queue_state->last_update_ts) * queue_state->num_jobs_in_queue;
		
		queue_state->C[type]++;
		//queue_state->B[type] += now - queue_state->start_processing_timestamp[core];
		queue_state->B[type] += info->total_computation;
		queue_state->W[type] += now - info->arrived_in_node_timestamp;
		queue_state->A[type] = queue_state->A_post[type];
		queue_state->actual_timestamp[type] = now;

		if(type == REPLY){

			//queue_state->C[TRANSITION]++; //(TO USE FOR REPLY UPDATE ALTERNATIVE)
			//queue_state->B[TRANSITION] += info->busy_time_transition; //(TO USE FOR REPLY UPDATE ALTERNATIVE)
			//queue_state->W[TRANSITION] += info->waiting_time_transition; //(TO USE FOR REPLY UPDATE ALTERNATIVE)
			
			//queue_state->B[TRANSITION] += now - queue_state->start_processing_timestamp[core];
			queue_state->B[TRANSITION] += info->total_computation;
			queue_state->W[TRANSITION] += now - info->arrived_in_node_timestamp;
			//queue_state->A[TRANSITION] = queue_state->A_post[TRANSITION]; //(TO USE FOR REPLY UPDATE ALTERNATIVE)
			queue_state->actual_timestamp[TRANSITION] = now;

		}

	}
	queue_state->num_jobs_in_queue--;
	queue_state->last_update_ts = now;

}

static void schedule_next_job(unsigned int id_device, simtime_t now, queue_state * queue_state, double * service_rates, lan_direction direction, events_type event_to_trigger, unsigned int id_lp, int core){

	int num_job_in_array = 1;
	job_info array_job_info[num_job_in_array];
	int ret_schedule = schedule_out(queue_state->queues, array_job_info, num_job_in_array);
	
	queue_state->current_jobs[core].job_type = INVALID_JOB; ///set the actual core to free. It will be overwrite if there is a job in queue that can be served
	queue_state->num_running_jobs--;
	
//#if PREEMPTION == 1
//	if(ret_schedule != SCHEDULE_DONE){
//		printf("schedule return: %d\n", ret_schedule);
//	}
//#endif
	
	if(ret_schedule == SCHEDULE_DONE){
		queue_state->current_jobs[core] = array_job_info[0];
		//double rate = service_rates[queue_state->current_jobs[core].job_type];
		//simtime_t ts_finish = now + Expent(rate);
		queue_state->start_processing_timestamp[core] = now;
		message_finish msg;
		msg.header.element_id = id_device;
		msg.core = core;
		msg.direction = direction;
		
		simtime_t ts_finish;
		double time_slice = queue_state->current_jobs[core].time_slice;
		double remain_computation = queue_state->current_jobs[core].remain_computation;
		
// 		printf("remain_computation: %f\n", remain_computation);
// 		printf("time_slice: %f\n", time_slice);
		
		if( remain_computation >= time_slice){
			ts_finish = time_slice;
		}
		else{
			ts_finish = remain_computation;
		}
		
		queue_state->current_jobs[core].time_slice = ts_finish;
// 		printf("time_slice in finish: %f\n", queue_state->current_jobs[core].time_slice);

		ScheduleNewEvent(id_lp, now + ts_finish, event_to_trigger, &msg, sizeof(message_finish));
		
		queue_state->num_running_jobs++;
	}

}

static void send_to_up_node(unsigned int id_device, simtime_t now, device_state  * state, double delay, job_info * info, double order_shift){
	int up_node;
#ifdef NO_CENTRAL
	if GET_NODE_TYPE(state->topology == REGIONAL){
		up_node = -1;
	}else{
		up_node = GET_UPPER_NODE(state->topology);
	}
#else
	up_node = GET_UPPER_NODE(state->topology);
#endif
	int up_lp = CONVERT_ELEMENT_TO_LP(state->topology, up_node);

	message_arrive msg;
	msg.header.element_id = up_node;
	msg.info = *info;

	if(up_node != -1)
		ScheduleNewEvent(up_lp, order_shift + now + delay, ARRIVE, &msg, sizeof(message_arrive));

}

static void save_data_on_disk(unsigned int id_device, simtime_t now, job_type type, unsigned int id_lp){

	job_info info_to_send;
	//fill_info_to_send(&info_to_send, type, -1, -1);
	fill_job_info(&info_to_send, -1.0, -1.0, type, -1, -1.0, -1.0, -1);

	message_arrive msg;
	msg.header.element_id = id_device;
	msg.info = info_to_send;

	ScheduleNewEvent(id_lp, SHIFT_EVENT_DISK + now, ARRIVE_DISK, &msg, sizeof(message_arrive));

}

static void send_aggregated_data(unsigned int id_device, simtime_t now, device_state  * state, double delay_up, job_type type, int * num_aggregated, int max_aggregated, unsigned int id_lp, double order_shift){

	int actual_aggr = ++(*num_aggregated);

	if(actual_aggr >= max_aggregated){

		job_info info_to_send;
		//fill_info_to_send(&info_to_send, type, -1, -1);
		fill_job_info(&info_to_send, -1.0, -1.0, type, -1, -1.0, -1.0, -1);

		send_to_up_node(id_device, now, state, delay_up, &info_to_send, order_shift);

		*num_aggregated = 0;

		//##################################################
		//SAVE DATA TO DISK
		if(GET_NODE_TYPE(state->topology) == CENTRAL)
			save_data_on_disk(id_device, now, type, id_lp);
	}
}
//unsigned int id_device, simtime_t now, queue_state * queue_state, double * service_rates, lan_direction direction, events_type event_to_trigger, unsigned int id_lp, int core
void schedule_remaining_job(simtime_t now, queue_state * queue_state, job_info * info){
	
	info->deadline = now;
	schedule_in(queue_state->queues, *info);
	
}

static void simulate_computation(double time){
	#if SIM_PROCESSING==1
	int j;
	for(int i=0;i<SIM_PROCESSING_MULTIPLIER*Expent(time);i++){
		j=i*j;
	}
	#endif
}


void finish_node(unsigned int id_device, simtime_t now, device_state  * state, unsigned int id_lp, int core){

	//job_info * info = state->info.node->queue_state->current_job;
	job_info current_job = state->info.node->queue_state->current_jobs[core];
	job_info * info = &current_job;
	simulate_computation(info->time_slice);
	//if not completed
	//schedule in
	//schedule next job
	//printf("%f\n", info->remain_computation);
	info->remain_computation -= info->time_slice;
	//printf("%f\n", info->remain_computation);
	
	if(info->remain_computation < 0)
		printf("WARNING: remain_computation it's less than zero..\n");

	
	if(info->remain_computation > 0){
		schedule_remaining_job(now, state->info.node->queue_state, info);
		schedule_next_job(id_device, now, state->info.node->queue_state, state->info.node->service_rates, 0, FINISH, id_lp, core);
		return;
	}

	double busy_time_transition = now - state->info.node->queue_state->start_processing_timestamp[core];
	double waiting_time_transition = now - info->arrived_in_node_timestamp;

	//Update metrics
	//if(!( info->job_type == TRANSITION && (state->info.node->type == LOCAL || state->info.node->type == REGIONAL) )) //(TO USE FOR REPLY UPDATE ALTERNATIVE)
		update_metrics(now, state->info.node->queue_state, info, core);


	double delay_up = state->info.node->up_delay;
	double delay_down = state->info.node->down_delay;


	if(info->job_type == TELEMETRY){
		//printf("TELEMETRY\n");
		send_aggregated_data(id_device, now, state, delay_up, TELEMETRY, &state->info.node->num_telemetry_aggregated, state->info.node->telemetry_aggregation, id_lp, SHIFT_EVENT_FIRST);


	}
	else if(info->job_type == TRANSITION){
		//printf("TRANSITION\n");
		//###################################################
		//SEND REPLY
		if(state->info.node->type != LOCAL){
			send_reply(id_device, now, state, info->device_sender, delay_down, info->busy_time_transition, info->waiting_time_transition, SHIFT_EVENT_FIRST);
		}

		//###################################################
		//GENERATE COMMAND
		double random_prob = Random();
		double prob_cmd = state->info.node->prob_cmd;
		if (random_prob <= prob_cmd){
			int id_selected_actuator = get_id_random_actuator(id_device, state);
			send_command(id_device, now, state, id_selected_actuator, delay_down, SHIFT_EVENT_SECOND);

			//###################################################
			//SEND BATCH_DATA
			send_aggregated_data(id_device, now, state, delay_up, BATCH_DATA, &state->info.node->num_batch_aggregated, state->info.node->batch_aggregation, id_lp, SHIFT_EVENT_THIRD);
			
			//###################################################
			//UPDATE TRANSITION METRICS (TO USE FOR REPLY UPDATE ALTERNATIVE)
			//if(state->info.node->type == LOCAL || state->info.node->type == REGIONAL) //(TO USE FOR REPLY UPDATE ALTERNATIVE)
			//	update_metrics(now, state->info.node->queue_state, info,core); //(TO USE FOR REPLY UPDATE ALTERNATIVE)

		}
		else{
			//###################################################
			//FORWARD TRANSITION
			
			job_info info_to_send; //this should be a copy of the transition that we are processing (in terms of payload). At least in the old implementation
			fill_job_info(&info_to_send, -1.0, -1, info->job_type, id_device, busy_time_transition, waiting_time_transition, -1);
			//info->busy_time_transition = busy_time_transition;
			//info->waiting_time_transition = waiting_time_transition;
			//info->device_sender = id_device;
			send_to_up_node(id_device, now, state, delay_up, &info_to_send, SHIFT_EVENT_SECOND);
			
			//##################################################
			//SAVE DATA TO DISK
			if(GET_NODE_TYPE(state->topology) == CENTRAL)
				save_data_on_disk(id_device, now, TRANSITION, id_lp);
			
		}
		
	}
	else if(info->job_type == COMMAND){

		send_command(id_device, now, state, info->device_destination, delay_down, SHIFT_EVENT_FIRST);

	}
	else if(info->job_type == BATCH_DATA){

		send_aggregated_data(id_device, now, state, delay_up, BATCH_DATA, &state->info.node->num_batch_aggregated, state->info.node->batch_aggregation, id_lp, SHIFT_EVENT_FIRST);

	}
	else if(info->job_type == REPLY){

		//printf("REPLY RECEIVED\n");
		//printf("%f, %d, %d\n", now, me, info->lp_sender);

	}
	
	//Schedule the next job if present
	schedule_next_job(id_device, now, state->info.node->queue_state, state->info.node->service_rates, 0, FINISH, id_lp, core);

}

void finish_actuator(unsigned int id_device, simtime_t now, device_state  * state, unsigned int id_lp, int core){

	job_info current_job = state->info.actuator->queue_state->current_jobs[core];
	job_info * info = &current_job;
	simulate_computation(info->time_slice);
	//printf("%f\n", info->remain_computation);
	info->remain_computation -= info->time_slice;
	//printf("%f\n", info->remain_computation);
	if(info->remain_computation < 0)
		printf("WARNING: remain_computation it's less than zero..\n");
	
	if(info->remain_computation > 0){
		schedule_remaining_job(now, state->info.node->queue_state, info);
		double service_rates[NUM_OF_JOB_TYPE]; //meh
		service_rates[COMMAND] = state->info.actuator->service_rate_command;
		schedule_next_job(id_device, now, state->info.actuator->queue_state, service_rates, 0, FINISH, id_lp, core);
		return;
	}
	
	//Update metrics
	update_metrics(now, state->info.actuator->queue_state, info, core);


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

	//Schedule the next job if present
	double service_rates[NUM_OF_JOB_TYPE]; //meh
	service_rates[COMMAND] = state->info.actuator->service_rate_command;
	schedule_next_job(id_device, now, state->info.actuator->queue_state, service_rates, 0, FINISH, id_lp, core);
	
}

void finish_lan(unsigned int id_device, simtime_t now, device_state  * state, lan_direction direction, unsigned int id_lp, int core){

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

	job_info current_job = queue_state->current_jobs[core];
	job_info * info = &current_job;
	simulate_computation(info->time_slice);
	//printf("%f\n", info->remain_computation);
	info->remain_computation -= info->time_slice;
	//printf("%f\n", info->remain_computation);
	if(info->remain_computation < 0)
		printf("WARNING: remain_computation it's less than zero..\n");
	
	if(info->remain_computation > 0){
		schedule_remaining_job(now, queue_state, info);
		schedule_next_job(id_device, now, queue_state, service_rates, direction, FINISH, id_lp, core);
		return;
	}
	
	//Update metrics
	update_metrics(now, queue_state, info, core);


	if(info->job_type == TELEMETRY){
		//printf("TELEMETRY\n");
		send_to_up_node(id_device, now, state, 0.0, info, SHIFT_EVENT_FIRST);

	}
	else if(info->job_type == TRANSITION){
		//printf("TRANSITION\n");
		send_to_up_node(id_device, now, state, 0.0, info, SHIFT_EVENT_FIRST);

	}
	else if(info->job_type == COMMAND){

		int destination = info->device_destination; //you should use the function of the topology
		if(destination == -1){
			printf("WARNING: command destination not present\n");
			return;
		}
		int next_lp = CONVERT_ELEMENT_TO_LP(state->topology, destination);

		message_arrive msg;
		msg.header.element_id = destination;
		msg.info = *info;

		ScheduleNewEvent(next_lp, SHIFT_EVENT_FIRST + now, ARRIVE, &msg, sizeof(message_arrive));

	}
	else if(info->job_type == BATCH_DATA){

		printf("WARNING: lan received a batch data\n");

	}
	else if(info->job_type == REPLY){

		printf("WARNING: lan received a reply data\n");
	}

	//Schedule the next job if present
	schedule_next_job(id_device, now, queue_state, service_rates, direction, FINISH, id_lp, core);

}


void finish_disk(unsigned int id_device, simtime_t now, device_state * state, unsigned int id_lp, int core){

	job_info current_job = state->info.node->disk_state->current_jobs[core];
	job_info * info = &current_job;
	simulate_computation(info->time_slice);
	//printf("%f\n", info->remain_computation);
	info->remain_computation -= info->time_slice;
	//printf("%f\n", info->remain_computation);
	if(info->remain_computation < 0)
		printf("WARNING: remain_computation it's less than zero..\n");
	
	if(info->remain_computation > 0){
		schedule_remaining_job(now, state->info.node->disk_state, info);
		schedule_next_job(id_device, now, state->info.node->disk_state, GET_DISK_SERVICES(state->topology), 0, FINISH_DISK, id_lp, core);
		return;
	}
	
	//Update metrics
	update_metrics(now, state->info.node->disk_state, info, core);

	//Schedule the next job if present
	schedule_next_job(id_device, now, state->info.node->disk_state, GET_DISK_SERVICES(state->topology), 0, FINISH_DISK, id_lp, core);

}

