/** \file setup_recv_functions.c
 *	\brief The implementations of the setup receive functions.
 */

#include "setup_recv_functions.h"

void recv_setup_info(setup_info* info,lp_state* state){
	//we allocate a new setup_data_info struct if it doesn't exists
	if(state->setup_data_info==NULL){
		state->setup_data_info=malloc(sizeof(setup_info));
	}
	//if setup_data_info exists we need to wipe it
	memset(state->setup_data_info,0,sizeof(setup_info));
	//then we can copy the received info in it.
	memcpy(state->setup_data_info,info,sizeof(setup_info));
}

/** \brief Handling of inner pointer of node_topology struct
 * \param[in] state The LP state
 * \param[in] data The data received
 *
 *  The function  updates these dangling pointers assuming that the
 * order of the events received follows the order in which the struct contents are declared.
 *
 * Any duplicate or mismatching event will be ignored.
 * Only one of the elements at a time can be matched.
 * This function assumes that info for the below devices is sent in the order they are declared into the below_devs struct.
 */
static void recv_data_in_node_topology(lp_state* state,void* data){
	//we verify that we can allocate the received data
	if(state->topology==NULL || state->topology->spec_top.node==NULL){
		printf("Error: mismatched message for node_topology received while element_topology or node_topology doesn't exists\n");
		exit(EXIT_FAILURE);
	}
	setup_info* info=state->setup_data_info;
	node_topology* node_top=state->topology->spec_top.node;
	//we have diskServices and we are a central node?
	if(node_top->diskServices==NULL && info->data_type==SETUP_DATA_PDOUBLE && GET_NODE_TYPE(state->topology)==CENTRAL){
		node_top->diskServices=malloc(info->data_size);
		memcpy(node_top->diskServices,data,info->data_size);
		return;
	}
	//we have aggregation_rate?
	if(node_top->aggregation_rate==NULL && info->data_type==SETUP_DATA_PINT){
		node_top->aggregation_rate=malloc(info->data_size);
		memcpy(node_top->aggregation_rate,data,info->data_size);
		return;
	}
	//we have service_time?
	if(node_top->service_time==NULL && info->data_type==SETUP_DATA_PDOUBLE){
		node_top->service_time=malloc(info->data_size);
		memcpy(node_top->service_time,data,info->data_size);
		return;
	}
	//we have actuatorsTypesBelow?
	if(node_top->actuatorsTypesBelow==NULL && info->data_type==SETUP_DATA_PINT){
		node_top->actuatorsTypesBelow=malloc(info->data_size);
		memcpy(node_top->actuatorsTypesBelow,data,info->data_size);
		return;
	}
	/*we don't send sensorsTypesBelow,kept for future use
	if(node_top->sensorsTypesBelow==NULL && info->data_type==SETUP_DATA_PINT){
		node_top->sensorsTypesBelow=malloc(info->data_size);
		memcpy(node_top->sensorsTypesBelow,data,info->data_size);
		return;
	}
	*/
	/*we don't send ListSensorsByType, kept for future use
	if(node_top->ListSensorsByType==NULL && info->data_type==SETUP_DATA_PINT){
		node_top->ListSensorsByType=malloc(info->data_size);
		memcpy(node_top->ListSensorsByType,data,info->data_size);
		return;
	}
	*/
	//we have ListActuatorsByType?
	if(node_top->ListActuatorsByType==NULL && info->data_type==SETUP_DATA_PINT){
		node_top->ListActuatorsByType=malloc(info->data_size);
		memcpy(node_top->ListActuatorsByType,data,info->data_size);
		return;
	}
	//we reach this point only if we already have all the necessarydata for this topology, thus we got a mismatched message
	printf("Error: mismatched message for node_topology received\n");
	exit(EXIT_FAILURE);
}


