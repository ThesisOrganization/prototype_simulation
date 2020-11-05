#include "../ROOT-Sim-bin/include/ROOT-Sim.h"
#include <stdio.h>
#define EVENT 1
#define ARRIVE 2
#define FINISH 3
#define TOTAL_NUMBER_OF_EVENTS 100
#define DELAY_MEAN 1
#define ARRIVE_RATE 10
#define FINISH_RATE 10
#define LEN_QUEUE 50

typedef struct _state_type {
    //int executed_events;
    int *queue;
    int index_insert;
    int index_remove;
    int num_clients;
    //info for statistics and termination
    int tot_num_clients;
    int sum_all_queue_number;
    int num_of_sums;
    int max_queue_len;
} state_type;

typedef struct _client_info {
    int id;
} client_info;

void ProcessEvent(unsigned int me, simtime_t now, unsigned int event_type, void *content, int size, state_type *state)
{

    unsigned int my_up_node[3] = {1, 2, -1};

    simtime_t ts_arrive = now + (ARRIVE_RATE * Poisson());
    simtime_t ts_finish = now + (FINISH_RATE * Poisson());
    simtime_t ts_delay = now + (DELAY_MEAN * Poisson());

    //simtime_t timestamp = now + 10 * Random();
    //unsigned int receiver = (unsigned int)(n_prc_tot * Random());
    int event_id_value;
    int up_node;

    switch(event_type) {
        case INIT:

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

            break;

        case ARRIVE:
            event_id_value = ((client_info*)content)->id;

            /*
            if(!me)
                printf("Client %d started\n", event_id_value);
            printf("Client %d arrived to queue: %d\n", event_id_value, me);
            */

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

            break;

        case FINISH:
            //printf("Client %d finish in queue: %d\n", state->queue[state->index_remove], me);

            //give the finished client to the next queue or terminate it
            up_node = my_up_node[me];
            if(up_node != -1){
                client_info *new_event = malloc(sizeof(client_info));
                new_event->id = state->queue[state->index_remove];
                ScheduleNewEvent(up_node, ts_delay, ARRIVE, new_event, sizeof(client_info));
            }/*
            else{
                printf("Client %d terminated\n", state->queue[state->index_remove]);
            }*/

            //to terminate the simulation
            state->tot_num_clients++;

            //remove the actual client from the queue
            state->index_remove = (state->index_remove + 1) % LEN_QUEUE;
            state->num_clients--;

            //start job for the next client in the queue if present
            if(state->num_clients > 0)
                ScheduleNewEvent(me, ts_finish, FINISH, NULL, 0);

            break;
        /*
        case EVENT:
            state->executed_events++;
            ScheduleNewEvent(me, timestamp, EVENT, NULL, 0);
            break;
*/
    }
}

bool OnGVT(int me, state_type *snapshot)
{
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
}
