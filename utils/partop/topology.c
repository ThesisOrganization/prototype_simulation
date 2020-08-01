#include <stdio.h>
#include "header.h"

//for now extremely simple function, just returning the node to send the data, will get messier.
int * getNext(topology * top, int index)
{
  topArray ** array = top->topArr;
  return array[index]->receiver;
}

void * getInfo(topology * top, int index)
{
  topArray ** array = top->topArr;
  return array[index]->info;
}

int getNumberReceiv(topology * top, int index){
  topArray ** array = top->topArr;
  return array[index]->numberOfReceivers;
}

int getNumberInfos(topology * top, int index){
  topArray ** array = top->topArr;
  return array[index]->numberInfos;
}