/** \brief Handling of inner pointer of lan_topology struct
 * \param[in] state The LP state
 * \param[in] data The data received
 *
 *  The function  updates these dangling pointers assuming that the
 * order of the events received follows the order in which the struct contents are declared.
 *
 * Any duplicate or mismatching event will be ignored.
 * Only one of the elements at a time can be matched.
 * This function assumes that info for the below devices is sent in the order they are declared into the below_devs struct.
 */
static void recv_data_in_lan_topology(lp_state* state,void* data){
	if(state->topology==NULL || state->topology->spec_top.lan==NULL){
		printf("Error: mismatched message for lan topology received while element_topology or lan_topology does not exist\n");
		exit(EXIT_FAILURE);
	}
	setup_info* info=state->setup_data_info;
	lan_topology* lan_top=state->topology->spec_top.lan;
	//we have LANsINserviceTimes?
	if(lan_top->LANsINserviceTimes==NULL && info->data_type==SETUP_DATA_PDOUBLE){
		lan_top->LANsINserviceTimes=malloc(info->data_size);
		memcpy(lan_top->LANsINserviceTimes,data,info->data_size);
		return;
	}
	//we have LANsOUTserviceTimes
	if(lan_top->LANsOUTserviceTimes==NULL && info->data_type==SETUP_DATA_PDOUBLE){
		lan_top->LANsOUTserviceTimes=malloc(info->data_size);
		memcpy(lan_top->LANsOUTserviceTimes,data,info->data_size);
		return;
	}
	//we have actuatorsTypesBelow?
	if(lan_top->actuatorsTypesBelow==NULL && info->data_type==SETUP_DATA_PINT){
		lan_top->actuatorsTypesBelow=malloc(info->data_size);
		memcpy(lan_top->actuatorsTypesBelow,data,info->data_size);
		return;
	}
	/* we don't send sensorsTypesBelow
	if(lan_top->sensorsTypesBelow==NULL && info->data_type==SETUP_DATA_PINT){
		lan_top->sensorsTypesBelow=malloc(info->data_size);
		memcpy(lan_top->sensorsTypesBelow,data,info->data_size);
		return;
	}*/
	/*we don't send ListSensorsByType
	if(lan_top->ListSensorsByType==NULL && info->data_type==SETUP_DATA_PINT){
		lan_top->ListSensorsByType=malloc(info->data_size);
		memcpy(lan_top->ListSensorsByType,data,info->data_size);
		return;
	}*/
	//we have ListActuatorsByType?
	if(lan_top->ListActuatorsByType==NULL && info->data_type==SETUP_DATA_PINT){
		lan_top->ListActuatorsByType=malloc(info->data_size);
		memcpy(lan_top->ListActuatorsByType,data,info->data_size);
		return;
	}
	//we reach this point only if we already have all the necessary data for this topology, thus we got a mismatched message
	printf("Error: mismatched message for lan_topology received\n");
	exit(EXIT_FAILURE);
}

/** \brief Handling of inner pointer of wan_topology struct
 * \param[in] state The LP state
 * \param[in] data The data received
 *
 *  The function  updates these dangling pointers assuming that the
 * order of the events received follows the order in which the struct contents are declared.
 *
 * Any duplicate or mismatching event will be ignored.
 * Only one of the elements at a time can be matched.
 * This function assumes that info for the below devices is sent in the order they are declared into the below_devs struct.
 */
