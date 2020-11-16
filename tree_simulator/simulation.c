#include "./simulation.h"


char topology_path[] = "./topology.txt";
char file_name[] = "lp_data/lp";
char end_file_name[] = ".json";
char file_name_complete[64];

#ifdef DEBUG_INITIAL_VALUES
void print_array_double(double * array, int num_el){

	for(int i=0; i<num_el; i++)
		printf("%f ", array[i]);

	printf("\n");

}
void print_array_int(int * array, int num_el){

	for(int i=0; i<num_el; i++)
		printf("%d ", array[i]);

	printf("\n");
}

#endif

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define CHECK_TTCB 15
#define CHECK_C 4
#define CHECK_TTB 11
#define CHECK_TTC 7
#define CHECK_TT 3

int get_flag_from_bitmap(unsigned int bitmap, int index){

	unsigned int shifted = bitmap >> index;
	return shifted & 1;

}


unsigned int check_metrics(queue_state * queue_state, unsigned int bitmap, int min_number_of_events){

	int return_bool = 1;
	int i;
	int sum_arrived = 0;

	for(i=0; i < NUM_OF_JOB_TYPE; i++){
		sum_arrived += queue_state->C[i];
		unsigned int flag = get_flag_from_bitmap(bitmap, i);
		if(flag && queue_state->C[i] < min_number_of_events && queue_state->C[i] > 0){
			return_bool = 0;
			return return_bool;
		}
	}
	
	if(sum_arrived == 0){
		return_bool = 0;
		return return_bool;
	}

	for(i=0; i < NUM_OF_JOB_TYPE; i++){
		double R = queue_state->W[i] / queue_state->C[i];
		double normalization = MAX(R, queue_state->old_response_times[i]);
		double diff_value = ( fabs(R - queue_state->old_response_times[i]) ) / normalization;
		double threshold = THRESHOLD*0.01;
		if(diff_value > threshold)
			return_bool = 0;
		queue_state->old_response_times[i] = R;
	}

	return return_bool;
}

void update_stable_metrics(queue_state * queue_state){

	memcpy(queue_state->C_stable, queue_state->C, sizeof(int)*NUM_OF_JOB_TYPE);
	memcpy(queue_state->A_stable, queue_state->A, sizeof(int)*NUM_OF_JOB_TYPE);
	memcpy(queue_state->W_stable, queue_state->W, sizeof(double)*NUM_OF_JOB_TYPE);
	memcpy(queue_state->B_stable, queue_state->B, sizeof(double)*NUM_OF_JOB_TYPE);
	memcpy(queue_state->actual_timestamp_stable, queue_state->actual_timestamp, sizeof(double)*NUM_OF_JOB_TYPE);

}

void broadcast_message(int number_lps_enabled, simtime_t ts_to_send, events_type event_to_broadcast){

	for(int lp = 0; lp < number_lps_enabled; lp++){

		ScheduleNewEvent(lp, ts_to_send, event_to_broadcast, NULL, 0);

	}

}

void schedule_first_update_timestamp(unsigned int me, simtime_t now, int id_device){

	message_update msg_update;
	simtime_t ts_update_timestamp;
	ts_update_timestamp = now + NEXT_UPDATE_TIMESTAMP + id_device;
	msg_update.header.element_id = id_device;
	ScheduleNewEvent(me, ts_update_timestamp, UPDATE_TIMESTAMP, &msg_update, sizeof(message_update));

}


