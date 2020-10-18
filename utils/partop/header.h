#ifndef HEADER_H
#define HEADER_H
#include "../application_datatypes.h"
//PSA: "directly" means that the elements is the next hop in
//any direction, the direction is specified by the function.
//##################################
//specific topology

//Return the type of actuator index
int getActuatorType(LP_topology * lpt);
//Return the type of sensor index
int getSensorType(LP_topology * lpt);
int getLanType(LP_topology * lpt);
int getTypeJob(LP_topology * lpt);
int getMeasureType(LP_topology * lpt);
int getLANup(LP_topology * lpt);
//Rate transition of actuators
double getRateTransition(LP_topology * lpt);
//Service time for commands messages for actuators
double getServiceTimeCommand(LP_topology * lpt);
int getWanType(LP_topology * lpt);
int getLanType(LP_topology * lpt);
//Delay of WAN/LAN
float getDelay(LP_topology * lpt);
//Array of service times for lan in
double * getLANsINserviceTimesByType(LP_topology * lpt);
//Same but for LAN OUT
double * getLANsOUTserviceTimesByType(LP_topology * lpt);

//CENTRAL/REGIONAL/LOCAL
int getNodeType(LP_topology * lpt);
//Returns the list of service times for the element index
double * getServiceRates(LP_topology * lpt);
int getScheduler(LP_topology * lpt);
int getNumberOfBelowActuators(LP_topology * lpt);
//Retrieve a list where the i-th represents how many actuators
//of type i the element index has.
int * getActType(LP_topology * lpt);
int getWanUp(LP_topology * lpt);
int getWanDown(LP_topology * lpt);
//Same as previous functions, but for sensors.
int getNumberOfBelowSensors(LP_topology * lpt);
int * getSensType(LP_topology * lpt);
int * getAggregationRate(LP_topology * lpt);
float getDelayUpperRouter(LP_topology * lpt);
float getDelayLowerRouter(LP_topology * lpt);
//Return probability of the node index of reacting to a TRANSITION with a COMMAND
float getProbCommandResponse(LP_topology * lpt);
int getDiskType(LP_topology * lpt);
//For the disk connected to the i-th node
double * getDiskServices(LP_topology * lpt);

//##################################
//_LP_topology *
//Return the type(NODE,SENSOR,ACTUATOR,WAN,LAN)
int getType(LP_topology * lpt);
//Retrieve upper node of index element
int getUpperNode(LP_topology * lpt);
//How many directly lower elements has the element index
int getNumberLower(LP_topology * lpt);
//Array of directly below elements
int * getLowers(LP_topology * lpt);
//How many LANS the node directly manages
int getNumberLANS(LP_topology * lpt);
//Array of those LANs.
int * getLANS(LP_topology * lpt);
//Returns the list of actuators of type actuatorType which are below(even undirectly)
int * getListActuatorsByType(LP_topology * lpt, int actuatorType);
//Same but with sensors
int * getListSensorsByType(LP_topology * lpt, int sensorType);
//#################################

//Create topology from txt file in path
total_topology *  getTopology(char * path);
//topology struct GETs, first 8 are general informations
//self-explanatory.
int getTotalNodes(general_topology * genTop);
int getSensorNodes(general_topology * genTop);
int getActuatorNodes(general_topology * genTop);
int getNumberOfTotalLANs(general_topology * genTop);
int getNumberOfTotalWANs(general_topology * genTop);
int getNumberOfActTypes(general_topology * genTop);
int getNumberOfSensTypes(general_topology * genTop);
int getNumberOfLANsTypes(general_topology * genTop);
//array of actuator's probability of receiving a command, ordered by act type
double * getProbOfActuators(general_topology * genTop);

//Returns the matrix of paths to reach actuators.
//Indeed an element [x][y] represents the next hop that the element x needs
//to send the message to make it arrive at the actuator with id y.
int * getActuatorPaths(LP_topology * lpt);
//These function are self explanatory but should not be used outside of first run
//by parser.c
void setSensorRates(LP_topology * lpt, double * array);
void setLowerElements(LP_topology *  lpt, int * lowerEle, int numberLower);
void setLANs(LP_topology *  lpt, int * arrayLANs, int numberLANs);
void setWANdown(LP_topology * lpt, int idWAN);
void setWANup(LP_topology * lpt);
void setArrayActuatorPaths(LP_topology * lpt, int * arrayActuatorPaths);
void setSensorTypes(LP_topology * lpt, int * array, int nts);
void setActuatorTypes(LP_topology * lpt, int * array, int nt);
void setListActuatorsByType(LP_topology * lpt, int ** array);
void setListSensorsByType(LP_topology * lpt, int ** array);
void setLANserviceTimes(LP_topology * lpt,double * LANsINserviceTimes, double * LANsOUTserviceTimes);

#endif /* HEADER_H */
