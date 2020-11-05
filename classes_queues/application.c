//#include ../ROOT-Sim-bin/include/ROOT-Sim.h
#include <stdio.h>
#include <string.h>

//#include "application.h"
#include "../utils/priority_queue/priority_queue.h"
#include "../utils/priority_scheduler/priority_scheduler.h"
#include "../utils/partop/header.h"
#include "../utils/application_datatypes.h"

#define EVENT 1
#define ARRIVE 2
#define FINISH 3
#define GENERATE 4
#define TOTAL_NUMBER_OF_EVENTS 100
#define DELAY_MEAN 1
#define ARRIVE_RATE 30
#define FINISH_RATE 5
#define LEN_QUEUE 50

#define RANGE_TIMESTAMP 10

#define NUM_QUEUES 3

queue_conf** create_new_queues(int num_queues){


    queue_conf** queues = malloc(sizeof(queue_conf *) * num_queues);
    for(int i = 0; i < num_queues; i++){
        queues[i] = malloc(sizeof(queue_conf));
        queues[i]->queue = create_queue();
        queues[i]->type = i;
        queues[i]->enqueue = enqueue;
        queues[i]->dequeue = dequeue;
        queues[i]->check_presence = check_presence;
        queues[i]->check_full = NULL;
    }

    return queues;

}

char topology_path[] = "./topology.txt";

void ProcessEvent(unsigned int me, simtime_t now, unsigned int event_type, void *content, int size, lp_state * state)
{

    simtime_t ts_arrive = now + (ARRIVE_RATE * Poisson());
    simtime_t ts_finish = now + (FINISH_RATE * Poisson());
    simtime_t ts_delay = now + (DELAY_MEAN * Poisson());


    int up_node;

    job_info * info,**info_arr;

    job_info info_to_send;
    processing_info proc_info_to_send;

    switch(event_type) {
        case INIT:
            state = malloc(sizeof(lp_state));
            SetState(state);

            state->num_jobs_processed = 0;
            state->topology = getTopology(topology_path); //later we will use a static struct

            unsigned int num_nodes = state->topology->total_nodes;
            unsigned int num_sensor = state->topology->sensor_nodes;

            //if there are too few LPs, exit
            if(num_nodes + num_sensor > n_prc_tot){
                printf("Error: too few LPs, add more LPs\n");
                exit(EXIT_FAILURE);
            }

            //if there are too may LPs, return it
            if(me >= num_nodes + num_sensor){
                state->num_jobs_processed = TOTAL_NUMBER_OF_EVENTS + 1;
                break;
            }


            lp_infos* infos = getInfo(state->topology, me);
            state->type = infos->lp_type;
            state->ts=now;

            //initializza strutture (if sensors and ecc)
            if(state->type == NODE){

                state->info.node = malloc(sizeof(node_state));
                state->info.node->num_jobs_in_queue = 0;
                state->info.node->num_jobs_arrived = 0;
                state->info.node->num_lossy_jobs_rejected=0;
                state->info.node->num_rt_jobs_rejected=0;
                state->info.node->num_batch_jobs_rejected=0;
                state->info.node->last_arrived_in_node_timestamp = 0.0;
                state->info.node->sum_all_service_time = 0.0;
                state->info.node->sum_all_time_between_arrivals = 0.0;
                state->info.node->sum_all_response_time = 0.0;

                int num_queues = NUM_QUEUES;
                state->info.node->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);

            }
            else if(state->type == SENSOR){

                state->info.sensor = malloc(sizeof(sensor_state));
                printf("%d\n", infos->type_job);
                state->info.sensor->job_generated = infos->type_job;

                //schedule generate for all sensors
                ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

            }

            break;

        case GENERATE:
            //update timestamp in the node
            state->ts=now;
            //check number of events is up to
            info_to_send.type = state->info.sensor->job_generated;
            info_to_send.deadline = now + (Random() * RANGE_TIMESTAMP);
            info_to_send.payload = NULL;

            up_node = getNext(state->topology, me)[0];
            ScheduleNewEvent(up_node, ts_delay, ARRIVE, &info_to_send, sizeof(job_info));

            ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

            state->num_jobs_processed++;


            break;

        case ARRIVE:
           //update timestamp in the node
            state->ts=now;

            //schedule finish if queue empty
            if(state->info.node->num_jobs_in_queue < 1){
                //more_info = malloc(sizeof(processing_info));
                proc_info_to_send.start_processing_timestamp = now;
                ScheduleNewEvent(me, ts_finish, FINISH, &proc_info_to_send, sizeof(processing_info));
            }

            //add in the right queue
            info = malloc(sizeof(job_info));
            memcpy(info, content, sizeof(job_info));

            info->arrived_in_node_timestamp = now;
            schedule_in(state->info.node->queues, info);

            //update statistics
            state->info.node->num_jobs_in_queue++;
            state->info.node->num_jobs_arrived++;

            state->info.node->sum_all_time_between_arrivals += now - state->info.node->last_arrived_in_node_timestamp;
            state->info.node->last_arrived_in_node_timestamp = now;

            break;

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

    }
}

bool OnGVT(int me, lp_state *snapshot)
{
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

}
