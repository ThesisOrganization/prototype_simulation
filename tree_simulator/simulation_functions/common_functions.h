#include "../../utils/priority_scheduler/priority_scheduler.h"
#include "../../utils/partop/header.h"
#include "../../utils/priority_queue/priority_queue.h"
#include "../../utils/application_datatypes.h"

#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

/*
#define DEBUG  
#ifdef DEBUG 
#define ASSERT(x,s)\ 
if (! (x)) {\ 
    printf(s);\ 
    exit(1);\ 
} 
#else  
#define ASSERT(x,s) 
#endif 
*/

//#define DEBUG
#define PRINT_RESULTS


#ifdef DEBUG
#define PRINT(s) printf("%s\n", s)
#else
#define PRINT(s) 
#endif

#ifdef DEBUG
#define PRINT_VALUE(v) printf("%d\n", v)
#else
#define PRINT_VALUE(v) 
#endif

#ifdef DEBUG
#define PRINT_DOUBLE(v) printf("%f\n", v)
#else
#define PRINT_DOUBLE(v) 
#endif

#ifdef DEBUG
//list all debug prints and tests
//#define DEBUG_NUMBER_MSGS //used to print the type of message received by the central that are going to be saved on the disk
//#define DEBUG_RESPONSE_TIME
//#define DEBUG_INITIAL_VALUES_NODES


#endif


//NODES
#define GET_SERVICE_RATES(a) getServiceTimesNodes(a)
#define GET_NODE_TYPE(a) getNodeType(a)
#define GET_AGGREGATION_RATE(a) getAggregationRate(a)
#define GET_DELAY_UPPER_ROUTER(a) getDelayUpperRouter(a)
#define GET_DELAY_LOWER_ROUTER(a) getDelayLowerRouter(a)
#define GET_WAN_DOWN(a) getWanDown(a)
#define GET_PROB_COMMAND(a) getProbCommandResponse(a)

//SENSORS
#define GET_SENSOR_TYPE_RATES(a) getSensorRates(a)
#define GET_SENSOR_TYPE(a) getSensorType(a)

//ACTUATORS
#define GET_SERVICE_COMMAND(a) getServiceTimeCommand(a)
#define GET_RATE_TRANSITION(a) getRateTransition(a)

//LANS
#define GET_LAN_IN_TYPE_SERVICE(a) getLANsINserviceTimesByType(a)
#define GET_LAN_OUT_TYPE_SERVICE(a) getLANsOUTserviceTimesByType(a)
#define GET_LAN_TYPE(a) getLanType(a)

//WANS
#define GET_DELAY(a) getDelay(a)

#define GET_ACTUATOR_PATHS_INDEX(a) getActuatorPaths(a)
#define GET_ACT_TYPE(a) getActuatorTypesBelowList(a)
#define GET_LIST_ACTUATORS_BY_TYPE(a,b) getListActuatorsByType(a,b)
#define GET_UPPER_NODE(a) getUpperNode(a)

#define GET_DISK_SERVICES(a) getDiskServices(a)

#define GET_TYPE(a) getType(a)

//GENERAL TOPOLOGY
#define GET_TOTAL_NODES(a) getTotalNodes(a)
#define GET_SENSOR_NODES(a) getSensorNodes(a)
#define GET_ACTUATOR_NODES(a) getActuatorNodes(a)
#define GET_NUMBER_OF_LANS(a) getNumberOfTotalLANs(a)
#define GET_NUMBER_OF_WANS(a) getNumberOfTotalWANs(a)
#define GET_NUMBER_ACT_TYPES(a) getNumberOfActTypes(a)
#define GET_PROB_ACTUATORS(a) getProbOfActuators(a)

#endif /* COMMON_FUNCTIONS_H */
