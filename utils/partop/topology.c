#include <stdio.h>
#include "header.h"
//GETs from specific_topology
general_topology * getGenTopology(total_topology * totTop){
  return(totTop->gn);
}
Element_topology * getLPTopology(total_topology * totTop, int index){
  return(totTop->lpt[index]);
}
int getNodeType(Element_topology * lpt){
  return(lpt->spec_top.node->node_type);
}

int getActuatorType(Element_topology * lpt){
    return(lpt->spec_top.actuator->actuator_type);
}

int getSensorType(Element_topology * lpt){
    return(lpt->spec_top.sensor->sensor_type);
}

int getTypeJob(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 1){
    return(lpt->spec_top.sensor->type_job);

  }
  else{
    return(lpt->spec_top.actuator->type_job);
  }
}

int getMeasureType(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 1){
    return(lpt->spec_top.sensor->measure_type);

  }
  else{
    return(lpt->spec_top.actuator->measure_type);
  }

}
double * getSensorRates(Element_topology * lpt){
  return(lpt->spec_top.sensor->sensorRates);
}
int getLANup(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 1){
    return(lpt->spec_top.sensor->id_LAN_up);
  }
  else{
    return(lpt->spec_top.actuator->id_LAN_up);
  }
}

double getRateTransition(Element_topology * lpt){
  return(lpt->spec_top.actuator->rateTransition);
}

double getServiceTimeCommand(Element_topology * lpt){
  return(lpt->spec_top.actuator->serviceTimeCommand);
}

int getWanType(Element_topology * lpt){
  return(lpt->spec_top.wan->wan_type);
}

int getLanType(Element_topology * lpt){
  return(lpt->spec_top.lan->lan_type);
}

float getDelay(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 3){
    return(lpt->spec_top.wan->delay);
  }
  else{
    return(lpt->spec_top.lan->delay);
  }
}

double * getLANsINserviceTimesByType(Element_topology * lpt){
  return(lpt->spec_top.lan->LANsINserviceTimes);
}

double * getLANsOUTserviceTimesByType(Element_topology * lpt){
  return(lpt->spec_top.lan->LANsOUTserviceTimes);
}

//will be deprecated with getServiceTimesNodes
double * getServiceRates(Element_topology * lpt){
  return(lpt->spec_top.node->service_time);
}
double * getServiceTimesNodes(Element_topology * lpt){
  return(lpt->spec_top.node->service_time);
}
int getScheduler(Element_topology * lpt){
  return(lpt->spec_top.node->scheduler);
}

int getNumberOfBelowActuators(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 0){
    return(lpt->spec_top.node->numberOfBelowActuators);
  }
  else if(type == 3){
    return(lpt->spec_top.wan->numberOfBelowActuators);
  }
  else{
    return(lpt->spec_top.lan->numberOfBelowActuators);
  }
}

int * getActType(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 0){
    return(lpt->spec_top.node->actuatorsTypesBelow);
  }
  else if(type == 3){
    return(lpt->spec_top.wan->actuatorsTypesBelow);
  }
  else{
    return(lpt->spec_top.lan->actuatorsTypesBelow);
  }
}
int * getActuatorTypesBelowList(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 0){
    return(lpt->spec_top.node->actuatorsTypesBelow);
  }
  else if(type == 3){
    return(lpt->spec_top.wan->actuatorsTypesBelow);
  }
  else{
    return(lpt->spec_top.lan->actuatorsTypesBelow);
  }
}

int getWanUp(Element_topology * lpt){
  return(lpt->spec_top.node->id_WAN_up);
}

int getWanDown(Element_topology * lpt){
  return(lpt->spec_top.node->id_WAN_down);
}

int getNumberOfBelowSensors(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 0){
    return(lpt->spec_top.node->numberOfBelowSensors);
  }
  else if(type == 3){
    return(lpt->spec_top.wan->numberOfBelowSensors);
  }
  else{
    return(lpt->spec_top.lan->numberOfBelowSensors);
  }
}

int * getSensType(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 0){
    return(lpt->spec_top.node->sensorsTypesBelow);
  }
  else if(type == 3){
    return(lpt->spec_top.wan->sensorsTypesBelow);
  }
  else{
    return(lpt->spec_top.lan->sensorsTypesBelow);
  }

}

