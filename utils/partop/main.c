#include <stdio.h>
#include "header.h"

int main()
{
  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data

  printf("Main starting, retrieving array..\n");
  char * path = "test.txt";
  topology * genTop = getTopology(path);
  printf("Contents of generalTopology, total nodes : %d, sensor nodes: %d. \n",genTop->total_nodes,genTop->sensor_nodes);

  //topArray ** topArray = genTop->topArr;
  printf("Checking array integrity..\n");

  for(int i = 0; i < 6; i++){

    int * solution = getNext(genTop, i);
    int numReceiv = getNumberReceiv(genTop, i);
    int j = 0;

    while(j < numReceiv){
      printf("Node %d sends to node %d\n",i,solution[j]);
      j+=1;
    }

    char ** info = getInfo(genTop, i);
    int numInf = getNumberInfos(genTop, i);
    j=0;

    while(j < numInf){
      printf("Node %d has this info: %s\n",i,info[j]);
      j+=1;
    }
  }
}
