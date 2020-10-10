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

  printf("Contents of generalTopology:\ntotal nodes : %d;\nsensors/actuators: %d/%d;\nWANs/LANs %d/%d;\n",genTop->total_nodes,genTop->sensor_nodes,genTop->actuator_nodes, genTop->numberOfTotalWANs,  genTop->numberOfTotalLANs);

  double ** res = getSensorRatesByType(genTop);
  int k = 0;
  int l;

  printf("Rates for each sensor type and message type couple:\n");
  while(k < genTop->numberOfSensTypes){
    l = 0;
    while(l < 2){//# of possible message types
      printf("Type %d Sensor, sending a message of type %d has a rate of %f;\n",k, l, res[k][l]);
      l+=1;
    }
    k+=1;
  }
/*
  k = 0;
  printf("Rates for each sensor type and message type couple:\n");
  while(k < genTop->numberOfSensTypes){
    l = 0;
    double * temp1 = getSensorRatesForOneSensorType(genTop,k);
    while(l < 2){//# of possible message types
      printf("Type %d Sensor, sending a message of type %d has a rate of %f;\n",k, l, temp1[l]);
      l+=1;
    }
    k+=1;
  }*/
  /*
  double ** LSTBT = getLANServiceTimesByType(genTop);
  k = 0;
  printf("Service times for each LAN type and message type couple:\n");
  while(k < genTop->numberOfLANsTypes){ //# of possible message types
    l = 0;
    while(l < 5){
      printf("Type %d LAN serving a message of type %d has a service time of %f;\n",k, l, LSTBT[k][l]);
      l+=1;
    }
    k+=1;
  }
  */
  //alternatively
  k = 0;
  printf("Service times for each LAN type and message type couple:\n");
  while(k < genTop->numberOfLANsTypes){ //# of possible message types
    l = 0;
    double * temp2 = getLANServiceTimesForOneLANType(genTop,k);
    while(l < 5){
      printf("Type %d LAN serving a message of type %d has a service time of %f;\n",k, l, temp2[l]);
      l+=1;
    }
    k+=1;
  }
  //double * getSensorRatesForOneType(topology * top, int index);
  for(k = 0;k < genTop->numberOfActTypes;k++){
    printf("Probability of a sensor of type %d of receiving a command: %f. \n",k,genTop->probOfActuators[k]);
  }
  for(k = 0;k < 3;k++){
    printf("Probability of a node of type %d of reacting to a transition with a command: %f. \n",k,genTop->probNodeCommandArray[k]);
  }

  //topArray ** topArray = genTop->topArr;
  printf("\nChecking informations integrity..\n");

  for(int i = 0; i < genTop->total_nodes + genTop->sensor_nodes + genTop->actuator_nodes + genTop->numberOfTotalWANs + genTop->numberOfTotalLANs; i++){

    int * solution = getLowers(genTop, i);

    int numLow = getNumberLower(genTop, i);
    int j = 0;

    lp_infos * infos = getInfo(genTop, i);

    int upp = getUpperNode(genTop, i);
    if(upp != -1) printf("Element %d has upper node %d.\n",i,upp);

    if(solution[0] != -1){//-1 is a temporal solution when nodes have no lower level
      while(j < numLow){
        if(j == 0){
          printf("Element %d has lower node(s): %d",i,solution[j]);
        }
        else{
          printf(",%d",solution[j]);
        }
        j+=1;
      }
      printf(".\n");
    }

    if(infos->lp_type == 0){//node
      double * serviceTimes = getServiceRates(genTop, i);
      for(k = 0;k < 5;k++){
        printf("Node %d, when processing a message of type %d, has a service time %f.\n",i,k,serviceTimes[k]);
      }
      printf("Node %d has this scheduler: %d.\n",i,infos->scheduler);
      printf("Node %d has this type: %d.\n",i,infos->node_type);
      printf("Node %d has %d total sensors below.\n", i, infos->numberOfBelowSensors);
      for(k = 0;k < genTop->numberOfActTypes;k++){
        if(k == 0){
          printf("Node %d has %d actuators of type %d",i,infos->actuatorsTypesBelow[k],k);
        }
        else{
          printf(", %d of type %d",infos->actuatorsTypesBelow[k],k);
        }
      }

      printf(".\n");
      printf("Node %d has upper WAN : %d.\n",i,infos->id_WAN_up);
      printf("Node %d has lower WAN: %d.\n",i,infos->id_WAN_down);
      printf("Node %d has this aggregation rate: %d.\n",i,infos->aggregation_rate);
      printf("Node %d has this delay on upper router: %f.\n",i,infos->delay_upper_router);
      printf("Node %d has this delay on lower router: %f.\n",i,infos->delay_lower_router);

      int numLANS = getNumberLANS(genTop, i);

      int * LANS = getLANS(genTop, i);
      int c = 0;
      if(LANS[0] != -1){
        printf("Node %d manages %d LAN(s): %d", i,numLANS, LANS[0]);
        c+=1;
        while(c < numLANS){
          printf(", %d",LANS[c]);
          c+=1;
        }
      printf(".\n");
      }
    }

    else if(infos->lp_type ==1){//SENSOR
      printf("Sensor %d has this job type: %d.\n",i,infos->type_job);
      printf("Sensor %d is of this type: %d.\n",i,infos->sensor_type);
      printf("Sensor %d has this measure type: %d.\n",i,infos->measure_type);
      printf("Sensor %d has this LAN up: %d.\n",i,infos->id_LAN_up);

    }
    else if(infos->lp_type ==2){//ACTUATOR
      printf("Actuator %d has this job type: %d.\n",i,infos->type_job);
      printf("Actuator %d is of this type: %d.\n",i,infos->actuator_type);
      printf("Actuator %d has this measure type: %d.\n",i,infos->measure_type);
      printf("Actuator %d has this rate of transition type: %f.\n",i,infos->rateTransition);
      printf("Actuator %d has this service time for commands: %f.\n",i,infos->serviceTimeCommand);
      printf("Actuator %d has this LAN up: %d.\n",i,infos->id_LAN_up);
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
