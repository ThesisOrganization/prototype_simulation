#ifndef HEADER_H
#define HEADER_H
#include "../application_datatypes.h"
//PSA: "directly" means that the elements is the next hop in
//any direction, the direction is specified by the function.
//##################################
general_topology * getGenTopology(total_topology * totTop);
Element_topology * getLPTopology(total_topology * totTop, int index);
Element_topology ** getLPTopologyComplete(total_topology * totTop);
//specific topology

//Return the type of actuator index
int getActuatorType(Element_topology * lpt);
//Return the type of sensor index
int getSensorType(Element_topology * lpt);
int getLanType(Element_topology * lpt);
int getTypeJob(Element_topology * lpt);
int getMeasureType(Element_topology * lpt);
int getLANup(Element_topology * lpt);
double * getSensorRates(Element_topology * lpt);
//Rate transition of actuators
double getRateTransition(Element_topology * lpt);
//Service time for commands messages for actuators
double getServiceTimeCommand(Element_topology * lpt);
int getWanType(Element_topology * lpt);
int getLanType(Element_topology * lpt);
//Delay of WAN/LAN
float getDelay(Element_topology * lpt);
//Array of service times for lan in
double * getLANsINserviceTimesByType(Element_topology * lpt);
//Same but for LAN OUT
double * getLANsOUTserviceTimesByType(Element_topology * lpt);

//CENTRAL/REGIONAL/LOCAL
int getNodeType(Element_topology * lpt);
//Returns the list of service times for the element index, will be deprecated
//by getServiceTimesNodes
double * getServiceRates(Element_topology * lpt);
double * getServiceTimesNodes(Element_topology * lpt);
int getScheduler(Element_topology * lpt);
int getNumberOfBelowActuators(Element_topology * lpt);
//Retrieve a list where the i-th represents how many actuators
//of type i the element index has.
int * getActType(Element_topology * lpt);
int * getActuatorTypesBelowList(Element_topology * lpt);
int getWanUp(Element_topology * lpt);
int getWanDown(Element_topology * lpt);
//Same as previous functions, but for sensors.
int getNumberOfBelowSensors(Element_topology * lpt);
int * getSensType(Element_topology * lpt);
int * getSensorsTypesBelowList(Element_topology * lpt);
int * getAggregationRate(Element_topology * lpt);
float getDelayUpperRouter(Element_topology * lpt);
float getDelayLowerRouter(Element_topology * lpt);
//Return probability of the node index of reacting to a TRANSITION with a COMMAND
float getProbCommandResponse(Element_topology * lpt);
int getDiskType(Element_topology * lpt);
//For the disk connected to the i-th node
double * getDiskServices(Element_topology * lpt);

//##################################
//_Element_topology *
//Return the type(NODE,SENSOR,ACTUATOR,WAN,LAN)
int getType(Element_topology * lpt);
//Retrieve upper node of index element
int getUpperNode(Element_topology * lpt);
//How many directly lower elements has the element index
int getNumberLower(Element_topology * lpt);
//Array of directly below elements
int * getLowers(Element_topology * lpt);
//How many LANS the node directly manages
int getNumberLANS(Element_topology * lpt);
//Array of those LANs.
int * getLANS(Element_topology * lpt);
//Returns the list of actuators of type actuatorType which are below(even undirectly)
int * getListActuatorsByType(Element_topology * lpt, int actuatorType);
//Same but with sensors
int * getListSensorsByType(Element_topology * lpt, int sensorType);
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
int * getActuatorPaths(Element_topology * lpt);
//These function are self explanatory but should not be used outside of first run
//by parser.c
void setSensorRates(Element_topology * lpt, double * array);
void setLowerElements(Element_topology *  lpt, int * lowerEle, int numberLower);
void setLANs(Element_topology *  lpt, int * arrayLANs, int numberLANs);
void setWANdown(Element_topology * lpt, int idWAN);
void setWANup(Element_topology * specific_lpt,Element_topology ** lpt);
void setArrayActuatorPaths(Element_topology * lpt, int * arrayActuatorPaths);
void setSensorTypes(Element_topology * lpt, int * array, int nts);
void setActuatorTypes(Element_topology * lpt, int * array, int nt);
void setListActuatorsByType(Element_topology * lpt, int ** array, int nt);
void setListSensorsByType(Element_topology * lpt, int ** array, int nts);
void setLANserviceTimes(Element_topology * lpt,double * LANsINserviceTimes, double * LANsOUTserviceTimes);
//DESTROYS
void destroyGeneralTopology(general_topology * gn);
void destroyElementTopologyArray(Element_topology ** lpt,int total_elements);
void destroyTotalTopology(total_topology * totTop);



#endif /* HEADER_H */
