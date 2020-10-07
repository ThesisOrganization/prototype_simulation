#ifndef HEADER_H
#define HEADER_H
#include "../application_datatypes.h"

topology * getTopology(char * path);
int getUpperNode(topology * top, int index);
int getNumberLower(topology * top, int index);
int * getLowers(topology * top, int next);
int getNumberLANS(topology * top, int index);
int * getLANS(topology * top, int index);
double ** getSensorRatesByType(topology * top);
double * getSensorRatesForOneSensorType(topology * top, int index);
double * getServiceRates(topology * top, int index);
void * getInfo(topology * top, int index);
double ** getLANServiceTimesByType(topology * top);
double * getLANServiceTimesForOneLANType(topology * top, int index);
int getType(topology * top, int index);
int ** getActuatorPaths(topology * top);
int * getActuatorPathsIndex(topology * top, int index);
int * getActType(topology * top,int index);
int getActuatorType(topology * top, int index);
#endif /* HEADER_H */
