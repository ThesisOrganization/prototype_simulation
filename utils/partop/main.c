#include <stdio.h>
#include "header.h"

int main()
{
  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data

  printf("Main starting, retrieving array..\n");
  int * topArray = getTopology();
  printf("Checking array integrity..\n");
  for(int i = 0; i < 5; i++){
    int solution = getNext(topArray, i);
    printf("node %d sends to node %d\n",i,solution);
  }
}
