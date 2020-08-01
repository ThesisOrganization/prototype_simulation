#ifndef HEADER_H
#define HEADER_H

typedef struct _topArray
{
int numberOfReceivers;
int numberInfos;

int * receiver;
void * info;

} topArray;

typedef struct _topology{
  int total_nodes;
  int sensor_nodes;
  topArray ** topArr; //array of poiters to topArray
} topology;

topology * getTopology(char * path, void* (*converting)(char **) );
int getNumberReceiv(topology * top, int index);
int getNumberInfos(topology * top, int index);

int * getNext(topology * top, int next);
void * getInfo(topology * top, int index);

#endif /* HEADER_H */
