# Utils/partop
This folder will contain modules to parse input file and get informations in the relative data structure
## Syntax of text file
Being a work in progress project, this is subject to frequent changes and won't be a final version:
1;2;3,4;2;priority_sender, sensor2
Sender node, #receiver nodes,{receiver nodes},#informations,{informations}

## Data structure
TopArray is composed by:
nn : integer, number of sender nodes in the topology;
numberOfReceivers: integer, self explanatory;
numberInfos: idem;
receiver: array of integers containing the receivers
info: array of strings containing the informations

## Usage
This module should be used at startup with the function in parser.c
getTopology(); which returns an array of TopArray, ordered, having informations for all sender nodes.
Then in topology.c all useful retrieval functions are written.
