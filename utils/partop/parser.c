#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>

topology * getTopology(char * path, void* (*converting)(char **)){

  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char * numberOfNodes;

  fp = fopen(path, "r");

  if (fp == NULL)
      exit(EXIT_FAILURE);

  //read first line, assess number of nodes, save
  read = getline(&numberOfNodes, &len, fp);
  int nn = atoi(numberOfNodes);
  //same for 2nd line, number of sensors
  read = getline(&numberOfNodes, &len, fp);
  int ns = atoi(numberOfNodes);

  printf("There are %d nodes.\n",nn);
  printf("There are %d sensor nodes.\n",ns);
  topology * genTop = malloc(sizeof(topology));
  topArray ** returnArray = malloc(sizeof(topArray *) * ns);
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
          //printf("Node %d has %d informations",temp,numberOfInfos);
          infoArray = malloc(sizeof(char *)*numberOfInfos);
        }
        else{//infos
          infoArray[counter] = strdup(ptr2);
        }
        counter+=1;
        ptr2 = strtok_r(NULL,",",&end_token);
      }
      //finished one ";" token
      index+=1;
      ptr=strtok_r(NULL, ";",&end_str);
    }

    tp->numberOfReceivers = numberOfReceivers;
    tp->numberInfos = numberOfInfos;
    tp->receiver = receiversArray;
    tp->info = converting(infoArray);
    //tp->info = infoArray;
    returnArray[temp] = tp;
  }

  fclose(fp);

  genTop->total_nodes = nn;
  genTop->sensor_nodes = ns;
  genTop->topArr = returnArray;

  return genTop;
}
