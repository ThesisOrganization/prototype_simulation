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
    char * ptr = strtok(line, ",");
    int temp = atoi(ptr); //sender
    int index = 0;
    ptr=strtok(NULL, ",");
    //For now looping is useless but we hold it in case of future implementations of broadcasts
    while(ptr){
      if(index == 0){
        tp.receiver = atoi(ptr);//receiver(s)
      }
      else{
        //should probably remove /n
        char * tmp = strdup(ptr);
        tp.info = tmp;
      }
      index+=1;
      ptr=strtok(NULL, ",");
    }
    tp.nn = nn;
    returnArray[temp] = tp;

  }

  //printf("Array created, sending it to topology and closing file.\n");
  fclose(fp);

  return(returnArray);
}
