/** \file setup_send_functions.c
 * \brief Implementation of the setup functions that send structures to other LPs.
 */
#include "setup_send_functions.h"
#include "../common_functions.h"
#include "setup_datastructures.h"

/** \brief Sends a sensor_topology to the specified element_id
 * \param[in] elem_top The Element_topology containing the sensor_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a sensor contains a sensor_topology in the spec_top parameter.
 */
static void send_sensor_topology(Element_topology* elem_top,int element_id){
	void* message;
	size_t message_size;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	//we send the sensor_topology struct
	message=create_setup_message(element_id,SETUP_DATA_ELEMENT_TOPOLOGY,SETUP_DATA_SENSOR_TOPOLOGY,elem_top->spec_top.sensor,sizeof(sensor_topology),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//we send the sensorRates array, which use only two data classes.
	ts_skew+=0.01;
	message=create_setup_message(element_id,SETUP_DATA_SENSOR_TOPOLOGY,SETUP_DATA_PDOUBLE,GET_SENSOR_TYPE_RATES(elem_top),sizeof(double)*2,&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
}

/** \brief Sends an actuator_topology to the specified element_id
 * \param[in] elem_top The Element_topology containing the actuator_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a actuator contains an actuator_topology in the spec_top parameter.
 */
static void send_actuator_topology(Element_topology* elem_top, int element_id){
	void* message;
	size_t message_size;
	//we send the actuator_topology struct
	message=create_setup_message(element_id,SETUP_DATA_ELEMENT_TOPOLOGY,SETUP_DATA_ACTUATOR_TOPOLOGY,elem_top->spec_top.actuator,sizeof(actuator_topology),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
}

/** \brief Send info about the below actuators and sensors to the LP corresponding to element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology of the current element.
 * \param[in] element_id The id of __both__ the element and the LP.
 * \param[in] parem_struct The struct which will contain the info about sensors and actuators.
 */
static void send_below_devices_info(general_topology* gen_top,Element_topology* elem_top,below_devs below_devices,int element_id, setup_data_types parent_struct){
	void* message;
	size_t message_size;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp until we have sent all the nested pointers in below_devices
	//we send actuatorsTypesBelow
	if(below_devices.actuatorsTypesBelow!=NULL){
		message=create_setup_message(element_id,parent_struct,SETUP_DATA_PINT,below_devices.actuatorsTypesBelow,GET_NUMBER_ACT_TYPES(gen_top)*sizeof(int),&message_size);
		ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
		destroy_message(message);
	}
	//we send sensorTypesBelow
	if(below_devices.sensorsTypesBelow!=NULL){
		ts_skew+=0.01;
		message=create_setup_message(element_id,parent_struct,SETUP_DATA_PINT,below_devices.sensorsTypesBelow,GET_NUMBER_SENS_TYPES(gen_top)*sizeof(int),&message_size);
		ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
		destroy_message(message);
	}
	//we send ListSensorsByType
	if(below_devices.ListSensorsByType!=NULL){
		ts_skew+=0.01;
		message=create_setup_message(element_id,parent_struct,SETUP_DATA_PINT,below_devices.ListSensorsByType,GET_NUMBER_SENS_BELOW(elem_top)*sizeof(int),&message_size);
		ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
		destroy_message(message);
	}
	//we send ListActuatorsByType
	if(below_devices.ListActuatorsByType!=NULL){
		ts_skew+=0.01;
		message=create_setup_message(element_id,parent_struct,SETUP_DATA_PINT,below_devices.ListActuatorsByType,GET_NUMBER_ACT_BELOW(elem_top)*sizeof(int),&message_size);
		ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
		destroy_message(message);
	}
}

/** \brief Sends a lan_topology to the specified element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology containing the lan_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a lan contains an lan_topology in the spec_top parameter.
 */
static void send_lan_topology(general_topology* gen_top,Element_topology* elem_top,int element_id){
	void* message;
	size_t message_size;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the lan_topology.
	//we send the lan_topology struct
	message=create_setup_message(element_id,SETUP_DATA_ELEMENT_TOPOLOGY,SETUP_DATA_LAN_TOPOLOGY,elem_top->spec_top.lan,sizeof(lan_topology),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//we send LanINserviceTimes
	//we have a service time for each message class plus one for the replies.
	ts_skew+=0.01;
	message=create_setup_message(element_id,SETUP_DATA_LAN_TOPOLOGY,SETUP_DATA_PDOUBLE,GET_LAN_IN_TYPE_SERVICE(elem_top),sizeof(double)*5,&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//we send LansOUTserviceTimes
	//we have a service time for each message class plus one for the replies.
	ts_skew+=0.01;
	message=create_setup_message(element_id,SETUP_DATA_LAN_TOPOLOGY,SETUP_DATA_PDOUBLE,GET_LAN_OUT_TYPE_SERVICE(elem_top),sizeof(double)*5,&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	below_devs devs;
	devs.actuatorsTypesBelow=GET_ACT_TYPE_BELOW_LIST(elem_top);
	/* we don't send info on the below sensors
	devs.sensorsTypesBelow=GET_SENS_TYPE_BELOW_LIST(elem_top);
	devs.ListSensorsByType=GET_LIST_SENSORS_BY_TYPE(elem_top,0); // we need to get the begining of the row matrix
	*/
	devs.sensorsTypesBelow=NULL;
	devs.ListSensorsByType=NULL;
	devs.ListActuatorsByType=GET_LIST_ACTUATORS_BY_TYPE(elem_top,0);
	send_below_devices_info(gen_top,elem_top,devs,element_id,SETUP_DATA_LAN_TOPOLOGY);
}

/** \brief Sends a wan_topology to the specified element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology containing the wan_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a wan contains an wan_topology in the spec_top parameter.
 */
static void send_wan_topology(general_topology* gen_top, Element_topology* elem_top,int element_id){
	void* message;
	size_t message_size;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp until we have sent all the nested pointers of the wan_topology.
	message=create_setup_message(element_id,SETUP_DATA_ELEMENT_TOPOLOGY,SETUP_DATA_WAN_TOPOLOGY,elem_top->spec_top.wan,sizeof(wan_topology),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	below_devs devs;
	devs.actuatorsTypesBelow=GET_ACT_TYPE_BELOW_LIST(elem_top);
	/* we don't need to send info on the below sensors
	 devs.sensorsTypesBelow=GET_S*ENS_TYPE_BELOW_LIST(elem_top);
	 devs.ListSensorsByType=GET_LIST_SENSORS_BY_TYPE(elem_top,0); // we need to get the beginning of the row matrix
	 */
	devs.sensorsTypesBelow=NULL;
	devs.ListSensorsByType=NULL;
	devs.ListActuatorsByType=GET_LIST_ACTUATORS_BY_TYPE(elem_top,0);
	send_below_devices_info(gen_top,elem_top,devs,element_id,SETUP_DATA_WAN_TOPOLOGY);
}

/** \brief Sends a node_topology to the specified element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology containing the node_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a node contains a node_topology in the spec_top parameter.
 */
static void send_node_topology(general_topology* gen_top, Element_topology* elem_top,int element_id){
	void* message;
	size_t message_size;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the node_topology.
	//we send the node_topology struct

	message=create_setup_message(element_id,SETUP_DATA_ELEMENT_TOPOLOGY,SETUP_DATA_NODE_TOPOLOGY,elem_top->spec_top.node,sizeof(node_topology),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);

	//we send diskServices is the type of the node is central, we have a disk service time for each class of data
	if(GET_NODE_TYPE(elem_top)==CENTRAL){
		message=create_setup_message(element_id,SETUP_DATA_NODE_TOPOLOGY,SETUP_DATA_PDOUBLE,GET_DISK_SERVICES(elem_top),sizeof(double)*4,&message_size);
		ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
		destroy_message(message);
	}
	//we send aggregation_rate
	ts_skew+=0.01;
	message=create_setup_message(element_id,SETUP_DATA_NODE_TOPOLOGY,SETUP_DATA_PINT,GET_AGGREGATION_RATE(elem_top),sizeof(int)*4,&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);

	//we send service_time
	ts_skew+=0.01;
	message=create_setup_message(element_id,SETUP_DATA_NODE_TOPOLOGY,SETUP_DATA_PDOUBLE,GET_SERVICE_TIMES_NODES(elem_top),sizeof(double)*5,&message_size);
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	below_devs devs;
	devs.actuatorsTypesBelow=GET_ACT_TYPE_BELOW_LIST(elem_top);
	/* we don't need to send info on the below sensors
	devs.sensorsTypesBelow=GET_SENS_TYPE_BELOW_LIST(elem_top);
	devs.ListSensorsByType=GET_LIST_SENSORS_BY_TYPE(elem_top,0); // we need to get the beginning of the row matrix
	*/
	devs.sensorsTypesBelow=NULL;
	devs.ListSensorsByType=NULL;
	devs.ListActuatorsByType=GET_LIST_ACTUATORS_BY_TYPE(elem_top,0);
	send_below_devices_info(gen_top,elem_top,devs,element_id,SETUP_DATA_NODE_TOPOLOGY);
}

static void send_element_topology(total_topology* tot_top,int element_id, int total_elements){
	Element_topology *elem_top;
	general_topology *gen_top=GET_GEN_TOPOLOGY(tot_top);
	void* message;
	size_t message_size;
	double ts_skew; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	//we retrieve the topology of the element
	elem_top=GET_ELEMENT_TOPOLOGY(tot_top,element_id);
	//before sending the stuct we prepare a timestamp skew, to allow us to send all the struct nested elements in an ordered fashion before reaching ::TS_START_SIMULATION or TS_RECV_SPECIFIC_TOPOLOGY.
	ts_skew=0.0;
	//we send the info structure to the destination LP.
	message=create_setup_message(element_id,SETUP_DATA_DEVICE_STATE,SETUP_DATA_ELEMENT_TOPOLOGY,elem_top,sizeof(Element_topology),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//now we need to send the arrays.
	///When sending nested pointers in the topology we follow the order of the declaration
	//we send only actuatorPaths if we are a sensor or an actuator the other arrays are not needed by the LP.
	if(GET_TYPE(elem_top)!=SENSOR && GET_TYPE(elem_top)!=ACTUATOR){
		message=create_setup_message(element_id,SETUP_DATA_ELEMENT_TOPOLOGY,SETUP_DATA_PIDMAP,GET_ACTUATOR_PATHS(elem_top),sizeof(idmap)*total_elements,&message_size);
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	}
	//to send the specific_topology we need to check the element type and access the union accordingly
	switch(GET_TYPE(elem_top)){
		case SENSOR:
			send_sensor_topology(elem_top,element_id);
			break;
		case ACTUATOR:
			send_actuator_topology(elem_top,element_id);
			break;
		case LAN:
			send_lan_topology(gen_top,elem_top,element_id);
			break;
		case WAN:
			send_wan_topology(gen_top,elem_top,element_id);
			break;
		case NODE:
			send_node_topology(gen_top,elem_top,element_id);
			break;
		default:
			printf("Error: unknown element type\n");
			exit(EXIT_FAILURE);
	}
}

void send_general_topology(general_topology* gen_top,int element_id){
	double ts_skew=0.0; // we need a skew to send the topology info and data before reaching another point in time like TS_START_SIMULATION.
	size_t message_size;
	void* message;
	ts_skew+=0.01;
	message=create_setup_message(element_id,SETUP_DATA_LP_STATE,SETUP_DATA_GENERAL_TOPOLOGY,gen_top,sizeof(general_topology),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	//we send probofActuators
	ts_skew+=0.01;
	message=create_setup_message(element_id,SETUP_DATA_GENERAL_TOPOLOGY,SETUP_DATA_PDOUBLE,GET_PROB_ACTUATORS(gen_top),sizeof(double)*GET_NUMBER_ACT_TYPES(gen_top),&message_size);
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
}

/** \brief Creates and sends an element <-> index idmap.
 * \param[in] lp The id of the receiver lp.
 * \param[in] devices The devices array associated to the given lp.
 * \param[in] num_devices The number of devices associated the receiver lp.
 */
static void send_idmap(int lp,int* devices,int num_devices){
	int *ids=NULL, i,map_len;
	size_t message_size;
	idmap *map;
	void* message;
	if(devices==NULL || num_devices<=0){
		ids=malloc(sizeof(int)*num_devices);
		for(i=0;i<num_devices;i++){
			ids[i]=i;
		}
		map=create_idmap(ids,devices,num_devices,&map_len);
		message=create_setup_message(devices[0],SETUP_DATA_LP_STATE,SETUP_DATA_PIDMAP,map,sizeof(idmap)*map_len,&message_size);
		ScheduleNewEvent(lp,TS_RECV_ELEMENT_INDEX_MAP,RECEIVE_SETUP_MESSAGE,message,message_size);
		destroy_message(message);
		destroy_idmap(map);
	}
}

void send_lp_info(int lp,total_topology* tot_top){
	void* message=NULL;
	int i,num_devices=0, *devices=NULL;
	size_t message_size;
	//we retireve the devices array for to send the idmap and the element topology to the lp.
	num_devices=GET_NUM_LP_DEVICES(tot_top->lp_topology,lp);
	devices=GET_LP_DEVICES(tot_top->lp_topology,lp);
	//we send the idmap
	send_idmap(lp,devices,num_devices);
	//we need to send a setup message to make the LP allocate the devices_array
	///When sending the message for the allocation of the device array the data associated does not exists and data_size is used to pass the number of devices.
	message=create_setup_message(devices[0],SETUP_DATA_LP_STATE,SETUP_DATA_DEVICES_ARRAY,NULL,num_devices,&message_size);
	ScheduleNewEvent(lp,TS_RECV_DEVICES,RECEIVE_SETUP_MESSAGE,message,message_size);
	destroy_message(message);
	for(i=0;i<num_devices;i++){
		//we sent the element topology for each device.
		send_element_topology(tot_top,i,GET_NUM_ELEMENTS(tot_top->lp_topology));
	}
}
