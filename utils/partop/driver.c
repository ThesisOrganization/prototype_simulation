#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define NODE 1
#define SENSOR 2
#define SCHEDULER1 3
#define SCHEDULER2 4
#define SCHEDULER3 5
#define REAL_TIME 6
#define LOSSY 7
#define BATCH 8

//temp struct, to make things uniform with application.c
//and check integrity in this main file
typedef struct _parsingStruct
{
int lp_type;
int type_job;
int scheduler;
float response_time;
} parsingStruct;

void * parse_strings(char ** strings){
    parsingStruct * infos = malloc(sizeof(parsingStruct));

    if( !strcmp(strings[0], "NODE") ){

        infos->lp_type = NODE;

        if( !strcmp(strings[1], "SCHEDULER1") )
            infos->scheduler = SCHEDULER1;
        else if( !strcmp(strings[1], "SCHEDULER2") )
            infos->scheduler = SCHEDULER2;
        else if( !strcmp(strings[1], "SCHEDULER3") )
            infos->scheduler = SCHEDULER3;
        else
            exit(EXIT_FAILURE);

        //printf("%s\n",strings[2]);
        //printf("%f\n",atof(strings[2]));
        infos->response_time = atof(strings[2]);
        //printf("%f\n",infos->response_time);
    }
    else if( !strcmp(strings[0], "SENSOR") ){

        infos->lp_type = SENSOR;

        if( !strcmp(strings[1], "REAL_TIME") )
            infos->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY") )
            infos->type_job = LOSSY;
        else if( !strcmp(strings[1], "BATCH") )
            infos->type_job = BATCH;
        else
            exit(EXIT_FAILURE);

    }
    else{
        exit(EXIT_FAILURE);
    }
    return infos;
}

int main()
{
  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data

  printf("Main starting, retrieving array..\n");
  char * path = "test.txt";
  topology * genTop = getTopology(path, parse_strings);
  printf("Contents of generalTopology, total nodes : %d, sensor nodes: %d. \n",genTop->total_nodes,genTop->sensor_nodes);

  //topArray ** topArray = genTop->topArr;
  printf("Checking array integrity..\n");
  for(int i = 0; i < genTop->total_nodes + genTop->sensor_nodes; i++){

    int * solution = getNext(genTop, i);
    int numReceiv = getNumberReceiv(genTop, i);
    int j = 0;
    while(j < numReceiv){
      printf("Node %d sends to node %d.\n",i,solution[j]);
      j+=1;
    }

    parsingStruct * infos = getInfo(genTop, i);

    printf("Node/Sensor %d is of type: %d\n",i,infos->lp_type);
    if(infos->lp_type == 1){
      printf("Node %d has this response_time: %f\n",i,infos->response_time);
      printf("Node %d has this scheduler: %d\n",i,infos->scheduler);
    }
    else{
      printf("Sensor %d has this job type: %d\n",i,infos->type_job);
    }

  }
}