static void recv_data_in_wan_topology(lp_state* state,void* data){
	if(state->topology==NULL || state->topology->spec_top.wan==NULL){
		printf("Error: mismatched message for wan_topology received while element_topology or wan_topology does not exist\n");
		exit(EXIT_FAILURE);
	}
	setup_info* info=state->setup_data_info;
	wan_topology* wan_top=state->topology->spec_top.wan;
	// we have actuatorsTypesBelow?
	if(wan_top->actuatorsTypesBelow==NULL && info->data_type==SETUP_DATA_PINT){
		wan_top->actuatorsTypesBelow=malloc(info->data_size);
		memcpy(wan_top->actuatorsTypesBelow,data,info->data_size);
		return;
	}
	/* we don't send sensorsTypesBelow
	if(wan_top->sensorsTypesBelow==NULL && info->data_type==SETUP_DATA_PINT){
		wan_top->sensorsTypesBelow=malloc(info->data_size);
		memcpy(wan_top->sensorsTypesBelow,data,info->data_size);
		return;
	}
	*/
	/*we don't send ListSensorsByType
	if(wan_top->ListSensorsByType==NULL && info->data_type==SETUP_DATA_PINT){
		wan_top->ListSensorsByType=malloc(info->data_size);
		memcpy(wan_top->ListSensorsByType,data,info->data_size);
		return;
	}
	*/
	//we have ListActuatorsByType?
	if(wan_top->ListActuatorsByType==NULL && info->data_type==SETUP_DATA_PINT){
		wan_top->ListActuatorsByType=malloc(info->data_size);
		memcpy(wan_top->ListActuatorsByType,data,info->data_size);
		return;
	}
	//we reach this point only if we already have all the necessary data for this topology, thus we got a mismatched message
	printf("Error: mismatched message for wan_topology received\n");
	exit(EXIT_FAILURE);
}

/** \brief Handling of inner pointers of sensor_topology struct
 * \param[in] state The LP state
 * \param[in] data The data received
 * The function  updates these dangling pointers assuming that the
 * order of the events received follows the order in which the struct contents are declared.
 *
 * Any duplicate or mismatching event will be ignored.
 */
static void recv_data_in_sensor_topology(lp_state* state,void* data){
	if(state->topology==NULL || state->topology->spec_top.sensor==NULL){
		printf("Error: mismatched message for sensor_topology received while element_topology or sensor_topology does not exist\n");
		exit(EXIT_FAILURE);
	}
	setup_info *info=state->setup_data_info;
	if(state->topology->spec_top.sensor->sensorRates==NULL && info->data_type==SETUP_DATA_PDOUBLE){
		state->topology->spec_top.sensor->sensorRates=malloc(info->data_size);
		memcpy(state->topology->spec_top.sensor->sensorRates,data,info->data_size);
		return;
	}
	//we reach this point only if we already have all the necessary data for this topology, thus we got a mismatched message
	printf("Error: mismatched message for sensor_topology received\n");
	exit(EXIT_FAILURE);
}

/** \brief Handling of element_topology inner pointers
 * \param[in] state The LP state.
 * \param[in] data The received data
 *
 * The function sets any inner pointers for these structs to NULL, and updates these dangling pointers assuming that the
 * order of the events received follows the order in which the struct contents are declared.
 *
 * Any duplicate or mismatching event will be ignored.
 *
 * Only one of the elements at a time can be matched.
 */