int * getSensorsTypesBelowList(Element_topology * lpt){
  int type = getType(lpt);
  if(type == 0){
    return(lpt->spec_top.node->sensorsTypesBelow);
  }
  else if(type == 3){
    return(lpt->spec_top.wan->sensorsTypesBelow);
  }
  else{
    return(lpt->spec_top.lan->sensorsTypesBelow);
  }

}
int * getAggregationRate(Element_topology * lpt){
  return(lpt->spec_top.node->aggregation_rate);
}

float getDelayUpperRouter(Element_topology * lpt){
  return(lpt->spec_top.node->delay_upper_router);
}
float getDelayLowerRouter(Element_topology * lpt){
  return(lpt->spec_top.node->delay_lower_router);
}

float getProbCommandResponse(Element_topology * lpt)
{
  return lpt->spec_top.node->probCommandResponse;
}

int getDiskType(Element_topology * lpt){
  return(lpt->spec_top.node->disk_type);
}

double * getDiskServices(Element_topology * lpt){
  return(lpt->spec_top.node->diskServices);
}

//_Element_topology *
int getType(Element_topology * lpt){
    return(lpt->lp_type);
}

int getUpperNode(Element_topology * lpt)
{
  return lpt->upperNode;
}
int getNumberLower(Element_topology * lpt)
{
  return lpt->numberOfLowerElements;
}

int * getLowers(Element_topology * lpt)
{
  return lpt->lowerElements;
}

//The following two seems like none uses them..
int getNumberLANS(Element_topology * lpt){
  return lpt->numberOfLANS;
}

int * getLANS(Element_topology * lpt){
  return lpt->connectedLans;
}
int * getListActuatorsByType(Element_topology * lpt, int actuatorType)
{
    int type = getType(lpt);
    int * array;
    if(type == 0){
      array = lpt->spec_top.node->ListActuatorsByType;
    }
    else if(type == 3){
      array = lpt->spec_top.wan->ListActuatorsByType;
    }
    else{
      array = lpt->spec_top.lan->ListActuatorsByType;
    }
    int * ATB = getActuatorTypesBelowList(lpt);

    int so_far = 0;
    for(int i = 0; i < actuatorType; i++){
      int how_many = ATB[i];
      so_far+=how_many;
    }
    int how_many = ATB[actuatorType];
    int * result = malloc(sizeof(int)* how_many);
    for(int k = 0; k < how_many; k++){
        result[k] = array[so_far+k];
    }

    return(result);
}

int * getListSensorsByType(Element_topology * lpt, int sensorType)
{

  int type = getType(lpt);
  int * array;
  if(type == 0){
    array = lpt->spec_top.node->ListSensorsByType;
  }
  else if(type == 3){
    array = lpt->spec_top.wan->ListSensorsByType;
  }
  else{
    array = lpt->spec_top.lan->ListSensorsByType;
  }

  int * ATB = getSensorsTypesBelowList(lpt);
  int so_far = 0;
  for(int i = 0; i < sensorType; i++){
    int how_many = ATB[i];
    so_far+=how_many;
  }
  int how_many = ATB[sensorType];
  int * result = malloc(sizeof(int)* how_many);
  for(int k = 0; k < how_many; k++){
      result[k] = array[so_far+k];

  }

  return(result);
}

//#########################################
//TOPOLOGY
int getTotalNodes(general_topology * genTop)
{
  return genTop->total_nodes;
}

int getSensorNodes(general_topology * genTop)
{
  return genTop->sensor_nodes;
}

int getActuatorNodes(general_topology * genTop)
{
  return genTop->actuator_nodes;
}

int getNumberOfTotalLANs(general_topology * genTop)
{
  return genTop->numberOfTotalLANs;
}

int getNumberOfTotalWANs(general_topology * genTop)
{
  return genTop->numberOfTotalWANs;
}
int getNumberOfActTypes(general_topology * genTop)
{
  return genTop->numberOfActTypes;
}

int getNumberOfSensTypes(general_topology * genTop)
{
    return genTop->numberOfSensTypes;
}

int getNumberOfLANsTypes(general_topology * genTop)
{
  return genTop->numberOfLANsTypes;
}

double * getProbOfActuators(general_topology * genTop)
{
  return genTop->probOfActuators;
}


int * getActuatorPaths(Element_topology * lpt){
  return(lpt->actuatorPaths);
}


//SETUP FUNCTIONS
void setLowerElements(Element_topology * lpt, int * lowerEle, int numberLower)
{
  lpt->numberOfLowerElements = numberLower;
  lpt->lowerElements = lowerEle;
}

