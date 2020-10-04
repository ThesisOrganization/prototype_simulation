#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>
#include "../application_datatypes.h"

void * parse_strings(char ** strings){
    lp_infos * infos = malloc(sizeof(lp_infos));
    char * ptr;
    int flag = 0;

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

        double serv_time = strtod(strings[2], &ptr);
        infos->service_time = serv_time;

        if( !strcmp(strings[3], "CENTRAL") )
            infos->node_type = CENTRAL;
        else if( !strcmp(strings[3], "REGIONAL") )
            infos->node_type = REGIONAL;
        else if( !strcmp(strings[3], "LOCAL") )
            infos->node_type = LOCAL;
        else
            exit(EXIT_FAILURE);

        int WAN_up = atoi(strings[4]);
        infos->id_WAN_up = WAN_up;

        int WAN_down = atoi(strings[5]);
        infos->id_WAN_down = WAN_down;

        int aggregation_rate = atoi(strings[6]);
        infos->aggregation_rate = aggregation_rate;


        float delayUP = strtod(strings[7],&ptr);
        infos->delay_upper_router = delayUP;
        float delayDOWN = atof(strings[7]);
        infos->delay_lower_router = delayDOWN;

    }
    else if( !strcmp(strings[0], "SENSOR") ){

        infos->lp_type = SENSOR;

        if( !strcmp(strings[1], "BATCH") )
            infos->type_job = BATCH;
        else if( !strcmp(strings[1], "REAL_TIME") )
            infos->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY") )
            infos->type_job = LOSSY;
        else
            exit(EXIT_FAILURE);

        if( !strcmp(strings[2], "SENSOR_TYPE0") )
            infos->sensor_type = SENSOR_TYPE0;
        else if( !strcmp(strings[2], "SENSOR_TYPE1") )
            infos->sensor_type = SENSOR_TYPE1;
        else
            exit(EXIT_FAILURE);

        flag = 1;

    }
    else if( !strcmp(strings[0], "ACTUATOR") ){

        infos->lp_type = ACTUATOR;

        if( !strcmp(strings[1], "BATCH") )
            infos->type_job = BATCH;
        else if( !strcmp(strings[1], "REAL_TIME") )
            infos->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY") )
            infos->type_job = LOSSY;
        else
            exit(EXIT_FAILURE);
        if( !strcmp(strings[2], "ACTUATOR_TYPE0") )
            infos->actuator_type = ACTUATOR_TYPE0;
        else if( !strcmp(strings[2], "ACTUATOR_TYPE1") )
            infos->actuator_type = ACTUATOR_TYPE1;
        else
            exit(EXIT_FAILURE);

        flag = 1;

    }
    else if( !strcmp(strings[0], "WAN") ){

        infos->lp_type = WAN;

        if( !strcmp(strings[1], "WAN_TYPE0") )
            infos->wan_type = WAN_TYPE0;
        else if( !strcmp(strings[1], "WAN_TYPE1") )
            infos->wan_type = LAN_TYPE0;
        else
            exit(EXIT_FAILURE);

        double delay = strtod(strings[2], &ptr);
        infos->delay = delay;

    }
    else if( !strcmp(strings[0], "LAN") ){

        infos->lp_type = LAN;

        if( !strcmp(strings[1], "LAN_TYPE0") )
            infos->lan_type = LAN_TYPE0;
        else if( !strcmp(strings[1], "LAN_TYPE1") )
            infos->lan_type = LAN_TYPE1;
        else if( !strcmp(strings[1], "LAN_TYPE2") )
            infos->lan_type = LAN_TYPE2;
        else
            exit(EXIT_FAILURE);

        double delay = strtod(strings[2], &ptr);
        infos->delay = delay;

    }
    else{
        exit(EXIT_FAILURE);
    }

    if(flag){//Either sensor or actuator
      if( !strcmp(strings[3], "MEASURE0") )
          infos->measure_type = MEASURE0;
      else if( !strcmp(strings[3], "MEASURE1") )
          infos->measure_type = MEASURE1;
      else if( !strcmp(strings[3], "MEASURE2") )
          infos->measure_type = MEASURE2;
      else
        exit(EXIT_FAILURE);

      int id_LAN_up = atoi(strings[4]);
      infos->id_LAN_up = id_LAN_up;
    }

    return infos;

}

