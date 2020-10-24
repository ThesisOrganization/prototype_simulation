//#include <ROOT-Sim.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "./simulation_functions/init_event/init_functions.h"
#include "./simulation_functions/arrive_event/arrive_functions.h"
#include "./simulation_functions/finish_event/finish_functions.h"
#include "./simulation_functions/setup_protocol/setup_protocol.h"

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


void ProcessEvent(unsigned int me, simtime_t now, unsigned int event_type, void *content, int size, lp_state * state)
{


    job_info info_to_send;
    int up_node;
    job_info * info;
    double rate_generate;
    simtime_t ts_update_timestamp;

    switch(event_type) {

        case INIT:
            
            if(me == 0){
                
                //MASTER CODE
                ScheduleNewEvent(me, TS_START_SIMULATION, START_SIMULATION, NULL, 0);
                

            }
            else
                ScheduleNewEvent(me, TS_START_SIMULATION, START_SIMULATION, NULL, 0);

            break;

        case RECEIVE_INFO:

            break;

        case RECEIVE_DATA:

            break;

        case START_SIMULATION:
            state = malloc(sizeof(lp_state));
            SetState(state);
    
            //both timestamp are initialized at 0.0 in theory
            //state->start_timestamp = now; 
            //state->actual_timestamp = now;
            state->device_timestamp = now;
            //printf("%.3g\n", now);

            //state->num_jobs_processed = 0;
            total_topology * tot_top = getTopology(topology_path); //later we will use a static struct
            general_topology * gen_top = getGenTopology(tot_top);
            state->topology = getLPTopology(tot_top, me);

            unsigned int num_nodes = GET_TOTAL_NODES(gen_top);
            unsigned int num_sensors = GET_SENSOR_NODES(gen_top);
            unsigned int num_actuators = GET_ACTUATOR_NODES(gen_top);
            unsigned int num_wans = GET_NUMBER_OF_WANS(gen_top);
            unsigned int num_lans = GET_NUMBER_OF_LANS(gen_top);

            state->num_acts_types = GET_NUMBER_ACT_TYPES(gen_top);
            state->prob_actuators = GET_PROB_ACTUATORS(gen_top);

            //if there are too few LPs, exit
            if(num_nodes + num_sensors + num_actuators + num_lans + num_wans > n_prc_tot){
                printf("Error: too few LPs, add more LPs\n");
                exit(EXIT_FAILURE);
            }


            //if there are too may LPs, return it
            if(me >= num_nodes + num_sensors + num_actuators + num_lans + num_wans){
                //state->num_jobs_processed = TOTAL_NUMBER_OF_EVENTS + 1;
                state->lp_enabled = 0;
                break;
            }
            else
                state->lp_enabled = 1;


            state->type = GET_TYPE(state->topology);
            //printf("%d\n", state->type);
            //lp_infos* infos = getInfo(state->topology, me);

            //initializza strutture
            if(state->type == NODE){

                init_node(me, state);

            }
            else if(state->type == SENSOR){

                init_sensor(me, now, state);

            }
            else if(state->type == ACTUATOR){

                init_actuator(me, now, state);

            }
            else if(state->type == LAN){

                init_lan(me, state);

            }
            else if(state->type == WAN){

                init_wan(me, state);

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            ts_update_timestamp = now + NEXT_UPDATE_TIMESTAMP;
            ScheduleNewEvent(me, ts_update_timestamp, UPDATE_TIMESTAMP, NULL, 0);


            break;

        case GENERATE_TRANSITION:

            //state->actual_timestamp = now;
            state->device_timestamp = now;

            info_to_send.type = REAL_TIME;
            //info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
            info_to_send.payload = NULL;
            info_to_send.job_type = TRANSITION;

            up_node = GET_UPPER_NODE(state->topology);
            ScheduleNewEvent(up_node, now, ARRIVE, &info_to_send, sizeof(job_info));

            //ts_generate = now + Expent(ARRIVE_RATE);
            if(state->type == SENSOR){

                rate_generate = state->info.sensor->rate_transition;

            }
            else if(state->type == ACTUATOR){

                rate_generate = state->info.actuator->rate_transition;

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            generate_next_job(me, now, rate_generate, 0.0, GENERATE_TRANSITION);

            //state->num_jobs_processed++;
            //printf("%d\n", state->num_jobs_processed);
            break;

        case GENERATE_TELEMETRY:

            //state->actual_timestamp = now;
            state->device_timestamp = now;

            info_to_send.type = REAL_TIME;
            //info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
            info_to_send.payload = NULL;
            info_to_send.job_type = TELEMETRY;

            up_node = GET_UPPER_NODE(state->topology);
            ScheduleNewEvent(up_node, now, ARRIVE, &info_to_send, sizeof(job_info));

            //ts_generate = now + Expent(ARRIVE_RATE);
            if(state->type == SENSOR){

                rate_generate = state->info.sensor->rate_telemetry;

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            generate_next_job(me, now, rate_generate, 0.0, GENERATE_TELEMETRY);

            //state->num_jobs_processed++;
            break;

        case ARRIVE:

            //state->actual_timestamp = now;
            state->device_timestamp = now;

            info = malloc(sizeof(job_info));
            memcpy(info, content, sizeof(job_info));

            info->arrived_in_node_timestamp = now;
            info->deadline = now;

            if(state->type == NODE){

                arrive_node(me, now, state, info);

            }
            else if(state->type == ACTUATOR){

                arrive_actuator(me, now, state, info);

            }
            else if(state->type == LAN){

                arrive_lan(me, now, state, info);

            }
            else if(state->type == WAN){

                arrive_wan(me, now, state, info);

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            break;

        case ARRIVE_DISK:
            
            //state->actual_timestamp = now;
            state->device_timestamp = now;

            info = malloc(sizeof(job_info));
            memcpy(info, content, sizeof(job_info));

            info->arrived_in_node_timestamp = now;
            info->deadline = now;

            arrive_disk(me, now, state, info);

            break;


        case FINISH:

            //state->actual_timestamp = now;
            state->device_timestamp = now;

            //state->num_jobs_processed++;

            if(state->type == NODE){

                finish_node(me, now, state);

            }
            else if(state->type == ACTUATOR){

                finish_actuator(me, now, state);

            }
            else if(state->type == LAN){

                finish_lan(me, now, state, *( (lan_direction *) content));

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            break;

        case FINISH_DISK:

            //state->actual_timestamp = now;
            state->device_timestamp = now;

            finish_disk(me, now, state);

            break;

        case UPDATE_TIMESTAMP:
            
            state->device_timestamp = now;
            
            ts_update_timestamp = now + NEXT_UPDATE_TIMESTAMP;
            ScheduleNewEvent(me, ts_update_timestamp, UPDATE_TIMESTAMP, NULL, 0);

            break;


    }
}

void print_class_metrics(queue_state * queue_state, FILE * output_file, int i){

    //fprintf(output_file, "......................\n");
    //fprintf(output_file, "Class number %d\n", i);
    
    //all data here are averages
    double T = queue_state->actual_timestamp[i] - queue_state->start_timestamp[i];
    double S = queue_state->B[i] / queue_state->C[i];
    double R = queue_state->W[i] / queue_state->C[i];
    double N = queue_state->W[i] / T;
    double U = queue_state->B[i] / T;
    double lambda = queue_state->A[i] / T;
    double X = queue_state->C[i] / T;
    
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


    //for(int i=0; i < NUM_OF_JOB_TYPE; i++){

        //if(queue_state->actual_timestamp[i] > TRANSITION_TIME_LIMIT){

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
        
        //}


    //}
}
/*
void print_pre(int me, simtime_t device_timestamp, int device_type, int node_type, FILE * output_file){

    fprintf(output_file, "#################################################\n");
    fprintf(output_file, "Device number: %d, Type: %d, Node type: %d, timestamp: %f\n", me, device_type, node_type, device_timestamp);

}
*/
bool OnGVT(int me, lp_state *snapshot)
{

    if(!snapshot->lp_enabled)
        return true;

    if(snapshot->device_timestamp > MAX_SIMULATION_TIME){


    
        if(snapshot->type == NODE){
    
#ifdef PRINT_RESULTS
            sprintf(file_name_complete, "%s%d%s", file_name, me, end_file_name);

            FILE * output_file = fopen(file_name_complete, "w");
            //print_pre(me, snapshot->device_timestamp, snapshot->type, snapshot->info.node->type, output_file);

            fprintf(output_file, "[");

            fprintf(output_file, "{\"id\": %d,", me);
            fprintf(output_file, "\"type\": \"node\",", me);
            fprintf(output_file, "\"parameters\": {");
            print_metrics(snapshot->info.node->queue_state, output_file);
            fprintf(output_file, "},");
            if(GET_NODE_TYPE(snapshot->topology) == CENTRAL){
                //fprintf(output_file, "<<<<<<<<<<<<<<<<<<<<\n");
                //fprintf(output_file, "Disk:\n");
                fprintf(output_file, "\"storage\": {");
                print_metrics(snapshot->info.node->disk_state, output_file);
                fprintf(output_file, "},");
                fprintf(output_file, "\"node_type\": \"central\"}");
            }
            else if(GET_NODE_TYPE(snapshot->topology) == REGIONAL){
                fprintf(output_file, "\"node_type\": \"regional\"}");
            }
            else if(GET_NODE_TYPE(snapshot->topology) == LOCAL){
                fprintf(output_file, "\"node_type\": \"local\"}");
            }

            fprintf(output_file, "]");

            fclose(output_file);
#endif
    
        }
        else if(snapshot->type == ACTUATOR){
    
#ifdef PRINT_RESULTS
            sprintf(file_name_complete, "%s%d%s", file_name, me, end_file_name);

            FILE * output_file = fopen(file_name_complete, "w");

            fprintf(output_file, "[");

            //print_pre(me, snapshot->device_timestamp, snapshot->type, -1, output_file);
            fprintf(output_file, "{\"id\": %d,", me);
            fprintf(output_file, "\"type\": \"actuator\",", me);
            fprintf(output_file, "\"parameters\": {");
            print_metrics(snapshot->info.actuator->queue_state, output_file);
            fprintf(output_file, "},");
            fprintf(output_file, "\"node_type\": \"\"}");
            
            fprintf(output_file, "]");

            fclose(output_file);
#endif
    
        }
        else if(snapshot->type == LAN){
    
#ifdef PRINT_RESULTS
            sprintf(file_name_complete, "%s%d%s", file_name, me, end_file_name);

            FILE * output_file = fopen(file_name_complete, "w");

            fprintf(output_file, "[");

            //print_pre(me, snapshot->device_timestamp, snapshot->type, -1, output_file);
    
            //fprintf(output_file, "<<<<<<<<<<<<<<<<<<<<\n");
            //fprintf(output_file, "Lan IN:\n");
            fprintf(output_file, "{\"id\": %d,", me);
            fprintf(output_file, "\"type\": \"lan\",", me);
            fprintf(output_file, "\"lan_in\": {");
            print_metrics(snapshot->info.lan->queue_state_in, output_file);
            fprintf(output_file, "},");
            //fprintf(output_file, "<<<<<<<<<<<<<<<<<<<<\n");
            //fprintf(output_file, "Lan OUT:\n");
            fprintf(output_file, "\"lan_out\": {");
            print_metrics(snapshot->info.lan->queue_state_out, output_file);
            fprintf(output_file, "},");
            fprintf(output_file, "\"node_type\": \"\"}");

            fprintf(output_file, "]");

            fclose(output_file);
#endif
    
        }

        snapshot->lp_enabled = 0;
    
    }

    return false;

}