void setLANs(Element_topology * lpt, int * arrayLANs, int numberLANs){
  lpt->numberOfLANS = numberLANs;
  lpt->connectedLans = arrayLANs;
}
void setArrayActuatorPaths(Element_topology * lpt, int * arrayActuatorPaths){
  lpt->actuatorPaths = arrayActuatorPaths;
}
void setListActuatorsByType(Element_topology * lpt, int ** array, int nt){
  int * ATB = getActuatorTypesBelowList(lpt);
  int total = getNumberOfBelowActuators(lpt);
  int * result = malloc(sizeof(int) *total);
  int so_far = 0;
  for(int i = 0; i < nt; i++){
    int how_many = ATB[i];
    for(int k = 0; k < how_many; k++){
      result[so_far+k] = array[i][k];
    }
    so_far+=how_many;

  }

  int type = getType(lpt);
  if(type == 0){
    lpt->spec_top.node->ListActuatorsByType = result;
  }
  else if(type == 3){
    lpt->spec_top.wan->ListActuatorsByType = result;
  }
  else{
    lpt->spec_top.lan->ListActuatorsByType = result;
  }
}
void setListSensorsByType(Element_topology * lpt, int ** array, int nts){
  int * ATB = getSensorsTypesBelowList(lpt);
  int total = getNumberOfBelowSensors(lpt);
  int * result = malloc(sizeof(int) *total);
  int so_far = 0;
  for(int i = 0; i < nts; i++){
    int how_many = ATB[i];
    for(int k = 0; k < how_many; k++){
      result[so_far+k] = array[i][k];
    }
    so_far+=how_many;
  }
  int type = getType(lpt);
  if(type == 0){
    lpt->spec_top.node->ListSensorsByType = result;
  }
  else if(type == 3){
    lpt->spec_top.wan->ListSensorsByType = result;
  }
  else{
    lpt->spec_top.lan->ListSensorsByType = result;
  }
}
void setWANdown(Element_topology * lpt, int idWAN){
  lpt->spec_top.node->id_WAN_down = idWAN;
}
void setWANup(Element_topology * specific_lpt,Element_topology ** lpt){
  int upp = getUpperNode(specific_lpt);
  if(upp == -1){
    specific_lpt->spec_top.node->id_WAN_up = -1;
  }
  else{
    int uppType =getType(lpt[upp]);
    if(uppType == 3){
      specific_lpt->spec_top.node->id_WAN_up = upp;
    }
  }
}
void setSensorRates(Element_topology * lpt, double * array){
  lpt->spec_top.sensor->sensorRates = array;
}

void setSensorTypes(Element_topology * lpt, int * array, int nts){
  int type = getType(lpt);
  int total = 0;
  for(int count = 0; count < nts; count++){
    total+=array[count];
  }
  if(type == 0){
    lpt->spec_top.node->sensorsTypesBelow = array;
    lpt->spec_top.node->numberOfBelowSensors = total;
  }
  else if(type == 3){
    lpt->spec_top.wan->sensorsTypesBelow = array;
    lpt->spec_top.wan->numberOfBelowSensors = total;
  }
  else{
    lpt->spec_top.lan->sensorsTypesBelow = array;
    lpt->spec_top.lan->numberOfBelowSensors = total;

  }

}
void setActuatorTypes(Element_topology * lpt, int * array, int nt){
  int total = 0;
  for(int count = 0; count < nt; count++){
    total+=array[count];
  }

  int type = getType(lpt);
  if(type == 0){
    lpt->spec_top.node->actuatorsTypesBelow = array;
    lpt->spec_top.node->numberOfBelowActuators = total;
  }
  else if(type == 3){
    lpt->spec_top.wan->actuatorsTypesBelow = array;
    lpt->spec_top.wan->numberOfBelowActuators = total;
  }
  else{
    lpt->spec_top.lan->actuatorsTypesBelow = array;
    lpt->spec_top.lan->numberOfBelowActuators = total;
  }

}

void setLANserviceTimes(Element_topology * lpt,double * LANsINserviceTimes, double * LANsOUTserviceTimes){
  lpt->spec_top.lan->LANsINserviceTimes = LANsINserviceTimes;
  lpt->spec_top.lan->LANsOUTserviceTimes = LANsOUTserviceTimes;
}
