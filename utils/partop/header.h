#ifndef HEADER_H
#define HEADER_H
#include "../application_datatypes.h"

topology * getTopology(char * path);
int getNumberReceiv(topology * top, int index);
int getNumberSender(topology * top, int index);

int * getNext(topology * top, int next);
int * getPrevious(topology * top, int prev);
void * getInfo(topology * top, int index);

#endif /* HEADER_H */
