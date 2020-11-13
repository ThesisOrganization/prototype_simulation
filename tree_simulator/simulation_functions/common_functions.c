#include "common_functions.h"

void fill_job_info(job_info * info, double arrived_in_node_timestamp, double deadline, job_type job_type, int device_sender, double busy_time_transition, double waiting_time_transition, int device_destination){
	
	info->type = REAL_TIME;
	info->arrived_in_node_timestamp = arrived_in_node_timestamp;
	info->deadline = deadline;
	info->job_type = job_type;
	info->device_sender = device_sender;
	info->busy_time_transition = busy_time_transition;
	info->waiting_time_transition = waiting_time_transition;
	info->device_destination = device_destination;
	
}
