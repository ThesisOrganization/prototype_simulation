#include <ROOT-Sim.h>
#include <stdio.h>
#include <string.h>

#include "application.h"
#include "../utils/priority_queue/priority_queue.h"
#include "../utils/priority_scheduler/priority_scheduler.h"
#include "../utils/partop/header.h"

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

void * parse_strings(char ** strings){
    lp_infos * infos = malloc(sizeof(lp_infos));
    
    if( !strcmp(strings[0], "NODE\n") ){

        infos->lp_type = NODE;

    }
    else if( !strcmp(strings[0], "SENSOR") ){

        infos->lp_type = SENSOR;
        if( !strcmp(strings[1], "REAL_TIME\n") )
            infos->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY\n") )
            infos->type_job = LOSSY;
        else if( !strcmp(strings[1], "BATCH\n") )
            infos->type_job = BATCH;
        else
            exit(EXIT_FAILURE);

    }
    else{
        exit(EXIT_FAILURE);
    }
    
    return infos;
}

char topology_path[] = "./topology.txt";

void ProcessEvent(unsigned int me, simtime_t now, unsigned int event_type, void *content, int size, lp_state * state)
{

    simtime_t ts_arrive = now + (ARRIVE_RATE * Poisson());
    simtime_t ts_finish = now + (FINISH_RATE * Poisson());
    simtime_t ts_delay = now + (DELAY_MEAN * Poisson());
    
    int up_node;

    job_info * info;
    
    switch(event_type) {
        case INIT:
            state = malloc(sizeof(lp_state));
            SetState(state);
            
            state->num_jobs_processed = 0;
            state->topology = getTopology(topology_path, parse_strings); //later we will use a static struct

            int num_nodes = state->topology->total_nodes;
            int num_sensor = state->topology->sensor_nodes;

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

            
            lp_infos * infos = getInfo(state->topology, me);
            state->type = infos->lp_type;

            //initializza strutture (if sensors and ecc)
            if(state->type == NODE){
                
                state->info.node = malloc(sizeof(node_state));
                state->info.node->num_jobs_in_queue = 0;
                
                int num_queues = NUM_QUEUES;
                state->info.node->queues = new_prio_scheduler(create_new_queues(num_queues), NULL, num_queues, 0, 1, UPGRADE_PRIO);

            }
            else if(state->type == SENSOR){

                state->info.sensor = malloc(sizeof(sensor_state));
                state->info.sensor->job_generated = infos->type_job;
                
                //schedule generate for all sensors
                ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

            }

            break;

        case GENERATE:
            
            //check number of events is up to
            if(state->num_jobs_processed <= TOTAL_NUMBER_OF_EVENTS){

                info = malloc(sizeof(job_info));
                info->type = state->info.sensor->job_generated;
                info->deadline = now + (Random() * RANGE_TIMESTAMP); //should be random, like now + random
                info->payload = NULL;

                up_node = getNext(state->topology, me)[0]; 
                ScheduleNewEvent(up_node, ts_delay, ARRIVE, info, sizeof(job_info));

                ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

                state->num_jobs_processed++;

            }

            break;

        case ARRIVE:
            
            //content has job type
            
            //schedule finish if queue empty
            if(state->info.node->num_jobs_in_queue < 1)
                ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);

            //add in the right queue
            schedule_in(state->info.node->queues, content);

            //update statistics
            state->info.node->num_jobs_in_queue++;

            
            break;

        case FINISH:
            
            //send arrive to next LP
            
            info = schedule_out(state->info.node->queues, now)[0];
            
            up_node = getNext(state->topology, me)[0]; 
            if(up_node != -1){
                ScheduleNewEvent(up_node, ts_delay, ARRIVE, info, sizeof(job_info));
            }
            
            //change state
            state->num_jobs_processed++;
            state->info.node->num_jobs_in_queue--;
            
            //schedule new event if other are presents (num clients)
            if(state->info.node->num_jobs_in_queue > 0)
                ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);
            
            

            break;

    }
}

bool OnGVT(int me, lp_state *snapshot)
{
        if(snapshot->num_jobs_processed > TOTAL_NUMBER_OF_EVENTS)
            return true;

        if(snapshot->type == NODE){

            printf("Number of elements in the queue: %d\n", snapshot->info.node->num_jobs_in_queue);
        }

    return false;

}
