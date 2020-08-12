#ifndef HEADER_H
#define HEADER_H
#include "../application_datatypes.h"

topology * getTopology(char * path);
int getNumberReceiv(topology * top, int index);

int * getNext(topology * top, int next);
void * getInfo(topology * top, int index);

#endif /* HEADER_H */
