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
#define FINISH_RATE 10
#define LEN_QUEUE 50

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
    
    //simtime_t timestamp = now + 10 * Random();
    //unsigned int receiver = (unsigned int)(n_prc_tot * Random());
    //int event_id_value;
    int up_node;

    job_info * info;
    
    switch(event_type) {
        case INIT:

            state = malloc(sizeof(state));
            SetState(state);

            //initializza strutture (if sensors and ecc)
            if(my_type[me] == NODE){
                
                state->node = malloc(sizeof(node_state));
                state->node->num_jobs = 0;
                state->node->num_jobs_processed = 0;
                state->node->queues = create_queue();

            }
            else if(my_type[me] == SENSOR){
                
                state->sensor = malloc(sizeof(sensor_state));
                state->sensor->num_jobs_generated = 0;
                state->sensor->job_generated = my_job_type[me];

                //schedule generate for all sensors
                ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

            }

            /*
            state = malloc(sizeof(state_type));
            SetState(state);
            //state->executed_events = 0;
            //ScheduleNewEvent(me, timestamp, EVENT, NULL, 0);

            state->queue = malloc(sizeof(int)*LEN_QUEUE);
            state->index_insert = 0;
            state->index_remove = 0;
            state->num_clients = 0;
            
            state->tot_num_clients = 0;
            state->sum_all_queue_number = 0;
            state->num_of_sums = 0;
            state->max_queue_len = 0;

            //start the first job for the first queue
            if(!me){
                client_info *event = malloc(sizeof(client_info));
                event->id = 0;
                ScheduleNewEvent(me, ts_arrive, ARRIVE, event, sizeof(client_info));
            }
            */

            break;

        case GENERATE:
            
            //check number of events is up to
            if(state->sensor->num_jobs_generated <= TOTAL_NUMBER_OF_EVENTS){

                info = malloc(sizeof(job_info));
                info->type = my_job_type[me];
                info->timestamp = RandomRange(1, 10); //should be random, like now + random

                ScheduleNewEvent(my_up_node[me], ts_delay, ARRIVE, info, sizeof(job_info));

                ScheduleNewEvent(me, ts_arrive, GENERATE, NULL, 0);

                state->sensor->num_jobs_generated++;

            }

            break;

        case ARRIVE:
            
            //content has job type
            
            //schedule finish if queue empty
            if(state->node->num_jobs < 1)
                ScheduleNewEvent(me, ts_finish, FINISH, content, sizeof(job_info));

            //add in the right queue
            enqueue(state->node->queues, ((job_info*) content)->timestamp, content);

            //update statistics
            state->node->num_jobs++;

            

            //event_id_value = ((client_info*)content)->id;
            
            /*
            if(!me)
                printf("Client %d started\n", event_id_value);
            printf("Client %d arrived to queue: %d\n", event_id_value, me);
            */
           /* 
            //if the queue is empty start the first job that will be added soon
            if(state->num_clients < 1)
                ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);

            //add new client in the queue
            state->queue[state->index_insert] = event_id_value;
            state->index_insert = (state->index_insert + 1) % LEN_QUEUE;
            state->num_clients++;
            
            //schedule the next arrive event for the first queue
            if(!me){
                client_info *new_event = malloc(sizeof(client_info));
                new_event->id = event_id_value + 1;
                ScheduleNewEvent(me, ts_arrive, ARRIVE, new_event, sizeof(client_info));
            }
            */
            break;

        case FINISH:
            
            //send arrive to next LP
            
            info = dequeue(state->node->queues);
            
            up_node = my_up_node[me];
            if(up_node != -1){
                ScheduleNewEvent(up_node, ts_delay, ARRIVE, info, sizeof(job_info));
            }
            
            //change state
            state->node->num_jobs_processed++;
            state->node->num_jobs--;
            
            //schedule new event if other are presents (num clients)
            if(state->node->num_jobs > 0)
                ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);
            
            

            //printf("Client %d finish in queue: %d\n", state->queue[state->index_remove], me);

            //give the finished client to the next queue or terminate it
            /*
            up_node = my_up_node[me];
            if(up_node != -1){
                client_info *new_event = malloc(sizeof(client_info));
                new_event->id = state->queue[state->index_remove];
                ScheduleNewEvent(up_node, ts_delay, ARRIVE, new_event, sizeof(client_info));
            }
            
            //to terminate the simulation
            state->tot_num_clients++;
            
            //remove the actual client from the queue
            state->index_remove = (state->index_remove + 1) % LEN_QUEUE;
            state->num_clients--;
        
            //start job for the next client in the queue if present
            if(state->num_clients > 0)
                ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);
*/
            break;

    }
}

bool OnGVT(int me, state *snapshot)
{
        unsigned int my_type[3] = {SENSOR, NODE, NODE}; //0 sensor, 1 node

        if(my_type[me] == NODE){

            printf("Number of elements in the queue: %d\n", snapshot->node->num_jobs);
                
            if(snapshot->node->num_jobs_processed > TOTAL_NUMBER_OF_EVENTS)
                return true;

        }
        else if(my_type[me] == SENSOR){
                
            if(snapshot->sensor->num_jobs_generated > TOTAL_NUMBER_OF_EVENTS)
                return true;

        }
        else
            return true;

    return false;

    /*
    int num_c = snapshot->num_clients;
    snapshot->sum_all_queue_number += num_c;
    snapshot->num_of_sums++;
    if(num_c > snapshot->max_queue_len)
        snapshot->max_queue_len = num_c;
    
    float mean = (float) snapshot->sum_all_queue_number / snapshot->num_of_sums;
    printf("Average number of clients in queue %d: %f\n", me, mean);
    printf("Max number of clients in queue %d: %d\n", me, snapshot->max_queue_len);
    printf("Number of clients in queue %d: %d\n", me, num_c);

    if (snapshot->tot_num_clients >= TOTAL_NUMBER_OF_EVENTS){
        return true;
    }
    return false;
    */
}
