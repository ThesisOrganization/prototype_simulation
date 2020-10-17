#ifndef HEADER_H
#define HEADER_H
#include "../application_datatypes.h"
//PSA: "directly" means that the elements is the next hop in
//any direction, the direction is specified by the function.
//##################################
//info
//Return the type(NODE,SENSOR,ACTUATOR,WAN,LAN)
int getType(topology * top, int index);
//Return the type of actuator index
int getActuatorType(topology * top, int index);
//Return the type of sensor index
int getSensorType(topology * top, int index);
int getTypeJob(topology * top, int index);
int getMeasureType(topology * top, int index);
int getLANup(topology * top, int index);
//Rate transition of actuators
double getRateTransition(topology * top, int index);
//Service time for commands messages for actuators
double getServiceTimeCommand(topology * top, int index);
int getWanType(topology * top, int index);
int getLanType(topology * top, int index);
//Delay of WAN/LAN
float getDelay(topology * top, int index);
//CENTRAL/REGIONAL/LOCAL
int getNodeType(topology * top, int index);
//Returns the list of service times for the element index
double * getServiceRates(topology * top, int index);
int getScheduler(topology * top, int index);
int getNumberOfBelowActuators(topology * top, int index);
//Retrieve a list where the i-th represents how many actuators
//of type i the element index has.
int * getActType(topology * top,int index);
int getWanUp(topology * top, int index);
int getWanDown(topology * top, int index);
//Same as previous functions, but for sensors.
int getNumberOfBelowSensors(topology * top, int index);
int * getSensType(topology * top,int index);
int * getAggregationRate(topology * top, int index);
float getDelayUpperRouter(topology * top, int index);
float getDelayLowerRouter(topology * top, int index);
//Return probability of the node index of reacting to a TRANSITION with a COMMAND
float getProbCommandResponse(topology * top, int index);
int getDiskType(topology * top, int index);
//For the disk connected to the i-th node
double * getDiskServices(topology * top, int index);

//##################################
//toparray
//Retrieve upper node of index element
int getUpperNode(topology * top, int index);
//How many directly lower elements has the element index
int getNumberLower(topology * top, int index);
//Array of directly below elements
int * getLowers(topology * top, int next);
//How many LANS the node directly manages
int getNumberLANS(topology * top, int index);
//Array of those LANs.
int * getLANS(topology * top, int index);
//In some cases, mainly in topology.c functions, it is useful to
//skip topology and interact directly with the informations struct
//of a specific index
void * getInfo(topology * top, int index);
//#################################

//Create topology from txt file in path
topology * getTopology(char * path);
//topology struct GETs, first 8 are general informations
//self-explanatory.
int getTotalNodes(topology * top);
int getSensorNodes(topology * top);
int getActuatorNodes(topology * top);
int getNumberOfTotalLANs(topology * top);
int getNumberOfTotalWANs(topology * top);
int getNumberOfActTypes(topology * top);
int getNumberOfSensTypes(topology * top);
int getNumberOfLANsTypes(topology * top);
//Returns the matrix of paths to reach actuators.
//Indeed an element [x][y] represents the next hop that the element x needs
//to send the message to make it arrive at the actuator with id y.
int ** getActuatorPaths(topology * top);
//Array of all next-hops of the element index, where the i-th is the next hop to
//arrive at actuator i.
int * getActuatorPathsIndex(topology * top, int index);
//Return a matrix where an element [x][y] represents the rate of a sensor
//of type [x] for the message of type [y] (either TELEMETRY or TRANSITION)
double ** getSensorRatesByType(topology * top);
//Returns the array of sensor rates for the sensor type index
double * getSensorRatesForOneSensorType(topology * top, int index);
//Returns a matrix where the element [x][y] is the service time of the LAN IN
//of type [x], message of type [y]
double ** getLANsINserviceTimesByType(topology * top);
//Same but for LAN OUT
double ** getLANsOUTserviceTimesByType(topology * top);
//Array where the i-th element is the service time for the LAN IN of type index
//for the message type i
double * getLANsINserviceTimesForOneLANType(topology * top, int index);
//Same, but for LAN OUT.
double * getLANsOUTserviceTimesForOneLANType(topology * top, int index);
//array of actuator's probability of receiving a command, ordered by act type
double * getProbOfActuators(topology * top);
//Returns the list of actuators of type actuatorType which are below(even undirectly)
//the node nodeId
int * getListActuatorsByType(topology * top, int nodeId, int actuatorType);
//Same but with sensors
int * getListSensorsByType(topology * top, int nodeId, int sensorType);


//These function are self explanatory but should not be used outside of first run
//by parser.c
void setLowerElements(topology * top, int * lowerEle, int numberLower, int index);
void setLANs(topology * top, int * arrayLANs, int numberLANs, int index);
void setWANdown(topology * top, int idWAN, int index);
void setWANup(topology * top, int index);
void setSensorTypes(topology * top, int * array, int index, int nts);
void setActuatorTypes(topology * top, int * array, int index, int nt);

#endif /* HEADER_H */
