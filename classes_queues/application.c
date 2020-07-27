#include <ROOT-Sim.h>
#include <stdio.h>
#include "application.h"
#include "../utils/priority_queue/priority_queue.h"

#define EVENT 1
#define ARRIVE 2
#define FINISH 3
#define GENERATE 4
#define TOTAL_NUMBER_OF_EVENTS 100
#define DELAY_MEAN 1
#define ARRIVE_RATE 10
#define FINISH_RATE 5
#define LEN_QUEUE 50

#define RANGE_TIMESTAMP 10

#define SENSOR 0
#define NODE 1


void ProcessEvent(unsigned int me, simtime_t now, unsigned int event_type, void *content, int size, state * state)
{
    unsigned int my_type[3] = {SENSOR, NODE, NODE}; //0 sensor, 1 node
    unsigned int my_job_type[1] = {REAL_TIME};
    unsigned int my_up_node[3] = {1, 2, -1};

    simtime_t ts_arrive = now + (ARRIVE_RATE * Poisson());
    simtime_t ts_finish = now + (FINISH_RATE * Poisson());
    simtime_t ts_delay = now + (DELAY_MEAN * Poisson());
    
    int up_node;

    job_info * info;
    
    switch(event_type) {
        case INIT:
            state = malloc(sizeof(state));
            SetState(state);

            //state->info = malloc(sizeof(state_info));
            state->num_jobs_processed = 0;
            
            //initializza strutture (if sensors and ecc)
            if(my_type[me] == NODE){
                
                state->info.node = malloc(sizeof(node_state));
                state->info.node->num_jobs_in_queue = 0;
                state->info.node->queues = create_queue();
            }
            else if(my_type[me] == SENSOR){

                state->info.sensor = malloc(sizeof(sensor_state));
                state->info.sensor->job_generated = my_job_type[me];
                
                //schedule generate for all sensors
                ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

            }


            break;

        case GENERATE:
            
            //check number of events is up to
            if(state->num_jobs_processed <= TOTAL_NUMBER_OF_EVENTS){

                info = malloc(sizeof(job_info));
                info->type = my_job_type[me];
                info->timestamp = now + (Random() * RANGE_TIMESTAMP); //should be random, like now + random

                ScheduleNewEvent(my_up_node[me], ts_delay, ARRIVE, info, sizeof(job_info));

                ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

                state->num_jobs_processed++;

            }

            break;

        case ARRIVE:
            
            //content has job type
            
            //schedule finish if queue empty
            if(state->info.node->num_jobs_in_queue < 1)
                ScheduleNewEvent(me, ts_finish, FINISH, content, sizeof(job_info));

            //add in the right queue
            enqueue(state->info.node->queues, ((job_info*) content)->timestamp, content);

            //update statistics
            state->info.node->num_jobs_in_queue++;

            
            break;

        case FINISH:
            
            //send arrive to next LP
            
            info = dequeue(state->info.node->queues);
            
            up_node = my_up_node[me];
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

bool OnGVT(int me, state *snapshot)
{
        if(snapshot->num_jobs_processed > TOTAL_NUMBER_OF_EVENTS)
            return true;

        unsigned int my_type[3] = {SENSOR, NODE, NODE}; //0 sensor, 1 node

        if(my_type[me] == NODE){

            printf("Number of elements in the queue: %d\n", snapshot->info.node->num_jobs_in_queue);
        }

    return false;

}
