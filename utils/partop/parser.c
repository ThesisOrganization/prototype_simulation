#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>

int * getTopology(){

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
  int * topArray = malloc(sizeof(int) * nn);

  //read the file, first token in each line is the sender node, second and following the receivers
  while ((read = getline(&line, &len, fp)) != -1) {
    char * ptr = strtok(line, ",");
    int temp = atoi(ptr); //sender
    ptr=strtok(NULL, ",");
    //For now looping is useless but we hold it in case of future implementations of broadcasts
    while(ptr){
      topArray[temp] = atoi(ptr);//receiver(s)
      ptr=strtok(NULL, ",");
    }
  }
  //printf("Array created, sending it to topology and closing file.\n");
  fclose(fp);

  return(topArray);

}
