#include <stdio.h>
#include "header.h"
#include "../idmap/idmap.h"
#include<stdlib.h>
#include<string.h>

//GETs from specific_topology

general_topology * getGenTopology(total_topology * totTop){
  return(totTop->gn);
}
Element_topology ** getLPTopologyComplete(total_topology * totTop){
  return(totTop->lpt);
}
Element_topology * getLPTopology(total_topology * totTop, int index){
  return(totTop->lpt[index]);
}
int * getAmountsOfElements(lp_topology * lptop){
  return(lptop->amountsOfElementsInLP);
}
int  getAmountsOfElementsInOneLP(lp_topology * lptop,int index){
  return(lptop->amountsOfElementsInLP[index]);
}
int ** getLPtoElementMapping(lp_topology * lptop){
  return(lptop->LPtoElementMapping);
}
int * getLPtoElementMappingOneLP(lp_topology * lptop,int index){
  return(lptop->LPtoElementMapping[index]);
}
idmap * getElementToLPMapping(lp_topology * lptop){
  return(lptop->ElementToLPMapping);
}
int getElementToLPMappingOneElement(lp_topology * lptop,int index){
  int valid = getNumValid(lptop);
  idmap * idm = getElementToLPMapping(lptop);
  int result = idmap_search(idm,index,valid);
  return(result);
}

