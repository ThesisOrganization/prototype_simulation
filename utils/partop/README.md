# Utils/partop
This folder will contain modules to parse input file and get informations in the relative data structure.\
The module relies on application_datatypes.h for the definition of the data structures.
## Syntax of text file
Being a work in progress project, this is subject to frequent changes and won't be a final version:\
7\
4\
6\
3\
4\
2\
2\
3\
0.33,0.10;0.24,0.12\
0.42,0.25,0.11;0.182,0.14,0.23;0.535,0.71,0.53;\
0.34;0.28\
0;2;1,2;-1;1;-1;11;NODE,SCHEDULER1,CENTRAL,-1,17,14,0.8,0.3,0.82/0.35/0.1/0.3,3/3,4\
7;1;-1;3;1;-1;5;SENSOR,BATCH,SENSOR_TYPE0,MEASURE0,20\
16;1;-1;5;1;-1;5;ACTUATOR,REAL_TIME,ACTUATOR_TYPE1,MEASURE1,23\
17;2;1,2;0;1;-1;3;WAN,WAN_TYPE0,0.82\
20;4;7,8,9,10;3;1;-1;3;LAN,LAN_TYPE0,0.42\
\
\
First line: number of nodes\
Second line: number of sensors\
3rd: # actuators\
4 line: number of WANs\
5 line: number of LANs\
6 line: # types actuators\
7 line: # types sensors\
8th line # types of LANs\
9th line: sensor type 0 transition rate,sensor type 0 telemetry rate;sensor type 1 tr. rate, ...\
10th line:lan type1 service time transition,telemetry, command\
11th line: probability receiving actuator type 0;pr type 1;...\
\
id element, #lower nodes,{lower nodes},upper node,#LANs below, {LANs below},#informations,type of element,{informations}\
\
{informations} is different based on the type of node:
Node: scheduler, type of node,id WAN up, id WAN below,  aggregation rate, delay upper router, delay lower router,  {service time telemtry/service time transition/service time command/service time batch send}, {# actuators type 0/...}, #sensors\
Sensor/Actuator: type of job, Sensor/Actuator type, measure done, id LAN up\
Actuator: type of job, Actuator type, measure done, id LAN up,rate transition, service time command\
WAN:type of WAN[CENTRAL-REGIONAL/REGIONAL-LOCAL], delay\
LAN:type of LAN[MANET/WIRELESS/ETC], delay, {# actuators type 0/...}\

## Data structure 1
topology is composed by the general, useful informations:\
First 8 info that shouldn't need explaining:\
int total_nodes;\
int sensor_nodes;\
int actuator_nodes;\
int numberOfTotalLANs;\
int numberOfTotalWANs;\
int numberOfActTypes;\
int numberOfSensTypes;\
int numberOfLANsTypes;\
double ** sensorRatesByType: array of arrays containing the sensor rates of each type of sensor for each type of message(only telemetry and transition).\
double ** LANServiceTimesByType: same as sensorRatesByType but in this case it is the service time of each type of lan for each type of message(all 5, even if batch send should not be here, but for future updates).\
double * probOfActuators: probability of each type of actuator to receive a command message.
topArray ** topArr: array of topArray structs, see below.

## DS 2
topArray is an additional data structure, used to give informations about one element without specifics about the node's nature, composed :\
int upperNode: which node is directly above.\
int numberOfLowerElements: how many elements are directly below.\
int * lowerElements: array of elements below(skipping WANs/LANs)\
int numberOfLANS: how many LANs directly below.\
int * connectedLans: array of LANs below.\
void * info: see below.

## DS 3
lp_infos contains all other useful informations.\
\
int lp_type: Node, Sensor, Actuator, WAN, LAN\
\
int sensor_type;\
int actuator_type;\
int type_job;\
int measure_type;\
\
int wan_type;\
int lan_type;\
float delay;\
\
int node_type: CENTRAL, REGIONAL, LOCAL\
double * service_time: array of service time for each type of message\
int scheduler: which scheduler it is used\
int * actuatorsTypesBelow: how many actuators the node has below(even indirectly)\
int id_WAN_up;\
int id_WAN_down;\
int id_LAN_up;\
int numberOfBelowSensors: how many sensors below, even indirectly.\
int aggregation_rate;\
float delay_upper_router;\
float delay_lower_router;


## Usage
This module should be used at startup with the function in parser.c, where the txt file specified gets parsed and put into data structures.\
driver.c is used to show corrected and mode of use.\
getTopology(path) starts parser.c to do so.\
Then in topology.c all useful retrieval functions are written, for now need to pass the topology structures, if needed will be modified to accept others structs.


##TODO
