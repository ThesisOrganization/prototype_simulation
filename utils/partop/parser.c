#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>

struct topArray * getTopology(){

  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char * numberOfNodes;

  fp = fopen("test.txt", "r");

  if (fp == NULL)
      exit(EXIT_FAILURE);

  //read first line, assess number of nodes, allocate array
  read = getline(&numberOfNodes, &len, fp);
  int nn = atoi(numberOfNodes);
  //printf("There are %d sender nodes.\n",nn);
  struct topArray * returnArray = malloc(sizeof(struct topArray) * nn);

  //read the file, first token in each line is the sender node, second and following the receivers
  while ((read = getline(&line, &len, fp)) != -1) {
    struct topArray tp;

    char *end_str;
    char * ptr = strtok_r(line, ";", &end_str);
    //char * ptr = strtok(line, ",");

    int temp = atoi(ptr); //sender
    int numberOfReceivers;
    int numberOfInfos;
    int index = 0;

    //For now looping is useless but we hold it in case of future implementations of broadcasts
    //receiver;#senders;senders;#info;info
    int * receiversArray;
    char ** infoArray;
    ptr = strtok_r(NULL, ";", &end_str);

    while(ptr){
      char * end_token;
      char *ptr2 = strtok_r(ptr,",",&end_token);

      int counter = 0;
      while(ptr2){
        if(index == 0){//#receivers
          numberOfReceivers = atoi(ptr2);
          //printf("node %d has %d receivers\n",temp,numberOfReceivers);
          receiversArray = malloc(sizeof(int *)*numberOfReceivers);
        }
        else if(index == 1){//receivers
          //should probably remove /n
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
      index+=1;
      ptr=strtok_r(NULL, ";",&end_str);
    }
    tp.numberOfReceivers = numberOfReceivers;
    tp.numberInfos = numberOfInfos;
    tp.receiver = receiversArray;
    tp.info = infoArray;
    tp.nn = nn;
    returnArray[temp] = tp;
  }




  //printf("Array created, sending it to topology and closing file.\n");
  fclose(fp);

  return(returnArray);
}
