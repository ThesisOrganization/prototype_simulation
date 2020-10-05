#include <stdio.h>
#include "header.h"

//for now extremely simple function, just returning the node to send the data, will get messier.


int getUpperNode(topology * top, int index)
{
  topArray ** array = top->topArr;

  return array[index]->upperNode;
}

int getNumberLower(topology * top, int index)
{
  topArray ** array = top->topArr;
  return array[index]->numberOfLowerElements;
}

int * getLowers(topology * top, int index)
{
  topArray ** array = top->topArr;
  return array[index]->lowerElements;
}

int getNumberLANS(topology * top, int index){
  topArray ** array = top->topArr;
  return array[index]->numberOfLANS;
}

int * getLANS(topology * top, int index){
  topArray ** array = top->topArr;
  return array[index]->connectedLans;
}

void * getInfo(topology * top, int index)
{
  topArray ** array = top->topArr;
  return array[index]->info;
}
double ** getSensorRatesByType(topology * top)
{
  return top->sensorRatesByType;
}
