#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int main()
{
  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data

  printf("Main starting, retrieving array..\n");
  char * path = "test.txt";
  topology * genTop = getTopology(path);
  printf("Contents of generalTopology, total nodes : %d, sensor/actuator nodes: %d. \n",genTop->total_nodes,genTop->sensor_nodes);

  //topArray ** topArray = genTop->topArr;
  printf("Checking array integrity..\n");
  for(int i = 0; i < genTop->total_nodes + genTop->sensor_nodes; i++){

    int * solution = getNext(genTop, i);
    int numReceiv = getNumberReceiv(genTop, i);
    int j = 0;

    parsingStruct * infos = getInfo(genTop, i);

    while(j < numReceiv){
      printf("Node %d sends to node %d.\n",i,solution[j]);
      j+=1;
    }

    if(infos->lp_type == 1){//node
      printf("Node %d has this response_time: %f.\n",i,infos->response_time);
      printf("Node %d has this scheduler: %d.\n",i,infos->scheduler);
      printf("Node %d has this type: %d.\n",i,infos->node_type);
    }
    else if(infos->lp_type ==2){
      printf("Sensor %d has this job type: %d.\n",i,infos->type_job);
      printf("Sensor %d is of this type: %d.\n",i,infos->sensor_type);
      printf("Sensor %d has this measure type: %d.\n",i,infos->measure_type);
    }
    else{
      printf("Actuator %d has this job type: %d.\n",i,infos->type_job);
      printf("Actuator %d is of this type: %d.\n",i,infos->actuator_type);
      printf("Actuator %d has this measure type: %d.\n",i,infos->measure_type);
    }

  }
}
