# Utils/partop
This folder will contain modules to parse input file and get informations in the relative data structure
## Syntax of text file
Being a work in progress project, this is subject to frequent changes and won't be a final version:
7
4
4
3
4
2
2
0.33,0.10;0.24,0.12
0.34;0.28
0;2;1,2;-1;1;-1;11;NODE,SCHEDULER1,CENTRAL,-1,17,14,0.8,0.3,0.82/0.35/0.1/0.3,3/3,4
7;1;-1;3;1;-1;5;SENSOR,BATCH,SENSOR_TYPE0,MEASURE0,20
16;1;-1;5;1;-1;5;ACTUATOR,REAL_TIME,ACTUATOR_TYPE1,MEASURE1,23
17;2;1,2;0;1;-1;3;WAN,WAN_TYPE0,0.82
20;4;7,8,9,10;3;1;-1;3;LAN,LAN_TYPE0,0.42


First line: number of nodes
Second line: number of sensors
3rd: # actuators
4 line: number of WANs
5 line: number of LANs
6 line: # types actuators
7 line: # types sensors
8th line: sensor type 0 transition rate,sensor type 0 telemetry rate;sensor type 1 tr. rate, ...
9th line: probability receiving actuator type 0;pr type 1;...

id element, #lower nodes,{lower nodes},upper node,#LANs below, {LANs below},#informations,type of element,{informations}

{informations} is different based on the type of node:
Node: scheduler, type of node,id WAN up, id WAN below,  aggregation rate, delay upper router, delay lower router,  {service time telemtry/service time transition/service time command/service time batch send}, {# actuators type 0/...}, #sensors
Sensor/Actuator: type of job, Sensor/Actuator type, measure done, id LAN up
WAN:type of WAN[CENTRAL-REGIONAL/REGIONAL-LOCAL], delay
LAN:type of LAN[MANET/WIRELESS/ETC], delay

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
double response_time: for nodes, the service response time;
int node_type: Central/Regional/Network;
//The following are placeholders, while waiting for more specifics.
int sensor_type;
int actuator_type;
int measure_type;


## Usage
This module should be used at startup with the function in parser.c
getTopology(); returns a struct with general informations and an array of topArray one for each sensor.
Then in topology.c all useful retrieval functions are written, for now need to pass the topArray array, if needed will be modified to accept generalTopology struct.


##TODO
