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

int getType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->lp_type);
}
double * getServiceRates(topology * top, int index){
  lp_infos * info = getInfo(top,index);
  return info->service_time;
}

double ** getSensorRatesByType(topology * top)
{
  return top->sensorRatesByType;
}
double * getSensorRatesForOneSensorType(topology * top, int index)
{
  return top->sensorRatesByType[index];
}

double ** getLANServiceTimesByType(topology * top){
  return(top->LANServiceTimesByType);
}
double * getLANServiceTimesForOneLANType(topology * top, int index)
{
  return top->LANServiceTimesByType[index];
}

double ** getActuatorPaths(topology * top){
  return(top->actuatorPaths);
}
double * getActuatorPathsIndex(topology * top, int index)
{
  return top->actuatorPaths[index];
}
