#include <ROOT-Sim.h>
#include <stdio.h>
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
    //unsigned int my_up_node[3] = {1, 2, -1};

    simtime_t ts_arrive = now + (ARRIVE_RATE * Poisson());
    simtime_t ts_finish = now + (FINISH_RATE * Poisson());
    simtime_t ts_delay = now + (DELAY_MEAN * Poisson());
    
    int up_node;

    job_info * info;
    
    switch(event_type) {
        case INIT:
            state = malloc(sizeof(state));
            SetState(state);
            
            char path[] = "./test.txt";
            
            //state->info = malloc(sizeof(state_info));
            state->num_jobs_processed = 0;
            state->topology = getTopology(path); //later we will use a static struct
            
            //initializza strutture (if sensors and ecc)
            if(my_type[me] == NODE){
                
                state->info.node = malloc(sizeof(node_state));
                state->info.node->num_jobs_in_queue = 0;
                //state->info.node->queues = create_queue();

                int num_queues = 3;
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

                state->info.node->queues = new_prio_scheduler(queues, NULL, num_queues, 0, 1, UPGRADE_PRIO);

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
                ScheduleNewEvent(me, ts_finish, FINISH, content, sizeof(job_info));

            //add in the right queue
            //enqueue(state->info.node->queues, ((job_info*) content)->deadline, content);
            schedule_in(state->info.node->queues, content);

            //update statistics
            state->info.node->num_jobs_in_queue++;

            
            break;

        case FINISH:
            
            //send arrive to next LP
            
            //info = dequeue(state->info.node->queues);
            info = schedule_out(state->info.node->queues, now)[0];
            
            //up_node = my_up_node[me];
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
