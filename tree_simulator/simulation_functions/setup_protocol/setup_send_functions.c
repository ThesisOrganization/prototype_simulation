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
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	info.element_id=element_id;
	info.data_size=sizeof(sensor_topology);
	info.data_type=SETUP_DATA_SENSOR_TOPOLOGY;
	info.container_struct=SETUP_DATA_ELEMENT_TOPOLOGY;
	//we send the sensor_topology struct
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.sensor,info.data_size);
	//we send the sensorRates array
	info.container_struct=SETUP_DATA_SENSOR_TOPOLOGY;
	info.data_type=SETUP_DATA_PDOUBLE;
	info.data_size=sizeof(double)*2; // sensors use only two classes of data
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_SENSOR_TYPE_RATES(elem_top),info.data_size);
}

/** \brief Sends an actuator_topology to the specified element_id
 * \param[in] elem_top The Element_topology containing the actuator_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a actuator contains an actuator_topology in the spec_top parameter.
 */
static void send_actuator_topology(Element_topology* elem_top, int element_id){
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp until we have sent all the nested pointers of the actuator_topology.
	info.element_id=element_id;
	info.data_size=sizeof(actuator_topology);
	info.data_type=SETUP_DATA_ACTUATOR_TOPOLOGY;
	info.container_struct=SETUP_DATA_ELEMENT_TOPOLOGY;
	//we send the actuator_topology struct
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.actuator,info.data_size);
}

/** \brief Send info about the below actuators and sensors to the LP corresponding to element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology of the current element.
 * \param[in] element_id The id of __both__ the element and the LP.
 * \param[in] parem_struct The struct which will contain the info about sensors and actuators.
 */
static void send_below_devices_info(general_topology* gen_top,Element_topology* elem_top,below_devs below_devices,int element_id, setup_data_types parent_struct){
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp until we have sent all the nested pointers in below_devices
	info.element_id=element_id;
	info.container_struct=parent_struct;
	//we send actuatorsTypesBelow
	info.data_type=SETUP_DATA_PINT;
	info.data_size=GET_NUMBER_ACT_TYPES(gen_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,below_devices.actuatorsTypesBelow,info.data_size);
	//we send sensorTypesBelow
	info.data_size=GET_NUMBER_SENS_TYPES(gen_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,below_devices.sensorsTypesBelow,info.data_size);
	//we send ListSensorsByType
	info.data_size=GET_NUMBER_SENS_BELOW(elem_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,below_devices.ListSensorsByType,info.data_size);
	//we send ListActuatorsByType
	info.data_size=GET_NUMBER_ACT_BELOW(elem_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,below_devices.ListActuatorsByType,info.data_size);
}

/** \brief Sends a lan_topology to the specified element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology containing the lan_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a lan contains an lan_topology in the spec_top parameter.
 */
