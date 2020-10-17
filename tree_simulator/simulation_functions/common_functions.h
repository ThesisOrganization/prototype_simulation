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
#define PRINT(s) printf(s)
#else
#define PRINT(s) 
#endif


//NODES
#define GET_SERVICE_RATES(a,b) getServiceRates(a,b)
#define GET_NODE_TYPE(a,b) getNodeType(a,b)
#define GET_AGGREGATION_RATE(a,b) getAggregationRate(a,b)
#define GET_DELAY_UPPER_ROUTER(a,b) getDelayUpperRouter(a,b)
#define GET_DELAY_LOWER_ROUTER(a,b) getDelayLowerRouter(a,b)
#define GET_WAN_DOWN(a,b) getWanDown(a,b)
#define GET_PROB_COMMAND(a,b) getProbCommandResponse(a,b)

//SENSORS
#define GET_SENSOR_TYPE_RATES(a,b) getSensorRatesForOneSensorType(a,b)
#define GET_SENSOR_TYPE(a,b) getSensorType(a,b)

//ACTUATORS
#define GET_SERVICE_COMMAND(a,b) getServiceTimeCommand(a,b)
#define GET_RATE_TRANSITION(a,b) getRateTransition(a,b)

//LANS
#define GET_LAN_IN_TYPE_SERVICE(a,b) getLANsINserviceTimesForOneLANType(a,b)
#define GET_LAN_OUT_TYPE_SERVICE(a,b) getLANsOUTserviceTimesForOneLANType(a,b)
#define GET_LAN_TYPE(a,b) getLanType(a,b)

//WANS
#define GET_DELAY(a,b) getDelay(a,b)




#endif /* COMMON_FUNCTIONS_H */