void ProcessEvent(unsigned int me, simtime_t now, unsigned int event_type, void *content, int size, lp_state * state)
{

	message_arrive msg_arrive;
	//message_finish msg_finish;
	//message_generate msg_generate;

	int id_device;
	int index_map;

	int up_node;
	int up_lp;
	job_info * info;
	job_info copy_info;
	double rate_generate;
	device_state * dev_state;

	switch(event_type) {

		case INIT:
			//all nodes except the master node are disabled by default
			state = malloc(sizeof(lp_state));
			//a safe memset to obtain a clean starting state
			memset(state,0,sizeof(lp_state));
			SetState(state);
			state->lp_enabled = LP_SETUP;
			if(me == 0){
				//we enable the master LP to avoid terminating the simulation during the setup
				//we setup the master node and the required LPs
				setup_master(n_prc_tot);
			}
			break;

		case DISABLE_UNUSED_LP:
			
			state->lp_enabled=LP_DISABLED;
			
			break;

		case RECEIVE_SETUP_MESSAGE:

			recv_setup_message(state,content);

			break;

		case START_SIMULATION:
			//we enable the LP
			state->lp_enabled=LP_ENABLED;
			state->num_stable_elements=0;

			for(int index = 0; index < state->num_devices; index++){

				idmap map = state->element_to_index[index];
				id_device = map.id;
				index_map = map.content;
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;

				dev_state->num_acts_types=GET_NUMBER_ACT_TYPES(state->general_topology);
				dev_state->prob_actuators=GET_PROB_ACTUATORS(state->general_topology);

				dev_state->type = GET_TYPE(dev_state->topology);
				dev_state->simulation_completed = SIMULATION_ACTIVE;

				//initializza strutture
				if(dev_state->type == NODE){

					init_node(id_device, dev_state);
					dev_state->stability = ELEMENT_UNSTABLE;
					schedule_first_update_timestamp(me, now, id_device);

				}
				else if(dev_state->type == SENSOR){

					init_sensor(id_device, now, dev_state, me);
					dev_state->stability = ELEMENT_STABLE;

				}
				else if(dev_state->type == ACTUATOR){

					init_actuator(id_device, now, dev_state, me);
					dev_state->stability = ELEMENT_UNSTABLE;
					schedule_first_update_timestamp(me, now, id_device);

				}
				else if(dev_state->type == LAN){

					init_lan(id_device, dev_state);
					dev_state->stability = ELEMENT_UNSTABLE;
					schedule_first_update_timestamp(me, now, id_device);

				}
				else if(dev_state->type == WAN){

					init_wan(id_device, dev_state);
					dev_state->stability = ELEMENT_STABLE;

				}
				else{

					printf("Error: device type not found\n");
					exit(EXIT_FAILURE);
				}

			}


			break;

			case GENERATE_TRANSITION:

				id_device = ((message_generate*)content)->header.element_id;
				index_map = idmap_search(state->element_to_index, id_device, state->num_devices);
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;

				//msg_arrive.info.type = REAL_TIME;
				//info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
				//msg_arrive.info.job_type = TRANSITION;
				fill_job_info(&msg_arrive.info, -1.0, -1.0, TRANSITION, -1, -1.0, -1.0, -1);

				up_node = GET_UPPER_NODE(dev_state->topology);
				msg_arrive.header.element_id = up_node;
				up_lp = CONVERT_ELEMENT_TO_LP(dev_state->topology, up_node);
				ScheduleNewEvent(up_lp, now, ARRIVE, &msg_arrive, sizeof(message_arrive));

				if(dev_state->type == SENSOR){

					rate_generate = dev_state->info.sensor->rate_transition;

				}
				else if(dev_state->type == ACTUATOR){

					rate_generate = dev_state->info.actuator->rate_transition;

				}
				else{

					printf("Error: device type not found\n");
					exit(EXIT_FAILURE);
				}

				generate_next_job(id_device, now, rate_generate, 0.0, GENERATE_TRANSITION, me);

				break;

			case GENERATE_TELEMETRY:

				id_device = ((message_generate*)content)->header.element_id;
				index_map = idmap_search(state->element_to_index, id_device, state->num_devices);
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;

				//msg_arrive.info.type = REAL_TIME;
				//info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
				//msg_arrive.info.job_type = TELEMETRY;
				fill_job_info(&msg_arrive.info, -1.0, -1.0, TELEMETRY, -1, -1.0, -1.0, -1);

				up_node = GET_UPPER_NODE(dev_state->topology);
				msg_arrive.header.element_id = up_node;
				up_lp = CONVERT_ELEMENT_TO_LP(dev_state->topology, up_node);
				ScheduleNewEvent(up_lp, now, ARRIVE, &msg_arrive, sizeof(message_arrive));

				if(dev_state->type == SENSOR){

					rate_generate = dev_state->info.sensor->rate_telemetry;

				}
				else{

					printf("Error: device type not found\n");
					exit(EXIT_FAILURE);
				}

				generate_next_job(id_device, now, rate_generate, 0.0, GENERATE_TELEMETRY, me);

				break;

			case ARRIVE:

				id_device = ((message_arrive*)content)->header.element_id;
				index_map = idmap_search(state->element_to_index, id_device, state->num_devices);
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;

				//info = malloc(sizeof(job_info));
				//memcpy(info, &(((message_arrive*)content)->info), sizeof(job_info));
				copy_info = ((message_arrive*)content)->info;
				info = &copy_info;

				info->arrived_in_node_timestamp = now;
				info->deadline = now;

				if(dev_state->type == NODE){

					arrive_node(id_device, now, dev_state, info, me);

				}
				else if(dev_state->type == ACTUATOR){

					arrive_actuator(id_device, now, dev_state, info, me);

				}
				else if(dev_state->type == LAN){

					arrive_lan(id_device, now, dev_state, info, me);

				}
				else if(dev_state->type == WAN){

					arrive_wan(id_device, now, dev_state, info);

				}
				else{

					printf("Error: device type not found\n");
					exit(EXIT_FAILURE);
				}

				break;

			case ARRIVE_DISK:

				id_device = ((message_arrive*)content)->header.element_id;
				index_map = idmap_search(state->element_to_index, id_device, state->num_devices);
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;

				//info = malloc(sizeof(job_info));
				//memcpy(info, &(((message_arrive*)content)->info), sizeof(job_info));
				copy_info = ((message_arrive*)content)->info;
				info = &copy_info;

				info->arrived_in_node_timestamp = now;
				info->deadline = now;

				arrive_disk(id_device, now, dev_state, info, me);

				break;


			case FINISH:

				id_device = ((message_finish*)content)->header.element_id;
				index_map = idmap_search(state->element_to_index, id_device, state->num_devices);
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;

				if(dev_state->type == NODE){

					finish_node(id_device, now, dev_state, me);

				}
				else if(dev_state->type == ACTUATOR){

					finish_actuator(id_device, now, dev_state, me);

				}
				else if(dev_state->type == LAN){

					finish_lan(id_device, now, dev_state, ((message_finish*)content)->direction, me);

				}
				else{

					printf("Error: device type not found\n");
					exit(EXIT_FAILURE);
				}

				break;

			case FINISH_DISK:

				id_device = ((message_finish*)content)->header.element_id;
				index_map = idmap_search(state->element_to_index, id_device, state->num_devices);
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;

				finish_disk(id_device, now, dev_state, me);

				break;

			case UPDATE_TIMESTAMP:

				id_device = ((message_update*)content)->header.element_id;
				index_map = idmap_search(state->element_to_index, id_device, state->num_devices);
				dev_state = state->devices_array[index_map];

				dev_state->device_timestamp = now;
								
				//########################
				//check if we return in UPDATE_TIMESTAMP after returning true
				//########################
				
				unsigned int num_nodes = GET_TOTAL_NODES(state->general_topology);
				//unsigned int num_sensors = GET_SENSOR_NODES(snapshot->general_topology);
				unsigned int num_actuators = GET_ACTUATOR_NODES(state->general_topology);
				//unsigned int num_wans = GET_NUMBER_OF_WANS(snapshot->general_topology);
				unsigned int num_lans = GET_NUMBER_OF_LANS(state->general_topology);

				int total_number_of_elements = num_nodes + num_actuators + num_lans;
				
				if(state->num_stable_elements == total_number_of_elements) //it means that also me send a broadcast_message
					break;
				
				//########################


				if(dev_state->device_timestamp > TRANSITION_TIME_LIMIT){

					int boolean_check = 0;

					if(dev_state->type == NODE){

						if(dev_state->info.node->type == CENTRAL)
							boolean_check = check_metrics(dev_state->info.node->queue_state, CHECK_TTB, MIN_NUMBER_OF_EVENTS_ALL);
						else if(dev_state->info.node->type == REGIONAL)
							boolean_check = check_metrics(dev_state->info.node->queue_state, CHECK_TTCB, MIN_NUMBER_OF_EVENTS_ALL);
						else if(dev_state->info.node->type == LOCAL)
							boolean_check = check_metrics(dev_state->info.node->queue_state, CHECK_TTC, MIN_NUMBER_OF_EVENTS_ALL);

						if(dev_state->simulation_completed == SIMULATION_ACTIVE)
							update_stable_metrics(dev_state->info.node->queue_state);


						if(dev_state->info.node->type == CENTRAL){
							boolean_check = check_metrics(dev_state->info.node->disk_state, CHECK_TTB, MIN_NUMBER_OF_EVENTS_DISK);
							if(dev_state->simulation_completed == SIMULATION_ACTIVE)
								update_stable_metrics(dev_state->info.node->disk_state);
						}

					}
					else if(dev_state->type == ACTUATOR){

						boolean_check = check_metrics(dev_state->info.actuator->queue_state, CHECK_C, MIN_NUMBER_OF_EVENTS_ALL);
						if(dev_state->simulation_completed == SIMULATION_ACTIVE)
							update_stable_metrics(dev_state->info.actuator->queue_state);

					}
					else if(dev_state->type == LAN){

						boolean_check = check_metrics(dev_state->info.lan->queue_state_in, CHECK_C, MIN_NUMBER_OF_EVENTS_ALL);
						if(dev_state->simulation_completed == SIMULATION_ACTIVE)
							update_stable_metrics(dev_state->info.lan->queue_state_in);
						boolean_check = check_metrics(dev_state->info.lan->queue_state_out, CHECK_TT, MIN_NUMBER_OF_EVENTS_ALL);
						if(dev_state->simulation_completed == SIMULATION_ACTIVE)
							update_stable_metrics(dev_state->info.lan->queue_state_out);

					}
					else{

						boolean_check = 1;
						printf("WARNING: a device that is not a node, actuator, lan receives an update timestamp\n");

					}

					int boolean_simulation_max = 0;

					if(dev_state->device_timestamp > MAX_SIMULATION_TIME){
						boolean_check = 1;
						boolean_simulation_max = 1;
					}

					if(dev_state->stability == ELEMENT_UNSTABLE && boolean_check && dev_state->simulation_completed == SIMULATION_ACTIVE){
						broadcast_message(state->number_lps_enabled, now, STABILITY_ACQUIRED);
						if(!boolean_simulation_max)
							dev_state->stability = ELEMENT_STABLE;
						else
							dev_state->simulation_completed = SIMULATION_STOP;
					}

					if(dev_state->stability == ELEMENT_STABLE && !boolean_check && dev_state->simulation_completed == SIMULATION_ACTIVE){
						broadcast_message(state->number_lps_enabled, now, STABILITY_LOST);
						dev_state->stability = ELEMENT_UNSTABLE;
					}

				}

				schedule_first_update_timestamp(me, now, id_device);

				break;

			case STABILITY_ACQUIRED:

				state->num_stable_elements++;

				break;

			case STABILITY_LOST:

				state->num_stable_elements--;

				break;


	}
}

