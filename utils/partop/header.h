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
void * getInfo(topology * top, int index);

#endif /* HEADER_H */
