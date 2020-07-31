#ifndef HEADER_H
#define HEADER_H

typedef struct _topArray
{
int numberOfReceivers;
int numberInfos;

int * receiver;
char ** info;

} topArray;

typedef struct _topology{
  int total_nodes;
  int sensor_nodes;
  topArray ** topArr; //array of poiters to topArray
} topology;

topology * getTopology();
int getNumberReceiv(topology * top, int index);
int getNumberInfos(topology * top, int index);

int * getNext(topology * top, int next);
char ** getInfo(topology * top, int index);

#endif /* HEADER_H */
