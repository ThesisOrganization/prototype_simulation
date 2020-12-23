#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int main()
{
/*
  lp_topology * lptopo =  getLPtopoogy("../../tree_simulator/LP.txt");
  int nlp = getNumLP(lptopo);
  printf("Number of LPs: %d.\n",nlp);
  int x = 0;
  while(x < nlp){
    int * elementArray = getLPtoElementMappingOneLP(lptopo,x);
    int amount = getAmountsOfElementsInOneLP(lptopo,x);
    printf("LP %d has elements:",x);
    for(int c = 0; c < amount; c++){
      printf(" %d",elementArray[c]);
    }
    printf(".\n");
    x+=1;
  }
  x = 0;
  while(x < 120){
    int res = getElementToLPMappingOneElement(lptopo,x);
    printf("Element %d maps to LP %d\n",x,res);
    x+=1;
  }
*/

  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data
  printf("Main starting, retrieving topology..\n");
  //char * path = "topology.txt";
  char * path = "../../tree_simulator/topology.txt";
  char * path1 = "../../tree_simulator/LP.txt";

  total_topology * totTop = getTopology(path,path1,LP_AGGR_REGIONAL);
  general_topology * genTop = getGenTopology(totTop);

  int nn = getTotalNodes(genTop);
  int ns = getSensorNodes(genTop);
  int na = getActuatorNodes(genTop);
  int nl = getNumberOfTotalLANs(genTop);
  int nw = getNumberOfTotalWANs(genTop);

  int totalElements = nn+ns+na+nl+nw;
  printf("Starting general topology prints\n");

  printf("Contents of generalTopology:\ntotal nodes : %d;\nsensors/actuators: %d/%d;\nWANs/LANs %d/%d;\n",nn,ns,na,nw,nl);

  int at = getNumberOfActTypes(genTop);
  int st = getNumberOfSensTypes(genTop);
  int lt = getNumberOfLANsTypes(genTop);
  printf("%d act types, %d sens types, %d lan types.\n",at,st,lt);

  double * probAct = getProbOfActuators(genTop);
  for(int c = 0; c < at; c++){
    printf("%f probability of actuator type %d of receiving a command.\n",probAct[c],c);
  }
  printf("General topology correctly analyzed!\n");

  printf("############################################\n");
  printf("Starting LP topologies prints..\n");

  for(int i = 0; i < totalElements; i++){
    Element_topology * temp_lpt =  getLPTopology(totTop,i);


    int * solution = getLowers(temp_lpt);

    int numLow = getNumberLower(temp_lpt);
    int j = 0;

    int upp = getUpperNode(temp_lpt);
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
    int lp_type = getType(temp_lpt);
    //char topology_path[] = "./topology.txt";
    char file_name[] = "bin/lp";
    //char end_file_name[] = ".json";


    char file_name_complete[64];
    sprintf(file_name_complete, "%s%d" ,file_name,i);

    FILE * output_file = fopen(file_name_complete, "w");
    fwrite(&i, sizeof(int), 1, output_file);

    fclose(output_file);

    if(lp_type == 0){//node
      int node_type = getNodeType(temp_lpt);
      printf("Node %d has this type: %d.\n",i,node_type);
      int scheduler = getScheduler(temp_lpt);
      printf("Node %d has this scheduler: %d.\n",i,scheduler);
      int cores = getNumberOfCores(temp_lpt);
      printf("Node %d has this many cores: %d \n",i,cores);
      if(node_type == 0){//Central
        int disk_type = getDiskType(temp_lpt);
        printf("Node %d has this type of DISK: %d.\n",i,disk_type);
        double * diskServices = getDiskServices(temp_lpt);
        for(int k = 0;k < 4;k++){
          if(k == 0){
            printf("Node %d has a disk with these service times: %f",i,diskServices[k]);
          }
          else{
            printf(", %f",diskServices[k]);
          }
        }
        printf(".\n");
      }
      int * aggregation_rates = getAggregationRate(temp_lpt);
      for(int k = 0;k < 4;k++){
        if(k == 0){
          printf("Node %d has these aggregation rates %d",i,aggregation_rates[k]);
        }
        else{
          printf(", %d",aggregation_rates[k]);
        }
      }
      printf(".\n");

      float delay_upper_router = getDelayUpperRouter(temp_lpt);
      float delay_lower_router = getDelayLowerRouter(temp_lpt);
      printf("Node %d has this delay on upper router: %f.\n",i,delay_upper_router);
      printf("Node %d has this delay on lower router: %f.\n",i,delay_lower_router);

      double * serviceTimes = getServiceTimesNodes(temp_lpt);
      for(int k = 0;k < 5;k++){
        printf("Node %d, when processing a message of type %d, has a service time %f.\n",i,k,serviceTimes[k]);
      }

      float pr = getProbCommandResponse(temp_lpt);
      printf("Node %d has probability of %f of responding to a transition with a command.\n",i,pr);

      int id_WAN_up = getWanUp(temp_lpt);
      int id_WAN_down = getWanDown(temp_lpt);
      printf("Node %d has upper WAN : %d.\n",i,id_WAN_up);
      printf("Node %d has lower WAN: %d.\n",i,id_WAN_down);

      int numberOfBelowActuators = getNumberOfBelowActuators(temp_lpt);
      printf("Node %d has %d actuators below(undirectly).\n",i,numberOfBelowActuators);
      int * actuatorsTypesBelow = getActuatorTypesBelowList(temp_lpt);
      for(int k = 0;k < at;k++){
        int * ListActuatorsByType = getListActuatorsByType(temp_lpt, k);
        if(actuatorsTypesBelow[k] != 0){
          printf("Node %d has %d actuators of type %d: %d",i,actuatorsTypesBelow[k],k,ListActuatorsByType[0]);
          for(int t = 1; t < actuatorsTypesBelow[k]; t++){
            printf(", %d",ListActuatorsByType[t]);
          }
          printf(".\n");
        }

      }


      int numLANS = getNumberLANS(temp_lpt);

      int * LANS = getLANS(temp_lpt);
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

    //idmap * actuatorPaths = getActuatorPaths(temp_lpt);
    for(int k = 0;k < totalElements; k++){
      if(k == 0){
        printf("Printing paths, only available paths are printed!\n");
      }
      int ap = getActuatorPathsIndex(temp_lpt,k);
      if(ap != -1){
        printf("Node %d next hop to reach %d : [%d]\n",i,k,ap);
      }
    }
  }
    else if(lp_type == 1){//sensor
      int type_job = getTypeJob(temp_lpt);
      printf("Sensor %d has this job type: %d.\n",i,type_job);
      int sensor_type = getSensorType(temp_lpt);
      printf("Sensor %d is of this type: %d.\n",i,sensor_type);
      int measure_type = getMeasureType(temp_lpt);
      printf("Sensor %d has this measure type: %d.\n",i,measure_type);
      int id_LAN_up = getLANup(temp_lpt);
      printf("Sensor %d has this LAN up: %d.\n",i,id_LAN_up);

      double * sensorRates = getSensorRates(temp_lpt);
      for(int k = 0; k < 2; k++){
        printf("Sensor %d has rate %f for message type %d.\n",i,sensorRates[k],k);
      }
    }
    else if(lp_type == 2){
      int type_job = getTypeJob(temp_lpt);
      printf("Actuator %d has this job type: %d.\n",i,type_job);
      int actuator_type = getActuatorType(temp_lpt);
      printf("Actuator %d is of this type: %d.\n",i,actuator_type);
      int measure_type = getMeasureType(temp_lpt);
      printf("Actuator %d has this measure type: %d.\n",i,measure_type);
      int id_LAN_up = getLANup(temp_lpt);
      printf("Actuator %d has this LAN up: %d.\n",i,id_LAN_up);
      double rateTransition = getRateTransition(temp_lpt);
      printf("Actuator %d has this rate of transition: %f.\n",i,rateTransition);
      double serviceTimeCommand = getServiceTimeCommand(temp_lpt);
      printf("Actuator %d has this service time for commands: %f.\n",i,serviceTimeCommand);

    }
    else if(lp_type == 3){//wan
      int wan_type = getWanType(temp_lpt);
      printf("WAN %d has this type: %d.\n",i,wan_type);
      float delay = getDelay(temp_lpt);
      printf("WAN %d has this delay: %f.\n",i,delay);

      //idmap * actuatorPaths = getActuatorPaths(temp_lpt);
      for(int k = 0;k < totalElements; k++){
        if(k == 0){
          printf("Printing paths, only available paths are printed!\n");
        }
        int ap = getActuatorPathsIndex(temp_lpt,k);
        if(ap != -1){
          printf("WAN %d next hop to reach %d : [%d]\n",i,k,ap);
        }
      }
      int numberOfBelowActuators = getNumberOfBelowActuators(temp_lpt);
      printf("WAN %d has %d actuators below(undirectly).\n",i,numberOfBelowActuators);
      int * actuatorsTypesBelow = getActuatorTypesBelowList(temp_lpt);
      for(int k = 0;k < at;k++){
        int * ListActuatorsByType = getListActuatorsByType(temp_lpt, k);
        if(actuatorsTypesBelow[k] != 0){
          printf("WAN %d has %d actuators of type %d: %d",i,actuatorsTypesBelow[k],k,ListActuatorsByType[0]);
          for(int t = 1; t < actuatorsTypesBelow[k]; t++){
            printf(", %d",ListActuatorsByType[t]);
          }
          printf(".\n");
        }

      }

    }
    else if(lp_type == 4){//lan
      int lan_type = getLanType(temp_lpt);
      printf("LAN %d has this type: %d.\n",i,lan_type);

      float delay = getDelay(temp_lpt);
      printf("LAN %d has this delay: %f.\n",i,delay);

      //idmap * actuatorPaths = getActuatorPaths(temp_lpt);
      for(int k = 0;k < totalElements; k++){
        if(k == 0){
          printf("Printing paths, only available paths are printed!\n");
        }
        int ap = getActuatorPathsIndex(temp_lpt,k);
        if(ap != -1){
          printf("LAN %d next hop to reach %d : [%d]\n",i,k,ap);
        }
      }

      int numberOfBelowActuators = getNumberOfBelowActuators(temp_lpt);
      printf("LAN %d has %d actuators below(undirectly).\n",i,numberOfBelowActuators);
      int * actuatorsTypesBelow = getActuatorTypesBelowList(temp_lpt);
      for(int k = 0;k < at;k++){
        int * ListActuatorsByType = getListActuatorsByType(temp_lpt, k);
        if(actuatorsTypesBelow[k] != 0){
          printf("LAN %d has %d actuators of type %d: %d",i,actuatorsTypesBelow[k],k,ListActuatorsByType[0]);
          for(int t = 1; t < actuatorsTypesBelow[k]; t++){
            printf(", %d",ListActuatorsByType[t]);
          }
          printf(".\n");
        }
      }

      double * LANsINserviceTimes = getLANsINserviceTimesByType(temp_lpt);
      double * LANsOUTserviceTimes = getLANsOUTserviceTimesByType(temp_lpt);
      for(int k = 0; k < 5;k++){
        printf("LAN IN service type for message type %d : %f.\n",k,LANsINserviceTimes[k]);
        printf("LAN OUT service type for message type %d : %f.\n",k,LANsOUTserviceTimes[k]);
      }
    }
  }

  for(int i = 0; i < totalElements; i++){
    Element_topology * temp_lpt =  getLPTopology(totTop,i);
    int numLowers = getNumberLower(temp_lpt);
    int * lowers = getLowers(temp_lpt);
    int upp = getUpperNode(temp_lpt);
    int index = 0;
    for(int j = 0; j < numLowers+1;j++){
      if(j == 0){
        index = upp;
      }
      else{
        index = lowers[j-1];
      }
      if(index != -1){
        int result = getElToLPMappingOneElement(temp_lpt,index);
        printf("Element %d to reach element %d goes through LP %d.\n",i,index,result);
      }
    }

  }
  

  destroyTotalTopology(totTop);
/*
  for(int i = 0; i < totalElements; i++){
    int intero;
    char file_name[] = "bin/lp";
    //char end_file_name[] = ".json";

    char file_name_complete[64];
    sprintf(file_name_complete, "%s%d" ,file_name,i);
    printf("file name %s\n",file_name_complete);
    FILE * output_file = fopen(file_name_complete, "r");
    fread(&intero, sizeof(Element_topology*), 1, output_file);

    printf("Upper node %d\n",intero);
    //int type = getType(temp_lpt);
    //printf("%d \n",type);
    fclose(output_file);
  }
*/
}