void print_class_metrics(queue_state * queue_state, FILE * output_file, int i){

	//all data here are averages
	double T = queue_state->actual_timestamp_stable[i] - queue_state->start_timestamp[i];
	double S = queue_state->B_stable[i] / queue_state->C_stable[i];
	double R = queue_state->W_stable[i] / queue_state->C_stable[i];
	//double N = queue_state->W_stable[i] / T;
	double U = queue_state->B_stable[i] / T;
	double lambda = queue_state->A_stable[i] / T;
	//double X = queue_state->C_stable[i] / T;

	if (isnan(-S))
		S = 0.0;
	if (isnan(-R))
		R = 0.0;
	if (isnan(-U))
		U = 0.0;
	if (isnan(-lambda))
		lambda = 0.0;
	fprintf(output_file, "\"service_demand\": %.3g,", S);
	fprintf(output_file, "\"response_time\": %.3g,", R);
	fprintf(output_file, "\"utilization_factor\": %.3g,", U);
	fprintf(output_file, "\"lambda_in\": %.3g", lambda);

}

void print_metrics(queue_state * queue_state, FILE * output_file){

	fprintf(output_file, "\"telemetry\": {");
	print_class_metrics(queue_state, output_file, TELEMETRY);
	fprintf(output_file, "},");
	fprintf(output_file, "\"transition\": {");
	print_class_metrics(queue_state, output_file, TRANSITION);
	fprintf(output_file, "},");
	fprintf(output_file, "\"command\": {");
	print_class_metrics(queue_state, output_file, COMMAND);
	fprintf(output_file, "},");
	fprintf(output_file, "\"batch\": {");
	print_class_metrics(queue_state, output_file, BATCH_DATA);
	fprintf(output_file, "}");

}