topology * getTopology(char * path){



  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
    int i=0,counter=0;

  fp = fopen(path, "r");

  if (fp == NULL)
      exit(EXIT_FAILURE);

  char * temp = NULL;
  //read first line, assess number of nodes, save
  read = getline(&temp, &len, fp);
  int nn = atoi(temp);
  //same for 2nd line, number of sensors/actuators
  read = getline(&temp, &len, fp);
  int ns = atoi(temp);
  //3rd line, number of connection elements
  read = getline(&temp, &len, fp);
  int nc = atoi(temp);

  //printf("There are %d nodes.\n",nn);
  //printf("There are %d sensor/actuator nodes.\n",ns);
  topology * genTop = malloc(sizeof(topology));
  topArray ** returnArray = malloc(sizeof(topArray *) * (ns + nn + nc));
  //iterate through the remaining lines, having the following syntax:

  while ((read = getline(&line, &len, fp)) != -1) {
    topArray * tp = malloc(sizeof(topArray));

    //tokenize the line using ";" as a separator
    char *end_str;
    char * ptr = strtok_r(line, ";", &end_str);
    //first element is the sender
    int temp = atoi(ptr);

    int numberOfLowerElements = 0;
    int numberOfLANS = 0;
    int numberOfInfos = 0;
    int index = 0;//keep track of how many ";" token we iterated on so we know
    //which kind data we are analyzing

    int * lowerElementsArray = NULL;
    int upperNode = 0;
    char ** infoArray = NULL;
    int * LANSarray = NULL;

    ptr = strtok_r(NULL, ";", &end_str);
    //loop through all the other tokens in the line

    while(ptr){
      char * end_token;
      char *ptr2 = strtok_r(ptr,",",&end_token);
      //tokenize each ";" token through ",", iterate until end of the line
      counter = 0;//used to keep track of how many "," we iterated on

      while(ptr2){
        if(index == 0){//#receivers

          numberOfLowerElements = atoi(ptr2);
          lowerElementsArray = malloc(sizeof(int *)*numberOfLowerElements);
        }
        else if(index == 1){//receivers

          lowerElementsArray[counter] = atoi(ptr2);
        }
        else if(index == 2){//sender
          upperNode = atoi(ptr2);
          //printf("Node %d has %d informations.\n",temp,numberOfInfos);
        }
        else if(index == 3){//#lans
          numberOfLANS = atoi(ptr2);
          LANSarray = malloc(sizeof(int *)*numberOfLANS);

        }
        else if(index == 4){//LANS
          LANSarray[counter] = atoi(ptr2);
        }
        else if(index == 5){//#info
          numberOfInfos = atoi(ptr2);
          infoArray = malloc(sizeof(char *)*numberOfInfos);
        }
        else{//#info
          char * tempString = strdup(ptr2);
          //remove endline char from the info
          if(counter == numberOfInfos - 1){
            tempString[strlen(tempString)-1] = 0;
          }
          infoArray[counter] = tempString;
        }
        counter+=1;
        ptr2 = strtok_r(NULL,",",&end_token);
      }
      //finished one ";" token
      index+=1;
      ptr=strtok_r(NULL, ";",&end_str);
    }

    tp->numberOfLowerElements = numberOfLowerElements;
    tp->lowerElements = lowerElementsArray;
    tp->upperNode = upperNode;
    tp->numberOfLANS = numberOfLANS;
    tp->connectedLans = LANSarray;
    tp->info = parse_strings(infoArray);

		for(i=0;i<counter;i++){
				free(infoArray[i]);
		}
		free(infoArray);
    returnArray[temp] = tp;
  }

  fclose(fp);

  genTop->total_nodes = nn;
  genTop->edge_nodes = ns;
  genTop->connection_elements = nc;

  genTop->topArr = returnArray;

  return genTop;
}
