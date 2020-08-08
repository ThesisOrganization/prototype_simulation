#ifndef HEADER_H
#define HEADER_H

#define NODE 1
#define SENSOR 2
#define ACTUATOR 3
#define SCHEDULER1 101
#define SCHEDULER2 102
#define SCHEDULER3 103
#define REAL_TIME 201
#define LOSSY 202
#define BATCH 203
#define SENSOR_TYPE0 301
#define SENSOR_TYPE1 302
#define ACTUATOR_TYPE0 401
#define ACTUATOR_TYPE1 402
#define CENTRAL 501
#define REGIONAL 502
#define NETWORK 503
#define MEASURE0 601
#define MEASURE1 602
#define MEASURE2 603

//temp struct, to make things uniform with application.c
//and check integrity in this main file
typedef struct _parsingStruct
{
int lp_type;
int type_job;
int scheduler;
double response_time;
int node_type;
int sensor_type;
int actuator_type;
int measure_type;
} parsingStruct;

typedef struct _topArray
{
int numberOfReceivers;
int * receiver;
void * info;
} topArray;

typedef struct _topology{
  int total_nodes;
  int sensor_nodes;
  topArray ** topArr; //array of poiters to topArray
} topology;

topology * getTopology(char * path);
int getNumberReceiv(topology * top, int index);

int * getNext(topology * top, int next);
void * getInfo(topology * top, int index);

#endif /* HEADER_H */
