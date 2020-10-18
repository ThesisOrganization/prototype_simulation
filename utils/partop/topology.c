#include <stdio.h>
#include "header.h"
//GETs from specific_topology

int getNodeType(LP_topology * lpt){
  return(lpt->spec_top->node.node_type);
}

int getActuatorType(LP_topology * lpt){
    return(lpt->spec_top->actuator.actuator_type);
}

int getSensorType(LP_topology * lpt){
    return(lpt->spec_top->sensor.sensor_type);
}

int getTypeJob(LP_topology * lpt){
  int type = getType(lpt);
  if(type == 1){
    return(lpt->spec_top->sensor.type_job);

  }
  else{
    return(lpt->spec_top->actuator.type_job);
  }
}

int getMeasureType(LP_topology * lpt){
  int type = getType(lpt);
  if(type == 1){
    return(lpt->spec_top->sensor.measure_type);

  }
  else{
    return(lpt->spec_top->actuator.measure_type);
  }

}

int getLANup(LP_topology * lpt){
  int type = getType(lpt);
  if(type == 1){
    return(lpt->spec_top->sensor.id_LAN_up);

  }
  else{
    return(lpt->spec_top->actuator.id_LAN_up);
  }
}

double getRateTransition(LP_topology * lpt){
  return(lpt->spec_top->actuator.rateTransition);
}

double getServiceTimeCommand(LP_topology * lpt){
  return(lpt->spec_top->actuator.serviceTimeCommand);
}

int getWanType(LP_topology * lpt){
  return(lpt->spec_top->wan.wan_type);
}

int getLanType(LP_topology * lpt){
  return(lpt->spec_top->lan.lan_type);
}

float getDelay(LP_topology * lpt){
  return(lpt->spec_top->wan.delay);
}
double * getLANsINserviceTimesByType(LP_topology * lpt){
  return(lpt->spec_top->lan.LANsINserviceTimes);
}

double * getLANsOUTserviceTimesByType(LP_topology * lpt){
  return(lpt->spec_top->lan.LANsOUTserviceTimes);
}


double * getServiceRates(LP_topology * lpt){
  return(lpt->spec_top->node.service_time);
}

int getScheduler(LP_topology * lpt){
  return(lpt->spec_top->node.scheduler);
}

int getNumberOfBelowActuators(LP_topology * lpt){
  return(lpt->spec_top->node.numberOfBelowActuators);
}

int * getActType(LP_topology * lpt){
    return(lpt->spec_top->node.actuatorsTypesBelow);
}
int getWanUp(LP_topology * lpt){
  return(lpt->spec_top->node.id_WAN_up);
}

int getWanDown(LP_topology * lpt){
  return(lpt->spec_top->node.id_WAN_down);
}

int getNumberOfBelowSensors(LP_topology * lpt){
  return(lpt->spec_top->node.numberOfBelowSensors);
}


int * getSensType(LP_topology * lpt){
  return(lpt->spec_top->node.sensorsTypesBelow);
}

int * getAggregationRate(LP_topology * lpt){
  return(lpt->spec_top->node.aggregation_rate);
}

float getDelayUpperRouter(LP_topology * lpt){
  return(lpt->spec_top->node.delay_upper_router);
}
float getDelayLowerRouter(LP_topology * lpt){
  return(lpt->spec_top->node.delay_lower_router);
}


float getProbCommandResponse(LP_topology * lpt)
{
  return lpt->spec_top->node.probCommandResponse;
}

int getDiskType(LP_topology * lpt){
  return(lpt->spec_top->node.disk_type);
}

double * getDiskServices(LP_topology * lpt){
  return(lpt->spec_top->node.diskServices);
}

//_LP_topology *
int getType(LP_topology * lpt){
    return(lpt->lp_type);
}

int getUpperNode(LP_topology * lpt)
{
  return lpt->upperNode;
}
int getNumberLower(LP_topology * lpt)
{
  return lpt->numberOfLowerElements;
}

int * getLowers(LP_topology * lpt)
{
  return lpt->lowerElements;
}

//The following two seems like none uses them..
int getNumberLANS(LP_topology * lpt){
  return lpt->numberOfLANS;
}

int * getLANS(LP_topology * lpt){
  return lpt->connectedLans;
}
int * getListActuatorsByType(LP_topology * lpt, int actuatorType)
{
    return lpt->ListActuatorsByType[actuatorType];
}

int * getListSensorsByType(LP_topology * lpt, int sensorType)
{
    return lpt->ListSensorsByType[sensorType];
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


int * getActuatorPaths(LP_topology * lpt){
  return(lpt->actuatorPaths);
}


//SETUP FUNCTIONS
void setLowerElements(LP_topology * lpt, int * lowerEle, int numberLower)
{
  lpt->numberOfLowerElements = numberLower;
  lpt->lowerElements = lowerEle;
}

void setLANs(LP_topology * lpt, int * arrayLANs, int numberLANs){
  lpt->numberOfLANS = numberLANs;
  lpt->connectedLans = arrayLANs;
}
void setArrayActuatorPaths(LP_topology * lpt, int * arrayActuatorPaths){
  lpt->actuatorPaths = arrayActuatorPaths;
}
void setListActuatorsByType(LP_topology * lpt, int ** array){
  lpt->ListActuatorsByType = array;
}
void setListSensorsByType(LP_topology * lpt, int ** array){
  lpt->ListSensorsByType = array;
}
void setWANdown(LP_topology * lpt, int idWAN){
  lpt->spec_top->node.id_WAN_down = idWAN;
}
void setWANup(LP_topology * lpt){
  int upp = getUpperNode(lpt);
  if(upp == -1){
    lpt->spec_top->node.id_WAN_up = -1;
  }
  else{
    int uppType =getType(lpt);
    if(uppType == 3){
      lpt->spec_top->node.id_WAN_up = upp;
    }
  }
}
void setSensorRates(LP_topology * lpt, double * array){
  lpt->spec_top->sensor.sensorRates = array;
}

void setSensorTypes(LP_topology * lpt, int * array, int nts){
  int type = getType(lpt);
  int total = 0;
  for(int count = 0; count < nts; count++){
    total+=array[count];
  }

  if(type == 0){
    lpt->spec_top->node.sensorsTypesBelow = malloc(sizeof(int)*nts);
    lpt->spec_top->node.sensorsTypesBelow = array;
    lpt->spec_top->node.numberOfBelowSensors = total;
  }
  else if(type == 3){
    lpt->spec_top->wan.sensorsTypesBelow = array;
    lpt->spec_top->wan.numberOfBelowSensors = total;
  }
  else{
    lpt->spec_top->lan.sensorsTypesBelow = array;
    lpt->spec_top->lan.numberOfBelowSensors = total;
  }

}
void setActuatorTypes(LP_topology * lpt, int * array, int nt){
  int total = 0;
  for(int count = 0; count < nt; count++){
    total+=array[count];
  }
  lpt->spec_top->node.actuatorsTypesBelow = array;
  lpt->spec_top->node.numberOfBelowActuators = total;
}

void setLANserviceTimes(LP_topology * lpt,double * LANsINserviceTimes, double * LANsOUTserviceTimes){
  lpt->spec_top->lan.LANsINserviceTimes = LANsINserviceTimes;
  lpt->spec_top->lan.LANsOUTserviceTimes = LANsOUTserviceTimes;
}