bool OnGVT(int me, lp_state *snapshot)
{
	if(snapshot->lp_enabled == LP_DISABLED){
		return true;
	}
	if(snapshot->lp_enabled == LP_SETUP){
		return false;
	}

	unsigned int num_nodes = GET_TOTAL_NODES(snapshot->general_topology);
	//unsigned int num_sensors = GET_SENSOR_NODES(snapshot->general_topology);
	unsigned int num_actuators = GET_ACTUATOR_NODES(snapshot->general_topology);
	//unsigned int num_wans = GET_NUMBER_OF_WANS(snapshot->general_topology);
	unsigned int num_lans = GET_NUMBER_OF_LANS(snapshot->general_topology);

	int total_number_of_elements = num_nodes + num_actuators + num_lans;

	int index;
	int index_map;
	int id_device;
	idmap map;
	device_state * dev_state;
	
	//for(index = 0; index < snapshot->num_devices; index++){

		map = snapshot->element_to_index[0];
		id_device = map.id;
		index_map = map.content;
		dev_state = snapshot->devices_array[index_map];
		if((((long int)dev_state->device_timestamp/100) % 500) == 20)
			printf("LP: %d -> %f\n", me, dev_state->device_timestamp);
	
		//printf("%f\n", dev_state->device_timestamp);
		//break;
	//}
	
		
	if(snapshot->num_stable_elements == total_number_of_elements){
#ifdef PRINT_RESULTS
		sprintf(file_name_complete, "%s%d%s", file_name, me, end_file_name);
		
		FILE * output_file = fopen(file_name_complete, "r");
		if(output_file != NULL){
			fclose(output_file);
			return true;
		}
		
		output_file = fopen(file_name_complete, "w");

		//FILE * output_file = fopen(file_name_complete, "w");

		fprintf(output_file, "[");


		for(index = 0; index < snapshot->num_devices; index++){

			map = snapshot->element_to_index[index];
			id_device = map.id;
			index_map = map.content;
			dev_state = snapshot->devices_array[index_map];

			if(index>0 &&  (dev_state->type == NODE || dev_state->type == LAN  || dev_state->type == ACTUATOR))
				fprintf(output_file, ",");
			if(dev_state->type == NODE){

				fprintf(output_file, "{\"id\": %d,", id_device);
				fprintf(output_file, "\"sim_time\": %f,",dev_state->device_timestamp);
				fprintf(output_file, "\"stable\": %d,", dev_state->stability);
				fprintf(output_file, "\"type\": \"node\",");
				fprintf(output_file, "\"parameters\": {");
				print_metrics(dev_state->info.node->queue_state, output_file);
				fprintf(output_file, "},");
				if(GET_NODE_TYPE(dev_state->topology) == CENTRAL){

					fprintf(output_file, "\"storage\": {");
					print_metrics(dev_state->info.node->disk_state, output_file);
					fprintf(output_file, "},");
					fprintf(output_file, "\"node_type\": \"central\"}");
				}
				else if(GET_NODE_TYPE(dev_state->topology) == REGIONAL){
					fprintf(output_file, "\"node_type\": \"regional\"}");
				}
				else if(GET_NODE_TYPE(dev_state->topology) == LOCAL){
					fprintf(output_file, "\"node_type\": \"local\"}");
				}

			}
			else if(dev_state->type == ACTUATOR){

				fprintf(output_file, "{\"id\": %d,", id_device);
				fprintf(output_file, "\"sim_time\": %f,",dev_state->device_timestamp);
				fprintf(output_file, "\"stable\": %d,", dev_state->stability);
				fprintf(output_file, "\"type\": \"actuator\",");
				fprintf(output_file, "\"parameters\": {");
				print_metrics(dev_state->info.actuator->queue_state, output_file);
				fprintf(output_file, "},");
				fprintf(output_file, "\"node_type\": \"\"}");

			}
			else if(dev_state->type == LAN){

				fprintf(output_file, "{\"id\": %d,", id_device);
				fprintf(output_file, "\"sim_time\": %f,",dev_state->device_timestamp);
				fprintf(output_file, "\"stable\": %d,", dev_state->stability);
				fprintf(output_file, "\"type\": \"lan\",");
				fprintf(output_file, "\"lan_in\": {");
				print_metrics(dev_state->info.lan->queue_state_in, output_file);
				fprintf(output_file, "},");

				fprintf(output_file, "\"lan_out\": {");
				print_metrics(dev_state->info.lan->queue_state_out, output_file);
				fprintf(output_file, "},");
				fprintf(output_file, "\"node_type\": \"\"}");

			}

		}
		fprintf(output_file, "]");
		fflush(output_file);
		fclose(output_file);
#endif

		return true;

	}

	return false;

}
