//#include <ROOT-Sim.h>
#include <stdio.h>
#include <string.h>

#include "./simulation_functions.h"

char topology_path[] = "./topology.txt";

void ProcessEvent(unsigned int me, simtime_t now, unsigned int event_type, void *content, int size, lp_state * state)
{

    /*
    simtime_t ts_arrive = now + Expent(ARRIVE_RATE);
    simtime_t ts_finish = now + Expent(FINISH_RATE);
    simtime_t ts_delay = now + Expent(DELAY_MEAN);

    int up_node;

    job_info * info,**info_arr;

    job_info info_to_send;
    processing_info proc_info_to_send;
    */

    job_info info_to_send;
    int up_node;
    job_info * info;
    
    simtime_t ts_generate;
    //simtime_t ts_arrive = now + Expent(ARRIVE_RATE);
    //simtime_t ts_finish = now + Expent(FINISH_RATE);
    //simtime_t ts_delay = now + Expent(DELAY_MEAN);

    switch(event_type) {

        case INIT:
            state = malloc(sizeof(lp_state));
            SetState(state);

            state->num_jobs_processed = 0;
            state->topology = getTopology(topology_path); //later we will use a static struct


            unsigned int num_nodes = state->topology->total_nodes;
            unsigned int num_sensors = state->topology->sensor_nodes;
            unsigned int num_actuators = state->topology->actuator_nodes;
            unsigned int num_connections = state->topology->connection_elements;

            
            //if there are too few LPs, exit
            if(num_nodes + num_sensors + num_actuators + num_connections > n_prc_tot){
                printf("Error: too few LPs, add more LPs\n");
                exit(EXIT_FAILURE);
            }

            
            //if there are too may LPs, return it
            if(me >= num_nodes + num_sensors + num_actuators + num_connections){
                state->num_jobs_processed = TOTAL_NUMBER_OF_EVENTS + 1;
                break;
            }
            

            lp_infos* infos = getInfo(state->topology, me);
            state->type = infos->lp_type;


            //initializza strutture 
            if(state->type == NODE){
                
                init_node(me, now, state, infos);

            }
            else if(state->type == SENSOR){

                init_sensor(me, now, state, infos);

            }
            else if(state->type == ACTUATOR){

                init_actuator(me, now, state, infos);

            }
            else if(state->type == LAN){

                init_lan(me, now, state, infos);

            }
            else if(state->type == WAN){

                init_wan(me, now, state, infos);

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            break;

        case GENERATE_TRANSITION:

            //check number of events is up to
            info_to_send.type = REAL_TIME;
            //info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
            info_to_send.payload = NULL;
            info_to_send.job_type = TRANSITION;

            up_node = getUpperNode(state->topology, me);
            ScheduleNewEvent(up_node, now, ARRIVE, &info_to_send, sizeof(job_info));
            
            ts_generate = now + Expent(ARRIVE_RATE);
            ScheduleNewEvent(me, ts_generate, GENERATE_TRANSITION, NULL, 0);

            state->num_jobs_processed++;
            //printf("%d\n", state->num_jobs_processed);
            break;
        
        case GENERATE_TELEMETRY:

            //check number of events is up to
            info_to_send.type = REAL_TIME;
            //info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
            info_to_send.payload = NULL;
            info_to_send.job_type = TELEMETRY;

            up_node = getUpperNode(state->topology, me);
            ScheduleNewEvent(up_node, now, ARRIVE, &info_to_send, sizeof(job_info));

            ts_generate = now + Expent(ARRIVE_RATE);
            ScheduleNewEvent(me, ts_generate, GENERATE_TELEMETRY, NULL, 0);

            state->num_jobs_processed++;
            break;

        case ARRIVE:

            info = malloc(sizeof(job_info));
            memcpy(info, content, sizeof(job_info));
            
            info->arrived_in_node_timestamp = now;

            if(state->type == NODE){
                
                arrive_node(me, now, state, info);

            }
            else if(state->type == ACTUATOR){

                arrive_actuator(me, now, state, info);

            }
            else if(state->type == LAN){

                arrive_lan(me, now, state, info);

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            break;

        /*
        case FINISH:
            //update timestamp in the node
            state->ts=now;

            //send arrive to next LP
            info_arr = schedule_out(state->info.node->queues);
            info=info_arr[0];

            up_node = getNext(state->topology, me)[0];
            //we send a new arrive event if we have a receiver and the job has not expired is is lossy.
            if(up_node != -1 && (info->type==LOSSY && info->deadline>=now)){
                ScheduleNewEvent(up_node, ts_delay, ARRIVE, info, sizeof(job_info));
            }
            //check if the lossy job has been rejected and remeber that
            if(info->type==LOSSY && info->deadline<now){
                state->info.node->num_lossy_jobs_rejected++;
           }

            double service_time = now - ( (processing_info*) content)->start_processing_timestamp;
            double response_time = now - info->arrived_in_node_timestamp;

            //change state
            state->num_jobs_processed++;
            state->info.node->sum_all_service_time += service_time;
            state->info.node->sum_all_response_time += response_time;

            free(info_arr);
            free(info);

            state->info.node->num_jobs_in_queue--;

            //schedule new event if other are presents (num clients)
            if(state->info.node->num_jobs_in_queue > 0){
                proc_info_to_send.start_processing_timestamp = now;
                ScheduleNewEvent(me, ts_finish, FINISH, &proc_info_to_send, sizeof(processing_info));
            }

            break;
            */

        case FINISH:

            if(state->type == NODE){
                
                //arrive_node(me, now, state, info);

            }
            else if(state->type == ACTUATOR){

                //arrive_actuator(me, now, state, info);

            }
            else if(state->type == LAN){

                //arrive_lan(me, now, state, info);

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            break;

    }
}

bool OnGVT(int me, lp_state *snapshot)
{
    if(snapshot->num_jobs_processed > TOTAL_NUMBER_OF_EVENTS)
            return true;
    
    //printf("%d\n", me);
    //return false; //se lo metti va infinito, altrimenti si ferma appena non ci sono eventi?

    /*
        if(snapshot->num_jobs_processed > TOTAL_NUMBER_OF_EVENTS)
            return true;

        if(snapshot->type == NODE){

            printf("################################################\n");
            printf("Node %d timestamp: %f\n",me,snapshot->ts);
            printf("Number of jobs in the node %d: %d\n", me, snapshot->info.node->num_jobs_in_queue);
            double average_processing = snapshot->info.node->sum_all_service_time / snapshot->num_jobs_processed;
            double average_arrivial = snapshot->info.node->sum_all_time_between_arrivals / snapshot->info.node->num_jobs_arrived;
            double average_response = snapshot->info.node->sum_all_response_time / snapshot->num_jobs_processed;

            printf("Average response time: %lf\n", average_response);
            printf("Average arrival rate: %lf\nAverage processing rate: %lf\n", 1/average_arrivial, 1/average_processing);

            double ro = average_processing / average_arrivial;
            printf("Utilization factor in the node %d: %lf\n", me, ro);
            printf("Rejected lossy job in node %d: %d\n",me,snapshot->info.node->num_lossy_jobs_rejected);

        }

    return false;
    */

}