static void send_lan_topology(general_topology* gen_top,Element_topology* elem_top,int element_id){
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the lan_topology.
	info.element_id=element_id;
	info.data_size=sizeof(lan_topology);
	info.data_type=SETUP_DATA_LAN_TOPOLOGY;
	info.container_struct=SETUP_DATA_ELEMENT_TOPOLOGY;
	//we send the lan_topology struct
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan,info.data_size);
	//we send LanINserviceTimes
	info.container_struct=SETUP_DATA_LAN_TOPOLOGY;
	info.data_type=SETUP_DATA_PDOUBLE;
	info.data_size=sizeof(double)*5; //we have a service time for each message class plus one for the replies.
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_LAN_IN_TYPE_SERVICE(elem_top),info.data_size);
	//we send LansOUTserviceTimes
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_LAN_OUT_TYPE_SERVICE(elem_top),info.data_size);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	below_devs devs;
	devs.actuatorsTypesBelow=GET_ACT_TYPE_BELOW_LIST(elem_top);
	devs.sensorsTypesBelow=GET_SENS_TYPE_BELOW_LIST(elem_top);
	devs.ListSensorsByType=GET_LIST_SENSORS_BY_TYPE(elem_top,0); // we need to get the begining of the row matrix
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
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	info.element_id=element_id;
	//we send the wan_topology struct
	info.data_size=sizeof(wan_topology);
	info.data_type=SETUP_DATA_WAN_TOPOLOGY;
	info.container_struct=SETUP_DATA_ELEMENT_TOPOLOGY;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.wan,info.data_size);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	below_devs devs;
	devs.actuatorsTypesBelow=GET_ACT_TYPE_BELOW_LIST(elem_top);
	devs.sensorsTypesBelow=GET_SENS_TYPE_BELOW_LIST(elem_top);
	devs.ListSensorsByType=GET_LIST_SENSORS_BY_TYPE(elem_top,0); // we need to get the begining of the row matrix
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
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	info.element_id=element_id;
	//we send the wan_topology struct
	info.data_size=sizeof(node_topology);
	info.data_type=SETUP_DATA_NODE_TOPOLOGY;
	info.container_struct=SETUP_DATA_ELEMENT_TOPOLOGY;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.node,info.data_size);
	//we send diskServices
	info.container_struct=SETUP_DATA_NODE_TOPOLOGY;
	info.data_type=SETUP_DATA_PDOUBLE;
	info.data_size=sizeof(double)*4; // we have a disk service time  for each class of data
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_DISK_SERVICES(elem_top),info.data_size);
	//we send aggregation_rate
	info.data_type=SETUP_DATA_PINT;
	info.data_size=sizeof(double)*4; // we have an aggregation rate for each class of data
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_AGGREGATION_RATE(elem_top),info.data_size);
	//we send service_time
	info.data_type=SETUP_DATA_PDOUBLE;
	info.data_size=sizeof(double)*5; // we have a service for each class of data plus the reply events
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_SERVICE_TIMES_NODES(elem_top),info.data_size);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	below_devs devs;
	devs.actuatorsTypesBelow=GET_ACT_TYPE_BELOW_LIST(elem_top);
	devs.sensorsTypesBelow=GET_SENS_TYPE_BELOW_LIST(elem_top);
	devs.ListSensorsByType=GET_LIST_SENSORS_BY_TYPE(elem_top,0); // we need to get the begining of the row matrix
	devs.ListActuatorsByType=GET_LIST_ACTUATORS_BY_TYPE(elem_top,0);
	send_below_devices_info(gen_top,elem_top,devs,element_id,SETUP_DATA_NODE_TOPOLOGY);
}

void send_element_topology(total_topology* tot_top,int element_id, int total_elements){
	Element_topology *elem_top;
	general_topology *gen_top=GET_GEN_TOPOLOGY(tot_top);
	setup_info info;
	double ts_skew; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	//we retrieve the topology of the element
	elem_top=GET_ELEMENT_TOPOLOGY(tot_top,element_id);
	//we prepare a data_info struct
	info.element_id=element_id;
	info.data_size=sizeof(Element_topology);
	info.data_type=SETUP_DATA_ELEMENT_TOPOLOGY;
	info.container_struct=SETUP_DATA_LP_STATE;
	//before sending the stuct we prepare a timestamp skew, to allow us to send all the struct nested elements in an ordered fashion before reaching ::TS_START_SIMULATION or TS_RECV_SPECIFIC_TOPOLOGY.
	ts_skew=0.0;
	//we send the info structure to the destination LP.
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	//we increment the skew
	ts_skew+=0.01;
	//we send the actual structure to the destination LP.
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top,sizeof(elem_top));
	//now we need to send the arrays.
	info.container_struct=SETUP_DATA_ELEMENT_TOPOLOGY;
	///When sending nested pointers in the topology we follow the order of the declaration
	//we send lowerElements
	info.data_size=sizeof(int*)*GET_NUMBER_LOWER_ELEMENTS(elem_top);
	info.data_type=SETUP_DATA_PINT;
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_LOWER_ELEMENTS(elem_top),sizeof(info.data_size));
	//we send connectedLans
	info.data_size=sizeof(int*)*GET_NUMBER_LANS(elem_top);
	info.data_type=SETUP_DATA_PINT;
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_LANS(elem_top),sizeof(info.data_size));
	//we send actuatorPaths
	info.data_size=sizeof(int*)*total_elements;
	info.data_type=SETUP_DATA_PINT;
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_ACTUATOR_PATHS_INDEX(elem_top),sizeof(info.data_size));
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
	setup_info info;
	info.element_id=element_id;
	info.data_size=sizeof(general_topology);
	info.data_type=SETUP_DATA_GENERAL_TOPOLOGY;
	info.container_struct=SETUP_DATA_LP_STATE;
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,gen_top,info.data_size);
	//we send probofActuators
	info.container_struct=SETUP_DATA_GENERAL_TOPOLOGY;
	info.data_type=SETUP_DATA_PDOUBLE;
	info.data_size=sizeof(double)*GET_NUMBER_ACT_TYPES(gen_top);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,GET_PROB_ACTUATORS(gen_top),info.data_size);
}
