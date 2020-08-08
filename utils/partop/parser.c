#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>

void * parse_strings(char ** strings){

    parsingStruct * infos = malloc(sizeof(parsingStruct));
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

        double resp_time = strtod(strings[2], &ptr);
        infos->response_time = resp_time;

        if( !strcmp(strings[3], "CENTRAL") )
            infos->node_type = CENTRAL;
        else if( !strcmp(strings[3], "REGIONAL") )
            infos->node_type = REGIONAL;
        else if( !strcmp(strings[3], "NETWORK") )
            infos->node_type = NETWORK;
        else
            exit(EXIT_FAILURE);

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
        else
            exit(EXIT_FAILURE);

        flag = 1;

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
    }
    return infos;
}

topology * getTopology(char * path){

  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char * numberOfNodes = NULL;

  fp = fopen(path, "r");

  if (fp == NULL)
      exit(EXIT_FAILURE);

  //read first line, assess number of nodes, save
  read = getline(&numberOfNodes, &len, fp);
  int nn = atoi(numberOfNodes);
  //same for 2nd line, number of sensors
  read = getline(&numberOfNodes, &len, fp);
  int ns = atoi(numberOfNodes);

  //printf("There are %d nodes.\n",nn);
  //printf("There are %d sensor nodes.\n",ns);
  topology * genTop = malloc(sizeof(topology));
  topArray ** returnArray = malloc(sizeof(topArray *) * (ns + nn));
  //iterate through the remaining lines, having the following syntax:
  //receiver;#senders;senders;#info;info
  while ((read = getline(&line, &len, fp)) != -1) {
    topArray * tp = malloc(sizeof(topArray));

    //tokenize the line using ";" as a separator
    char *end_str;
    char * ptr = strtok_r(line, ";", &end_str);
    //first element is the sender
    int temp = atoi(ptr);

    int numberOfReceivers = 0;
    int numberOfInfos = 0;
    int index = 0;//keep track of how many ";" token we iterated on so we know
    //which kind data we are analyzing

    int * receiversArray = NULL;
    char ** infoArray = NULL;

    ptr = strtok_r(NULL, ";", &end_str);
    //loop through all the other tokens in the line
    while(ptr){
      char * end_token;
      char *ptr2 = strtok_r(ptr,",",&end_token);
      //tokenize each ";" token through ",", iterate until end of the line
      int counter = 0;//used to keep track of how many "," we iterated on
      while(ptr2){
        if(index == 0){//#receivers
          numberOfReceivers = atoi(ptr2);
          //printf("node %d has %d receivers\n",temp,numberOfReceivers);
          receiversArray = malloc(sizeof(int *)*numberOfReceivers);
        }
        else if(index == 1){//receivers
          receiversArray[counter] = atoi(ptr2);
        }
        else if(index == 2){//#info
          numberOfInfos = atoi(ptr2);
          //printf("Node %d has %d informations.\n",temp,numberOfInfos);
          infoArray = malloc(sizeof(char *)*numberOfInfos);
        }
        else{//infos
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
    tp->numberOfReceivers = numberOfReceivers;
    tp->receiver = receiversArray;
    tp->info = parse_strings(infoArray);
    //tp->info = infoArray;
    returnArray[temp] = tp;
  }
  fclose(fp);

  genTop->total_nodes = nn;
  genTop->sensor_nodes = ns;
  genTop->topArr = returnArray;

  return genTop;
}
