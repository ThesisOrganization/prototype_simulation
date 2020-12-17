#ifndef HEADER_H
#define HEADER_H
#include "../idmap/idmap.h"
#include "partop_datatypes.h"

///The LP aggregation criteria
typedef enum _lp_aggregation_criteria{
	LP_AGGR_NONE=0, /// disables the generation of the LP.txt file
	LP_AGGR_REGIONAL, /// one lp per region, with the central node inside the first region
	LP_AGGR_LOCAL, ///one lp per local node, with the central and regional node in an lp each (WAN included)
	LP_AGGR_LAN ///one lp per lan with regional, local and central node in one lp each (WAN included)
} lp_aggregation_criteria;

//PSA: "directly" means that the elements is the next hop in
//any direction, the direction is specified by the function.
//##################################
general_topology * getGenTopology(total_topology * totTop);
Element_topology * getLPTopology(total_topology * totTop, int index);
Element_topology ** getLPTopologyComplete(total_topology * totTop);
lp_topology * getLPtopoogy(char * path);
//returns the whole matrix where the [i][y] is the y-th element contained in LP i
int ** getLPtoElementMapping(lp_topology * lptop);
//same but returns only the array for the LP i
int * getLPtoElementMappingOneLP(lp_topology * lptop,int index);
//returns the whole idmap for element->LP
idmap * getElementToLPMapping(lp_topology * lptop);
//returns the id of the LP for the elementh index
int getElementToLPMappingOneElement(lp_topology * lptop,int index);
int getNumLP(lp_topology * lptop);
//for idmap Ele->LP
int getNumValid(lp_topology * lptop);
//returns how many elements the LP index has mapped into.
int * getAmountsOfElements(lp_topology * lptop);
int getAmountsOfElementsInOneLP(lp_topology * lptop,int index);
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
int getNumberOfCores(Element_topology * lpt);
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
int getNumValidElToLP(Element_topology * lpt);
idmap * getElToLPMapping(Element_topology * lpt);
//returns the LP of the element index IF the element index is directly
//reachable for this element.
int getElToLPMappingOneElement(Element_topology * lpt,int index);
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
total_topology *  getTopology(char * path,char * path1,lp_aggregation_criteria aggregation_criteria);
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

int getNumValidActuatorPaths(Element_topology * lpt);
idmap * getActuatorPaths(Element_topology * lpt);
int getActuatorPathsIndex(Element_topology * lpt, int index);
//These function are self explanatory but should not be used outside of first run
//by parser.c
void setSensorRates(Element_topology * lpt, double * array, int size);
void setLowerElements(Element_topology *  lpt, int * lowerEle, int numberLower);
void setLANs(Element_topology *  lpt, int * arrayLANs, int numberLANs);
void setWANdown(Element_topology * lpt, int idWAN);
void setWANup(Element_topology * specific_lpt,Element_topology ** lpt);
void setArrayActuatorPaths(Element_topology * lpt, idmap * arrayActuatorPaths,int valid);
void setSensorTypes(Element_topology * lpt, int * array, int nts,int size);
void setActuatorTypes(Element_topology * lpt, int * array, int nt, int size);
void setListActuatorsByType(Element_topology * lpt, int ** array, int nt);
void setListSensorsByType(Element_topology * lpt, int ** array, int nts);
void setLANserviceTimes(Element_topology * lpt,double * LANsINserviceTimes, double * LANsOUTserviceTimes, int size1, int size2);
void setSensorTypes(Element_topology * lpt, int * array, int nts,int size);
//DESTROYS
void destroyGeneralTopology(general_topology * gn);
void destroyElementTopologyArray(Element_topology ** lpt,int total_elements);
void destroyTotalTopology(total_topology * totTop);
void destroyLPTopology(lp_topology * lptop);

//parseBin
Element_topology ** parseBinaryElementTopology(int id, int numElements);
general_topology * parseBinaryGeneralTopology();
lp_topology * parseBinaryLpTopology();
lp_topology * parseBinaryLpTopologyOneLPStripped(int lp);
void destroyBinaryLpTopologyOneLPStripped(lp_topology* lp_top);

void generateBinaries_elementTopology(general_topology * genTop,total_topology * totTop, int * elonelp, int a, int num,char* path);
void generateBinaries_generalTopology(total_topology * totTop,char* path);
void generateBinaries_lpTopology(lp_topology * lpTop,int totalElements,char* path);
void generateBinaries(general_topology * genTop,total_topology * totTop,int totalElements, lp_topology * lpTop,char* path);


#endif /* HEADER_H */
