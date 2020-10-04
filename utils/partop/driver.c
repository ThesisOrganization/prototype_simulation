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
  printf("Contents of generalTopology, total nodes : %d, sensor/actuator nodes: %d, number of connection elements: %d. \n",genTop->total_nodes,genTop->edge_nodes, genTop->connection_elements);

  //topArray ** topArray = genTop->topArr;
  printf("Checking array integrity..\n");
  for(int i = 0; i < genTop->total_nodes + genTop->edge_nodes + genTop->connection_elements; i++){

    int * solution = getLowers(genTop, i);
    int numLow = getNumberLower(genTop, i);
    int j = 0;

    lp_infos * infos = getInfo(genTop, i);

    while(j < numLow){
      printf("Node %d has lower node %d.\n",i,solution[j]);
      j+=1;
    }

    int upp = getUpperNode(genTop, i);

    printf("Node %d has upper node %d.\n",i,upp);


    if(infos->lp_type == 0){//node
      printf("Node %d has this service_time: %f.\n",i,infos->service_time);
      printf("Node %d has this scheduler: %d.\n",i,infos->scheduler);
      printf("Node %d has this type: %d.\n",i,infos->node_type);
      printf("Node %d has upper WAN : %d.\n",i,infos->id_WAN_up);
      printf("Node %d has lower WAN: %d.\n",i,infos->id_WAN_down);
      printf("Node %d has this aggregation rate: %d.\n",i,infos->aggregation_rate);
      printf("Node %d has this delay on upper router: %f.\n",i,infos->delay_upper_router);
      printf("Node %d has this delay on lower router: %f.\n",i,infos->delay_lower_router);

      int numLANS = getNumberLANS(genTop, i);

      int * LANS = getLANS(genTop, i);
      int c = 0;
      if(LANS[0] != -1){
        while(c < numLANS){
          printf("Node %d manages this LAN: %d.\n",i,LANS[c]);
          c+=1;
        }
      }
    }
    else if(infos->lp_type ==1){//SENSOR
      printf("Sensor %d has this job type: %d.\n",i,infos->type_job);
      printf("Sensor %d is of this type: %d.\n",i,infos->sensor_type);
      printf("Sensor %d has this measure type: %d.\n",i,infos->measure_type);
    }
    else if(infos->lp_type ==2){//ACTUATOR
      printf("Actuator %d has this job type: %d.\n",i,infos->type_job);
      printf("Actuator %d is of this type: %d.\n",i,infos->actuator_type);
      printf("Actuator %d has this measure type: %d.\n",i,infos->measure_type);
    }
    else if(infos->lp_type ==3){//WAN
      printf("WAN %d has this type: %d.\n",i,infos->wan_type);
      printf("WAN %d has this delay: %f.\n",i,infos->delay);
    }
    else if(infos->lp_type ==4){//LAN
      printf("LAN %d has this job type: %d.\n",i,infos->lan_type);
      printf("LAN %d has this delay: %f.\n",i,infos->delay);
    }
  }
}
