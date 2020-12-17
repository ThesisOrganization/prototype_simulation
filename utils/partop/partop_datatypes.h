#ifndef PARTOP_DATATYPES_H
#define PARTOP_DATATYPES_H

#include "../idmap/idmap.h"

#define MAX_LINE_LEN 128
//#############################################
//PARTOP DATA
//#############################################

typedef enum _state_type{
	NODE = 0,
	SENSOR,
	ACTUATOR,
	WAN,
	LAN
} state_type;

typedef enum _node_type{
	CENTRAL = 0,
	REGIONAL,
	LOCAL
} node_type;

typedef struct _node_topology{
	int node_type;
	int scheduler;
	int disk_type;
	int cores;
	double * diskServices;
	int * aggregation_rate;
	float delay_upper_router;
	float delay_lower_router;
	double * service_time;
	float probCommandResponse;
	int id_WAN_up;
	int id_WAN_down;
	int numberOfBelowActuators;
	int * actuatorsTypesBelow;
	int numberOfBelowSensors;
	int * sensorsTypesBelow;
	int * ListSensorsByType;
	int * ListActuatorsByType; //[x][y] element è il y-esimo elemento nella lista di attuatori di tipo x
	double disk_cost;
} node_topology;

typedef struct _sensor_topology{
	int type_job;
	int sensor_type;
	int measure_type;
	int id_LAN_up;
	double * sensorRates;
} sensor_topology;

typedef struct _actuator_topology{
	int type_job;
	int actuator_type;
	double rateTransition;
	double serviceTimeCommand;
	int measure_type;
	int id_LAN_up;
} actuator_topology;

typedef struct _wan_topology{
	int wan_type;
	float delay;
	int numberOfBelowActuators;
	int * actuatorsTypesBelow;
	int numberOfBelowSensors;
	int * sensorsTypesBelow;
	int * ListSensorsByType;
	int * ListActuatorsByType; //[x][y] element è il y-esimo elemento nella lista di attuatori di tipo x

} wan_topology;

typedef struct _lan_topology{
	int lan_type;
	float delay;
	double * LANsINserviceTimes;
	double * LANsOUTserviceTimes;
	int numberOfBelowActuators;
	int * actuatorsTypesBelow;
	int numberOfBelowSensors;
	int * sensorsTypesBelow;
	int * ListSensorsByType;
	int * ListActuatorsByType; //[x][y] element è il y-esimo elemento nella lista di attuatori di tipo x

} lan_topology;

//struct topology node specific
typedef union {
	sensor_topology * sensor;
	node_topology * node;
	actuator_topology * actuator;
	lan_topology * lan;
	wan_topology * wan;
} specific_topology;


typedef struct _general_topology{
	int total_nodes;
	int sensor_nodes;
	int actuator_nodes;
	int numberOfTotalLANs;
	int numberOfTotalWANs;
	int numberOfActTypes;
	int numberOfSensTypes;
	int numberOfLANsTypes;
	double * probOfActuators;
} general_topology;

typedef struct _Element_topology{
	int id;
	double cost;
  state_type lp_type;
  int upperNode;
  int numberOfLowerElements;
  int * lowerElements;
  int numberOfLANS;
  int * connectedLans;
  int numValidActuatorPaths;
  idmap * actuatorPaths;
  int numValidElToLP;
  idmap * ElementToLPMapping; // upper and lowers
  specific_topology spec_top;
} Element_topology;

typedef struct _lp_topology{
  int ** LPtoElementMapping;
  int * amountsOfElementsInLP;
  int numValid;
  idmap * ElementToLPMapping;
  int numLP;
} lp_topology;

typedef struct _total_topology{
  general_topology * gn;
  Element_topology ** lpt;
  lp_topology * lp_topology;
} total_topology;
#endif
