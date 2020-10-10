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
int * getActType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->actuatorsTypesBelow);
}
int * getSensType(topology * top,int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->sensorsTypesBelow);
}

int getActuatorType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->actuator_type);
}

int getSensorType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->sensor_type);
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

double ** getLANsINserviceTimesByType(topology * top){
  return(top->LANsINserviceTimes);
}
double ** getLANsOUTserviceTimesByType(topology * top){
  return(top->LANsOUTserviceTimes);
}
double * getLANsINserviceTimesForOneLANType(topology * top, int index){
  return(top->LANsINserviceTimes[index]);
}
double * getLANsOUTserviceTimesForOneLANType(topology * top, int index){
  return(top->LANsOUTserviceTimes[index]);
}

int ** getActuatorPaths(topology * top){
  return(top->actuatorPaths);
}
int * getActuatorPathsIndex(topology * top, int index)
{
  return top->actuatorPaths[index];
}
int * getListActuatorsByType(topology * top, int nodeId, int actuatorType)
{
    return top->ListActuatorsByType[nodeId][actuatorType];
}
int * getListSensorsByType(topology * top, int nodeId, int sensorType)
{
    return top->ListSensorsByType[nodeId][sensorType];
}

void setLowerElements(topology * top, int * lowerEle, int numberLower, int index)
{
  topArray ** array = top->topArr;
  array[index]->numberOfLowerElements = numberLower;
  array[index]->lowerElements = lowerEle;
}
void setLANs(topology * top, int * arrayLANs, int numberLANs, int index){
  topArray ** array = top->topArr;
  array[index]->numberOfLANS = numberLANs;
  array[index]->connectedLans = arrayLANs;
}
void setWANdown(topology * top, int idWAN, int index){
  lp_infos * infos = getInfo(top, index);
  infos->id_WAN_down = idWAN;
}
void setWANup(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  int upp = getUpperNode(top, index);
  if(upp == -1){
    infos->id_WAN_up = -1;
  }
  else{
    int uppType =getType(top, upp);
    if(uppType == 3){
      infos->id_WAN_up = upp;
    }
  }
}