static void recv_data_in_element_topology(lp_state *state,void* data){
	if(state->topology==NULL){
		printf("Error: mismatched message for element_topology received while element_topology does not exist\n");
		exit(EXIT_FAILURE);
	}
	setup_info* info=state->setup_data_info;
	specific_topology *spec_top=&(state->topology->spec_top);
	/*we don't need lowerElements, this is kept for future uses
	if(state->topology->lowerElements==NULL && info->data_type==SETUP_DATA_PINT){
		state->topology->lowerElements=malloc(info->data_size);
		memcpy(state->topology->lowerElements,data,info->data_size);
		return;
	}*/
	/*we already have connectedLans, kept for future uses
	if(state->topology->connectedLans==NULL && info->data_type==SETUP_DATA_PINT){
		state->topology->connectedLans=malloc(info->data_size);
		memcpy(state->topology->connectedLans,data,info->data_size);
		return;
	}*/
	//we already have actuatorPaths if we are not a sensor or an actuator?
	if(GET_TYPE(state->topology)!=SENSOR && GET_TYPE(state->topology)!=ACTUATOR){
		if(state->topology->actuatorPaths==NULL && info->data_type==SETUP_DATA_PINT){
			state->topology->actuatorPaths=malloc(info->data_size);
			memcpy(state->topology->actuatorPaths,data,info->data_size);
			return;
		}
	}
	//now we handle the specific topology, the topology which we'll receive will vary with the element type
	switch(state->topology->lp_type){
		//we have received a node topology?
		case NODE:
			if(spec_top->node==NULL && info->data_type==SETUP_DATA_NODE_TOPOLOGY){
				spec_top->node=malloc(info->data_size);
				memcpy(spec_top->node,data,info->data_size);
				//we set the inner pointer of node_topology to NULL
				spec_top->node->diskServices=NULL;
				spec_top->node->aggregation_rate=NULL;
				spec_top->node->service_time=NULL;
				spec_top->node->actuatorsTypesBelow=NULL;
				spec_top->node->sensorsTypesBelow=NULL;
				spec_top->node->ListSensorsByType=NULL;
				spec_top->node->ListActuatorsByType=NULL;
				return;
			}
			break;
			// we have received a sensor_topology?
		case SENSOR:
			if(spec_top->sensor==NULL && info->data_type==SETUP_DATA_SENSOR_TOPOLOGY){
				spec_top->sensor=malloc(info->data_size);
				memcpy(spec_top->sensor,data,info->data_size);
				//we set the inner pointer of sensor_topology to NULL
				spec_top->sensor->sensorRates=NULL;
				return;
			}
			break;
			//we have received an actuator_topology?
		case ACTUATOR:
			if(spec_top->actuator==NULL && info->data_type==SETUP_DATA_ACTUATOR_TOPOLOGY){
				spec_top->actuator=malloc(info->data_size);
				memcpy(spec_top->actuator,data,info->data_size);
				return;
			}
			break;
			//we have received a wan_topology?
		case WAN:
			if(spec_top->wan==NULL && info->data_type==SETUP_DATA_WAN_TOPOLOGY){
				spec_top->wan=malloc(info->data_size);
				memcpy(spec_top->wan,data,info->data_size);
				//we set the inner pointer of wan_topology to NULL
				spec_top->wan->actuatorsTypesBelow=NULL;
				spec_top->wan->sensorsTypesBelow=NULL;
				spec_top->wan->ListSensorsByType=NULL;
				spec_top->wan->ListActuatorsByType=NULL;
				return;
			}
			break;
			//we have received a lan_topology?
		case LAN:
			if(spec_top->lan==NULL && info->data_type==SETUP_DATA_LAN_TOPOLOGY){
				spec_top->lan=malloc(info->data_size);
				memcpy(spec_top->lan,data,info->data_size);
				//we set the inner pointer of node_topology to NULL
				spec_top->lan->LANsINserviceTimes=NULL;
				spec_top->lan->LANsOUTserviceTimes=NULL;
				spec_top->lan->actuatorsTypesBelow=NULL;
				spec_top->lan->sensorsTypesBelow=NULL;
				spec_top->lan->ListSensorsByType=NULL;
				spec_top->lan->ListActuatorsByType=NULL;
				return;
			}
			break;
	}
	//we reach this point only if we already have all the necessary data for this topology, thus we got a mismatched message
	printf("Error: mismatched message for element_topology received\n");
	exit(EXIT_FAILURE);
}


/** \brief Handling of general_topology inner pointers
 * \param[in] state The LP state.
 * \param[in] data The received setup data
 *
 * The function sets any inner pointers for these structs to NULL, and updtes these dangling pointers assuming that the
 * order of the events received follows the order in which the struct contents are declared.
 *
 * Any duplicate or mismatching event will be ignored.
 */
