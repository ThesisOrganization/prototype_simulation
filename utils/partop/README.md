# Utils/partop
This folder will contain modules to parse input file and get informations in the relative data structure
## Syntax of text file
Being a work in progress project, this is subject to frequent changes and won't be a final version:
4
2
1;2;3,4;3;NODE,SCHEDULER2,14.5
2;1;4;2;SENSOR,REAL_TIME
...

First line: number of nodes
Second line: number of sensor nodes
Sender node, #receiver nodes,{receiver nodes},#informations,{informations}

{informations} is different based on the type of node:
Sensor: Scheduler type, response time
Node: type of job created

## Data structure 1
TopArray is composed by:
nn : integer, number of sender nodes in the topology;
numberOfReceivers: integer, self explanatory;
receiver: array of integers containing the receivers
info: array of strings containing the informations

## DS 2
generalTopology is an additional data structure, used to give general informations, composed :
total_nodes: integer, will probably involve in only counting the non-sensor nodes;
sensor_nodes: integer, number of sensor nodes;
topArray * topArr: contains an array of TopArray structs, one for each sensor.

## DS 3
parsingStruct is a temporary data structure, used to uniform this module to the application.c, which dispose the informatios in a simpler way and confirming it to the lp_info data structure

int lp_type : Node/Sensor
int type_job : only for sensors, BATCH/REAL TIME/LOSSY;
int scheduler : Nodes only, which scheduler used, for now SCHEDULER1,SCHEDULER2, SCHEDULER3;
float response_time: for nodes, the service response time;



## Usage
This module should be used at startup with the function in parser.c
getTopology(); returns a struct with general informations and an array of topArray one for each sensor.
Then in topology.c all useful retrieval functions are written, for now need to pass the topArray array, if needed will be modified to accept generalTopology struct.
