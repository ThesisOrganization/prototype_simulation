#include <stdio.h>
#include "header.h"
//GETs from info
int getType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->lp_type);
}
int getActuatorType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->actuator_type);
}

int getSensorType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->sensor_type);
}

int getTypeJob(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->type_job);
}

int getMeasureType(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->measure_type);
}

int getLANup(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->id_LAN_up);
}

double getRateTransition(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->rateTransition);
}

double getServiceTimeCommand(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->serviceTimeCommand);
}

int getWanType(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->wan_type);
}

int getLanType(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->lan_type);
}

float getDelay(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->delay);
}

int getNodeType(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->node_type);
}

double * getServiceRates(topology * top, int index){
  lp_infos * info = getInfo(top,index);
  return info->service_time;
}

int getScheduler(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->scheduler);
}

int getNumberOfBelowActuators(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->numberOfBelowActuators);
}


int * getActType(topology * top,int index){
    lp_infos * infos = getInfo(top, index);
    return(infos->actuatorsTypesBelow);
}
int getWanUp(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->id_WAN_up);
}

int getWanDown(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->id_WAN_down);
}

int getNumberOfBelowSensors(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->numberOfBelowSensors);
}


int * getSensType(topology * top,int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->sensorsTypesBelow);
}

int * getAggregationRate(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->aggregation_rate);
}

float getDelayUpperRouter(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->delay_upper_router);
}
float getDelayLowerRouter(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->delay_lower_router);
}


float getProbCommandResponse(topology * top, int index)
{
  lp_infos * infos = getInfo(top, index);
  return infos->probCommandResponse;
}

int getDiskType(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->disk_type);
}

double * getDiskServices(topology * top, int index){
  lp_infos * infos = getInfo(top, index);
  return(infos->diskServices);
}

//TOPARRAY
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

//The following two seems like none uses them..
int getNumberLANS(topology * top, int index){
  topArray ** array = top->topArr;
  return array[index]->numberOfLANS;
}

int * getLANS(topology * top, int index){
  topArray ** array = top->topArr;
  return array[index]->connectedLans;
}

//Avoid using this one, will be deprecated soon.
void * getInfo(topology * top, int index)
{
  topArray ** array = top->topArr;
  return array[index]->info;
}

//TOPOLOGY
int getTotalNodes(topology * top)
{
  return top->total_nodes;
}

int getSensorNodes(topology * top)
{
  return top->sensor_nodes;
}

int getActuatorNodes(topology * top)
{
  return top->actuator_nodes;
}

int getNumberOfTotalLANs(topology * top)
{
  return top->numberOfTotalLANs;
}

int getNumberOfTotalWANs(topology * top)
{
  return top->numberOfTotalWANs;
}

int getNumberOfActTypes(topology * top)
{
  return top->numberOfActTypes;
}

int getNumberOfSensTypes(topology * top)
{
  return top->numberOfSensTypes;
}

int getNumberOfLANsTypes(topology * top)
{
  return top->numberOfLANsTypes;
}

int ** getActuatorPaths(topology * top){
  return(top->actuatorPaths);
}

int * getActuatorPathsIndex(topology * top, int index)
{
  return top->actuatorPaths[index];
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

double * getProbOfActuators(topology * top)
{
  return top->probOfActuators;
}

int * getListActuatorsByType(topology * top, int nodeId, int actuatorType)
{
    return top->ListActuatorsByType[nodeId][actuatorType];
}

int * getListSensorsByType(topology * top, int nodeId, int sensorType)
{
    return top->ListSensorsByType[nodeId][sensorType];
}

//SETUP FUNCTIONS
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

void setSensorTypes(topology * top, int * array, int index, int nts){
  lp_infos * infos = getInfo(top, index);
  int total = 0;
  for(int count = 0; count < nts; count++){
    total+=array[count];
  }
  infos->sensorsTypesBelow = array;
  infos->numberOfBelowSensors = total;
}
void setActuatorTypes(topology * top, int * array, int index, int nt){
  lp_infos * infos = getInfo(top, index);
  int total = 0;
  for(int count = 0; count < nt; count++){
    total+=array[count];
  }
  infos->actuatorsTypesBelow = array;
  infos->numberOfBelowActuators = total;
}