static void recv_data_in_general_topology(lp_state* state, void* data){
	if(state->general_topology==NULL){
		printf("Error: mismatched message for general_topology received while it does not exist\n");
		exit(EXIT_FAILURE);
	}
	setup_info* info=state->setup_data_info;
	//we check if we don't have probOfActuators and if we received the correct type
	if(state->general_topology->probOfActuators==NULL && info->data_type==SETUP_DATA_PDOUBLE){
		state->general_topology->probOfActuators=malloc(info->data_size);
		memcpy(state->general_topology->probOfActuators,data,info->data_size);
		return;
	}
	//we reach this point only if we already have all the necessary data for this topology, thus we got a mismatched message
	printf("Error: mismatched message for general_topology received\n");
	exit(EXIT_FAILURE);
}

/** \brief Handling of the structs that are placed directly inside the LP state.
 * \param[in] state The LP state.
 * \param[in] data The data to be placed inside the LP state
 *
 * The function sets any inner pointers for these structs to NULL, and updtes these dangling pointers assuming that the
 * order of the events received follows the order in which the struct contents are declared.
 *
 * Any duplicate or mismatching event will be ignored.
 */
static void recv_data_in_lp_state(lp_state* state,void* data){
	setup_info* info=state->setup_data_info;
	//we need to now which data are we receiving
	switch(info->data_type){
		case SETUP_DATA_GENERAL_TOPOLOGY:
			//we check if we don't have a general_topology
			if(state->general_topology==NULL){
				state->general_topology=malloc(info->data_size);
				memcpy(state->general_topology,data,info->data_size);
				//we set to NULL the probOfActuators since it will be received in a future message
				state->general_topology->probOfActuators=NULL;
				return;
			}
			break;
		case SETUP_DATA_ELEMENT_TOPOLOGY:
			//we already have an element_topology?
			if(state->topology==NULL){
				state->topology=malloc(info->data_size);
				memcpy(state->topology,data,info->data_size);
				//we set to NULL every nested pointer, since we need to receive them
				state->topology->lowerElements=NULL;
				state->topology->connectedLans=NULL;
				state->topology->actuatorPaths=NULL;
				//now we need to initialize the specific_topology to NULL according to the element type
				switch(state->topology->lp_type){
					case NODE:
						state->topology->spec_top.node=NULL;
						break;
					case SENSOR:
						state->topology->spec_top.sensor=NULL;
						break;
					case ACTUATOR:
						state->topology->spec_top.actuator=NULL;
						break;
					case WAN:
						state->topology->spec_top.wan=NULL;
						break;
					case LAN:
						state->topology->spec_top.lan=NULL;
						break;
				}
				return;
			}
			break;
			//we ignore any mismatching/duplicate events
		default:
			//we reach this point only if we already have all the necessary data for this topology, thus we got a mismatched message
			printf("Error: mismatched setup message for lp_state received\n");
			exit(EXIT_FAILURE);
	}
}

void recv_setup_data(lp_state* state,void* data){
	if(state->setup_data_info==NULL){
		return;
	}
	// we need to identify the container struct as first operation
	switch(state->setup_data_info->container_struct){
		case SETUP_DATA_LP_STATE:
			recv_data_in_lp_state(state,data);
			break;
		case SETUP_DATA_GENERAL_TOPOLOGY:
			recv_data_in_general_topology(state,data);
			break;
		case SETUP_DATA_ELEMENT_TOPOLOGY:
			recv_data_in_element_topology(state,data);
			break;
		case SETUP_DATA_LAN_TOPOLOGY:
			recv_data_in_lan_topology(state,data);
			break;
		case SETUP_DATA_WAN_TOPOLOGY:
			recv_data_in_wan_topology(state,data);
			break;
		case SETUP_DATA_SENSOR_TOPOLOGY:
			recv_data_in_sensor_topology(state,data);
			break;
		case SETUP_DATA_NODE_TOPOLOGY:
			recv_data_in_node_topology(state,data);
			break;
		default:
			//we reach this point only if we already have all the necessary data for this topology, thus we got a mismatched message
			printf("Error: mismatched setup message received\n");
			exit(EXIT_FAILURE);
	}
	free(state->setup_data_info);
	state->setup_data_info=NULL;
}
