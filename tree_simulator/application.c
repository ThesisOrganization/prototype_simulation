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
    double rate_generate;
    double time_between_arrivals;

    simtime_t ts_generate;
    //simtime_t ts_arrive = now + Expent(ARRIVE_RATE);
    //simtime_t ts_finish = now + Expent(FINISH_RATE);
    //simtime_t ts_delay = now + Expent(DELAY_MEAN);

    switch(event_type) {

        case INIT:
            state = malloc(sizeof(lp_state));
            SetState(state);

            state->start_timestamp = now;
            state->actual_timestamp = now;
            //printf("%.3g\n", now);

            //state->num_jobs_processed = 0;
            state->topology = getTopology(topology_path); //later we will use a static struct

            unsigned int num_nodes = state->topology->total_nodes;
            unsigned int num_sensors = state->topology->sensor_nodes;
            unsigned int num_actuators = state->topology->actuator_nodes;
            unsigned int num_wans = state->topology->numberOfTotalWANs;
            unsigned int num_lans = state->topology->numberOfTotalLANs;

            state->num_acts_types = state->topology->numberOfActTypes;
            state->prob_actuators = state->topology->probOfActuators;

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


            state->type = getType(state->topology, me);
            //printf("%d\n", state->type);
            lp_infos* infos = getInfo(state->topology, me);

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

            state->actual_timestamp = now;
            //check number of events is up to
            info_to_send.type = REAL_TIME;
            //info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
            info_to_send.payload = NULL;
            info_to_send.job_type = TRANSITION;

            up_node = getUpperNode(state->topology, me);
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

            time_between_arrivals = 1/rate_generate;
            ts_generate = now + Expent(time_between_arrivals);
            ScheduleNewEvent(me, ts_generate, GENERATE_TRANSITION, NULL, 0);

            //state->num_jobs_processed++;
            //printf("%d\n", state->num_jobs_processed);
            break;

        case GENERATE_TELEMETRY:

            state->actual_timestamp = now;
            //check number of events is up to
            info_to_send.type = REAL_TIME;
            //info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
            info_to_send.payload = NULL;
            info_to_send.job_type = TELEMETRY;

            up_node = getUpperNode(state->topology, me);
            ScheduleNewEvent(up_node, now, ARRIVE, &info_to_send, sizeof(job_info));

            //ts_generate = now + Expent(ARRIVE_RATE);
            if(state->type == SENSOR){

                rate_generate = state->info.sensor->rate_telemetry;

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            time_between_arrivals = 1/rate_generate;
            ts_generate = now + Expent(time_between_arrivals);
            ScheduleNewEvent(me, ts_generate, GENERATE_TELEMETRY, NULL, 0);

            //state->num_jobs_processed++;
            break;

        case ARRIVE:

            state->actual_timestamp = now;

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
            else if(state->type == WAN){

                arrive_wan(me, now, state, info);

            }
            else{

                printf("Error: device type not found\n");
                exit(EXIT_FAILURE);
            }

            break;


        case FINISH:

            state->actual_timestamp = now;

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

    }
}

void print_metrics(int me, queue_state * queue_state, double T){


    for(int i=0; i < NUM_OF_JOB_TYPE; i++){

        //if(queue_state->A[i] > 0){

            printf("......................\n");
            printf("Class number %d\n", i);

            //all data here are averages
            double S = queue_state->B[i] / queue_state->C[i];
            double R = queue_state->W[i] / queue_state->C[i];
            double N = queue_state->W[i] / T;
            double U = queue_state->B[i] / T;
            double lambda = queue_state->A[i] / T;
            double X = queue_state->C[i] / T;

            printf("Average Service time: %.3g\n", S);
            printf("Average Response time: %.3g\n", R);
            printf("Average number of visits: %.3g\n", N);
            printf("Utilization factor: %.3g\n", U);
            printf("Arrival rate: %.3g\n", lambda);
            printf("Throughput: %.3g\n", X);
        //}


    }
}

void print_pre(int me, simtime_t actual_timestamp){

    printf("#################################################\n");
    printf("Device number: %d, timestamp: %f\n", me, actual_timestamp);

}

bool OnGVT(int me, lp_state *snapshot)
{
    //if(snapshot->num_jobs_processed > TOTAL_NUMBER_OF_EVENTS) //now is used only by the LPs that are not used
    //        return true;

    if(!snapshot->lp_enabled)
        return true;

    double T = snapshot->actual_timestamp - snapshot->start_timestamp;


    //printf("%d\n", me);
    if(snapshot->type == NODE){

        print_pre(me, snapshot->actual_timestamp);
        print_metrics(me, snapshot->info.node->queue_state, T);

    }
    else if(snapshot->type == ACTUATOR){

        print_pre(me, snapshot->actual_timestamp);
        print_metrics(me, snapshot->info.actuator->queue_state, T);

    }
    else if(snapshot->type == LAN){

        print_pre(me, snapshot->actual_timestamp);

        printf("<<<<<<<<<<<<<<<<<<<<\n");
        printf("Lan IN:\n");
        print_metrics(me, snapshot->info.lan->queue_state_in, T);
        printf("<<<<<<<<<<<<<<<<<<<<\n");
        printf("Lan OUT:\n");
        print_metrics(me, snapshot->info.lan->queue_state_out, T);

    }

    return false;

}
