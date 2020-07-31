# Utils/partop
This folder will contain modules to parse input file and get informations in the relative data structure
## Syntax of text file
Being a work in progress project, this is subject to frequent changes and won't be a final version:
4
2
1;2;3,4;2;priority_sender, sensor2
...

First line: number of nodes
Second line: number of sensor nodes
Sender node, #receiver nodes,{receiver nodes},#informations,{informations}

## Data structure 1
TopArray is composed by:
nn : integer, number of sender nodes in the topology;
numberOfReceivers: integer, self explanatory;
numberInfos: idem;
receiver: array of integers containing the receivers
info: array of strings containing the informations

## DS 2
generalTopology is an additional data structure, used to give general informations, composed :
total_nodes: integer, will probably involve in only counting the non-sensor nodes;
sensor_nodes: integer, number of sensor nodes;
topArray * topArr: contains an array of TopArray structs, one for each sensor.


## Usage
This module should be used at startup with the function in parser.c
getTopology(); returns a struct with general informations and an array of topArray one for each sensor.
Then in topology.c all useful retrieval functions are written, for now need to pass the topArray array, if needed will be modified to accept generalTopology struct.