int getNumLP(lp_topology * lptop){
  return(lptop->numLP);
}
int getNumValid(lp_topology * lptop){
  return(lptop->numValid);
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
int getNumberOfCores(Element_topology * lpt){
  return(lpt->spec_top.node->cores);
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
int getNumValidElToLP(Element_topology * lpt)
{
  return(lpt->numValidElToLP);
}

idmap * getElToLPMapping(Element_topology * lpt){
  return(lpt->ElementToLPMapping);
}

int getElToLPMappingOneElement(Element_topology * lpt,int index)
{
  int valid = getNumValidElToLP(lpt);
  idmap * idm = getElToLPMapping(lpt);
  int result = idmap_search(idm,index,valid);
  return(result);
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
		return array+so_far;
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
  return array+so_far;
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

int getNumValidActuatorPaths(Element_topology * lpt){
  return(lpt->numValidActuatorPaths);
}
idmap * getActuatorPaths(Element_topology * lpt){
  return(lpt->actuatorPaths);
}

int getActuatorPathsIndex(Element_topology * lpt, int index){
  int valid = getNumValidActuatorPaths(lpt);
  idmap * idm = getActuatorPaths(lpt);
  int result = idmap_search(idm,index,valid);
  return result;
}
double getCostDisk(Element_topology * lpt){
  return(lpt->spec_top.node->disk_cost);
}
double getCost(Element_topology * lpt){
  return(lpt->cost);
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
void setArrayActuatorPaths(Element_topology * lpt, idmap* arrayActuatorPaths,int valid){
  idmap * results = malloc(sizeof(idmap)*valid);
  memcpy(results, arrayActuatorPaths, sizeof(idmap)*valid);
  lpt->actuatorPaths = results;
  lpt->numValidActuatorPaths = valid;
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
void setSensorRates(Element_topology * lpt, double * array, int size){
  double * results = malloc(size);
  memcpy(results, array, size);
  lpt->spec_top.sensor->sensorRates = results;
}

void setSensorTypes(Element_topology * lpt, int * array, int nts,int size){
  int * results = malloc(size);

  memcpy(results, array, size);

  int type = getType(lpt);
  int total = 0;
  for(int count = 0; count < nts; count++){
    total+=results[count];
  }
  if(type == 0){
    lpt->spec_top.node->sensorsTypesBelow = results;
    lpt->spec_top.node->numberOfBelowSensors = total;
  }
  else if(type == 3){
    lpt->spec_top.wan->sensorsTypesBelow = results;
    lpt->spec_top.wan->numberOfBelowSensors = total;
  }
  else{
    lpt->spec_top.lan->sensorsTypesBelow = results;
    lpt->spec_top.lan->numberOfBelowSensors = total;
  }

}
void setActuatorTypes(Element_topology * lpt, int * array, int nt, int size){
  int * results = malloc(size);
  memcpy(results, array, size);

  int total = 0;
  for(int count = 0; count < nt; count++){
    total+=array[count];
  }

  int type = getType(lpt);
  if(type == 0){
    lpt->spec_top.node->actuatorsTypesBelow = results;
    lpt->spec_top.node->numberOfBelowActuators = total;
  }
  else if(type == 3){
    lpt->spec_top.wan->actuatorsTypesBelow = results;
    lpt->spec_top.wan->numberOfBelowActuators = total;
  }
  else{
    lpt->spec_top.lan->actuatorsTypesBelow = results;
    lpt->spec_top.lan->numberOfBelowActuators = total;
  }

}

void setLANserviceTimes(Element_topology * lpt,double * LANsINserviceTimes, double * LANsOUTserviceTimes, int size1, int size2){
  double * results1 = malloc(size1);
  memcpy(results1, LANsINserviceTimes, size1);

  double * results2 = malloc(size2);
  memcpy(results2, LANsOUTserviceTimes, size2);

  lpt->spec_top.lan->LANsINserviceTimes = results1;
  lpt->spec_top.lan->LANsOUTserviceTimes = results2;
}

void destroyLPTopology(lp_topology * lptop){
  free(lptop->ElementToLPMapping);
  for(int i = 0; i < lptop->numLP; i++){
    free(lptop->LPtoElementMapping[i]);
  }
  free(lptop->LPtoElementMapping);
  free(lptop->amountsOfElementsInLP);
}

void destroyGeneralTopology(general_topology * gn){
  free(gn->probOfActuators);
}
void destroyElementTopologyArray(Element_topology ** lpt,int total_elements){
  for(int i = 0; i < total_elements; i++){

    free(lpt[i]->lowerElements);
    free(lpt[i]->connectedLans);
    free(lpt[i]->actuatorPaths);
    free(lpt[i]->ElementToLPMapping);
    int type = getType(lpt[i]);
    if(type == 0){//node
      if(getNodeType(lpt[i]) == 0){//central
        free(lpt[i]->spec_top.node->diskServices);
      }
      free(lpt[i]->spec_top.node->service_time);
      free(lpt[i]->spec_top.node->aggregation_rate);
      free(lpt[i]->spec_top.node->actuatorsTypesBelow);
      free(lpt[i]->spec_top.node->sensorsTypesBelow);
      free(lpt[i]->spec_top.node->ListSensorsByType);
      free(lpt[i]->spec_top.node->ListActuatorsByType);
      free(lpt[i]->spec_top.node);
    }
    else if (type == 1){//sensor
      free(lpt[i]->spec_top.sensor->sensorRates);
      free(lpt[i]->spec_top.sensor);
    }
    else if (type ==  2){//actuator
      free(lpt[i]->spec_top.actuator);
    }
    else if (type == 3){//wan
      free(lpt[i]->spec_top.wan->actuatorsTypesBelow);
      free(lpt[i]->spec_top.wan->sensorsTypesBelow);
      free(lpt[i]->spec_top.wan->ListSensorsByType);
      free(lpt[i]->spec_top.wan->ListActuatorsByType);
      free(lpt[i]->spec_top.wan);
    }
    else if (type == 4){//lan
      free(lpt[i]->spec_top.lan->LANsINserviceTimes);
      free(lpt[i]->spec_top.lan->LANsOUTserviceTimes);
      free(lpt[i]->spec_top.lan->actuatorsTypesBelow);
      free(lpt[i]->spec_top.lan->sensorsTypesBelow);
      free(lpt[i]->spec_top.lan->ListSensorsByType);
      free(lpt[i]->spec_top.lan->ListActuatorsByType);
      free(lpt[i]->spec_top.lan);
    }
    free(lpt[i]);
  }

}
//DESTROYS
void destroyTotalTopology(total_topology * totTop){

  int total_elements = getTotalNodes(getGenTopology(totTop))+ getSensorNodes(getGenTopology(totTop))+ getActuatorNodes(getGenTopology(totTop))+ getNumberOfTotalLANs(getGenTopology(totTop))+ getNumberOfTotalWANs(getGenTopology(totTop));
  destroyGeneralTopology(getGenTopology(totTop));
  free(totTop->gn);
  destroyLPTopology(totTop->lp_topology);
  free(totTop->lp_topology);
  destroyElementTopologyArray(getLPTopologyComplete(totTop),total_elements);
  free(totTop->lpt);
  free(totTop);

}
