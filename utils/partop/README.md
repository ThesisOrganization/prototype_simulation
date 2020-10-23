# Utils/partop
This folder will contain modules to parse input file and get informations in the relative data structure.\
The module relies on application_datatypes.h for the definition of the data structures.\
Important PSA: every array, matrix and so on that deals with the message types is ordered respecting the job_type ENUM in application_datatypes.h : TELEMETRY, TRANSITION, COMMAND, BATCH DATA.
## Syntax of text file
Being a work in progress project, this is subject to frequent changes and won't be a final version:\
24\
2\
2\
3\
0.33,0.10;0.24,0.12\
0.42,0.25,0.11;0.182,0.14,0.23;0.535,0.71,0.53;\
0.34;0.28\
0;-1;9;NODE,SCHEDULER1,CENTRAL,14/12/0/2,0.8,0.3,0.82/0.35/0.1/0.3/0.52,RAID3,0.4/0.73/0.00/0.23\
1;17;7;NODE,SCHEDULER2,REGIONAL,10/3/5/1,0.4,0.666,0.32/0.13/0.8/0.12/0.238;20;1;-1;5;SENSOR,LOSSY,SENSOR_TYPE0,MEASURE1\
9;20;1;-1;7;ACTUATOR,LOSSY,ACTUATOR_TYPE0,MEASURE1,0.14,0.32\
19;2;4;WAN,WAN_TYPE1,0.1\
22;4;4;LAN,LAN_TYPE1,0.312\
\
\
First line: # total elements\
2nd line: # types actuators\
3rd line: # types sensors\
4th line # types of LANs\
5th line: sensor type 0 telemetry rate,sensor type 0 transition rate;sensor type 1 tel. rate, ...\
6th line:lan IN type1 service time telemetry,trans, command\
7th line:lan OUT type1 service time telemetry,trans, command\
8th line: weight actuator type 0 for commands;weight act type 1;...\
\
id element, upper node,#informations,type of element,{informations}\
\
{informations} is different based on the type of node:
Node: scheduler, type of node, {aggregation rate telemetry/transition/command(0)/batch}, delay upper router, delay lower router,  {service time telemtry/service time transition/service time command/service time batch send}\
Sensor: type of job, Sensor type, measure done\
Actuator: type of job, Actuator type, measure done,rate transition, service time command\
WAN:type of WAN[CENTRAL-REGIONAL/REGIONAL-LOCAL], delay\
LAN:type of LAN[MANET/WIRELESS/ETC], delay\

For the CENTRAL NODE only, it has two more fields:\
type of connected Disk, {Disk service time telemetry, transition, command (0.00), batch}\
## Data Structure 1: total_topology
total_topology contains both the general topology and an array of LP_topologies, one for each element.\
general_topology * gn;\
LP_topology ** lpt;\

## DS 2: general_topology
general_topology is composed by the general, useful informations:\
int total_nodes;\
int sensor_nodes;\
int actuator_nodes;\
int numberOfTotalLANs;\
int numberOfTotalWANs;\
int numberOfActTypes;\
int numberOfSensTypes;\
int numberOfLANsTypes;\
double * probOfActuators: probability of each type of receiving a command.

## DS 3 : LP_topology
Second tier structure, containing the topology informations of a single node.\
int lp_type: Node, Sensor, Actuator, WAN, LAN\
int upperNode: which node is directly above.\
int numberOfLowerElements: how many elements are directly below.\
int * lowerElements: array of elements below(skipping WANs/LANs)\
int numberOfLANS: how many LANs directly below.\
int * connectedLans: array of LANs below.\
int *** ListActuatorsByType: matrix where an element [y][z] is the z-th actuator of type y in the sub-tree of this node.\
int *** ListSensorsByType;\
specific_topology spec_top: see below.\

double ** sensorRatesByType: array of arrays containing the sensor rates of each type of sensor for each type of message(only telemetry and transition).\
double ** LANsINserviceTimes: same as sensorRatesByType but in this case it is the service time of each type of lan IN for each type of message(all 5, even if batch send should not be here, but for future updates).\
double ** LANsOUTserviceTimes:specular.\
double * probOfActuators: probability of each type of actuator to receive a command message.\


## DS 3 Specific topology
Union of all different types of element topology.\\
\
NODE\
\
\
int node_type: CENTRAL, REGIONAL, LOCAL\
int scheduler: which scheduler it is used\
int disk_type: at the moment represents either RAID1, RAID2 or RAID3.\
double * diskServices : set containing the disk's service time for each type of message.\
int * aggregation_rate: set of aggregation rates for a node, for each type of message.\
float delay_upper_router;\
float delay_lower_router;\
double * service_time: array of service time for each type of message\
float probCommandResponse;\
int id_WAN_up;\
int id_WAN_down;\
int numberOfBelowActuators;\
int * actuatorsTypesBelow: how many actuators the node has below of each type(even indirectly)\
int numberOfBelowSensors;\
int * sensorsTypesBelow;\
\
\
SENSOR\
int type_job;\
int sensor_type;\
int measure_type;\
int id_LAN_up;\
double * sensorRates;\
\
\
ACTUATOR\
int type_job;\
int actuator_type;\
double rateTransition;\
double serviceTimeCommand;\
int measure_type;\
int id_LAN_up;\
\
\
WAN\
int wan_type;\
float delay;\
int numberOfBelowActuators;\
int * actuatorsTypesBelow;\
int numberOfBelowSensors;\
int * sensorsTypesBelow;\
\
\
LAN\
\
int lan_type;\
float delay;\
double * LANsINserviceTimes;\
double * LANsOUTserviceTimes;\
int numberOfBelowActuators;\
int * actuatorsTypesBelow;\
int numberOfBelowSensors;\
int * sensorsTypesBelow;\

## Usage
This module should be used at startup with the function in parser.c, where the txt file specified gets parsed and put into data structures.\
driver.c is used to show corrected and mode of use.\
getTopology(path) starts parser.c to do so.\
general_topology * getGenTopology(total_topology * totTop);\
LP_topology * getLPTopology(total_topology * totTop, int index)\
Then in topology.c all useful retrieval functions are written, most require a LP_topology as input.
## WILL BE DEPRECATED
getServiceRates(LP_topology * lpt) will have its name changed in getServiceTimesNodes(LP_topology * lpt)\
getActType(LP_topology * lpt) will have its name changed in getActuatorTypesBelowList(LP_topology * lpt)\
int * getSensType(LP_topology * lpt) will have its name changed in
getSensorsTypesBelowList(LP_topology * lpt)\
## DONE LAST PUSHS
15/10\
-added disk fields to CENTRAL:type of connected Disk, {Disk service time telemetry, transition, command (0.00), batch}.\\
-modified aggregation rates as a set instead of a single element(from transition only to all 4 types).\
16/10\
-Removed line 9 containing probabilities for node types, moved it to each node. Instead of topology->probNodeCommandArray[type], topology->topArray[id node]->info->probCommandResponse.\
17/10\
-Added GETs for every information, some refactoring.\
18/10\
-Everything got converted, check deprecated functions and how to use new syntax.
