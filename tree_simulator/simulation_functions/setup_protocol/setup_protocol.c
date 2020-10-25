#include "setup_protocol.h"
#include<stdio.h>

/** \brief Sends a sensor_topology to the specified element_id
 * \param[in] elem_top The Element_topology containing the sensor_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a sensor contains a sensor_topology in the spec_top parameter.
 */
void send_sensor_topology(Element_topology* elem_top,int element_id){
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	info.element_id=element_id;
	info.next_data_size=sizeof(sensor_topology);
	info.next_data_type=SETUP_DATA_SENSOR_TOPOLOGY;
	//we send the sensor_topology struct
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.sensor,info.next_data_size);
	//we send the sensorRates array
	info.next_data_type=SETUP_DATA_PDOUBLE;
	info.next_data_size=sizeof(double)*2; // sensors use only two classes of data
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.sensor->sensorRates,info.next_data_size);
}

/** \brief Sends an actuator_topology to the specified element_id
 * \param[in] elem_top The Element_topology containing the actuator_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a actuator contains an actuator_topology in the spec_top parameter.
 */
void send_actuator_topology(Element_topology* elem_top, int element_id){
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	info.element_id=element_id;
	info.next_data_size=sizeof(actuator_topology);
	info.next_data_type=SETUP_DATA_ACTUATOR_TOPOLOGY;
	//we send the sensor_topology struct
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.actuator,info.next_data_size);
}

/** \brief Send info about the below actuators and sensors to the LP corresponding to element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology of the current element.
 * \param[in] element_id The id of __both__ the element and the LP.
 */
void send_below_devices_info(general_topology* gen_top,Element_topology* elem_top,int element_id){
	setup_info info;
	double ts_skew=0.0;
	info.element_id=element_id;
	//we send actuatorsTypesBelow
	info.next_data_type=SETUP_DATA_PINT;
	info.next_data_size=GET_NUMBER_ACT_TYPES(gen_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan->actuatorsTypesBelow,info.next_data_size);
	//we send sensorTypesBelow
	info.next_data_size=GET_NUMBER_SENS_TYPES(gen_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan->sensorsTypesBelow,info.next_data_size);
	//we send ListSensorsByType
	info.next_data_size=GET_NUMBER_SENS_BELOW(elem_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan->ListSensorsByType,info.next_data_size);
	//we send ListActuatorsByType
	info.next_data_size=GET_NUMBER_ACT_BELOW(elem_top)*sizeof(int);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_BELOW_DEVICES_INFO+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan->ListActuatorsByType,info.next_data_size);
}

/** \brief Sends a lan_topology to the specified element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology containing the lan_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a lan contains an lan_topology in the spec_top parameter.
 */
void send_lan_topology(general_topology* gen_top,Element_topology* elem_top,int element_id){
	setup_info info;
	double ts_skew=0.0; //We need a skew to avoid reaching the ::TS_START_SIMULATION timestamp untile we have sent all the nested pointers of the sensor_topology.
	info.element_id=element_id;
	info.next_data_size=sizeof(lan_topology);
	info.next_data_type=SETUP_DATA_LAN_TOPOLOGY;
	//we send the sensor_topology struct
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan,info.next_data_size);
	//we send LanINserviceTimes
	info.next_data_type=SETUP_DATA_PDOUBLE;
	info.next_data_size=sizeof(double)*5; //we have a service time for each message class plus one for the replies.
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan->LANsINserviceTimes,info.next_data_size);
	//we send LansOUTserviceTimes
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.lan->LANsOUTserviceTimes,info.next_data_size);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	send_below_devices_info(gen_top,elem_top,element_id);
}

/** \brief Sends a wan_topology to the specified element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology containing the wan_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a wan contains an wan_topology in the spec_top parameter.
 */
void send_wan_topology(general_topology* gen_top, Element_topology* elem_top,int element_id){
	setup_info info;
	double ts_skew=0.0;
	info.element_id=element_id;
	//we send the wan_topology struct
	info.next_data_size=sizeof(wan_topology);
	info.next_data_type=SETUP_DATA_WAN_TOPOLOGY;

	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.wan,info.next_data_size);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	send_below_devices_info(gen_top,elem_top,element_id);
}

/** \brief Sends a node_topology to the specified element_id
 * \param[in] gen_top The general_topology struct with the informations about element types.
 * \param[in] elem_top The Element_topology containing the node_topology we want to send.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * This function leverages the fact that a node contains a node_topology in the spec_top parameter.
 */
void send_node_topology(general_topology* gen_top, Element_topology* elem_top,int element_id){
	setup_info info;
	double ts_skew=0.0;
	info.element_id=element_id;
	//we send the wan_topology struct
	info.next_data_size=sizeof(node_topology);
	info.next_data_type=SETUP_DATA_NODE_TOPOLOGY;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.node,info.next_data_size);
	//we send diskServices
	info.next_data_type=SETUP_DATA_PDOUBLE;
	info.next_data_size=sizeof(double)*4; // we have a disk service time  for each class of data
	ts_skew+=0.01;	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.node->diskServices,info.next_data_size);
	//we send aggregation_rate
	info.next_data_type=SETUP_DATA_PINT;
	info.next_data_size=sizeof(double)*4; // we have an aggregation rate for each class of data
	ts_skew+=0.01;	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.node->aggregation_rate,info.next_data_size);
	//we send service_time
	info.next_data_type=SETUP_DATA_PDOUBLE;
	info.next_data_size=sizeof(double)*5; // we have a service for each class of data plus the reply events
	ts_skew+=0.01;	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_SPECIFIC_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->spec_top.node->service_time,info.next_data_size);
	//to send informations about the below sensors and actuator we use send_below_devices_info
	send_below_devices_info(gen_top,elem_top,element_id);
}

/** \brief Sends an Element_topology struct to the destination LP.
 * \param[in] tot_top Total topology struct.
 * \param[in] element_id The id of __both__ the destination LP and the element of the topology.
 * \param[in] total_elements The total number of elements in the topology.
 */
void send_element_topology(total_topology* tot_top,int element_id, int total_elements){
	Element_topology *elem_top;
	general_topology *gen_top=GET_GEN_TOPOLOGY(tot_top);
	setup_info info;
	double ts_skew;
	//we retrieve the topology of the element
	elem_top=GET_ELEMENT_TOPOLOGY(tot_top,element_id);
	//we prepare a data_info struct
	info.element_id=element_id;
	info.next_data_size=sizeof(Element_topology);
	info.next_data_type=SETUP_DATA_ELEMENT_TOPOLOGY;
	//before sending the stuct we prepare a timestamp skew, to allow us to send all the struct nested elements in an ordered fashion before reaching ::TS_START_SIMULATION or TS_RECV_SPECIFIC_TOPOLOGY.
	ts_skew=0.0;
	//we send the info structure to the destination LP.
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	//we increment the skew
	ts_skew+=0.01;
	//we send the actual structure to the destination LP.
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top,sizeof(elem_top));
	//now we need to send the arrays.
	///When sending nested pointers in the topology we follow the order of the declaration
	//we send lowerElements
	info.next_data_size=sizeof(int*)*elem_top->numberOfLowerElements;
	info.next_data_type=SETUP_DATA_PINT;
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->lowerElements,sizeof(info.next_data_size));
	//we send connectedLans
	info.next_data_size=sizeof(int*)*elem_top->numberOfLANS;
	info.next_data_type=SETUP_DATA_PINT;
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->connectedLans,sizeof(info.next_data_size));
	//we send actuatorPaths
	info.next_data_size=sizeof(int*)*total_elements;
	info.next_data_type=SETUP_DATA_PINT;
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->actuatorPaths,sizeof(info.next_data_size));
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

/** \brief Sends a general_topology to an LP identified by element_id
 * \param[in] gen_top The general_topology to send.
 * \param[in] element_id The id of __both__ the LP and the element.
 */
void send_general_topology(general_topology* gen_top,int element_id){
	double ts_skew=0.0; // we need a skew to send the topology info and data before reaching another point in time like TS_START_SIMULATION.
	setup_info info;
	info.element_id=element_id;
	info.next_data_size=sizeof(general_topology);
	info.next_data_type=SETUP_DATA_GENERAL_TOPOLOGY;
		ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));

	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,gen_top,info.next_data_size);
	//we send probofActuators
	info.next_data_type=SETUP_DATA_PDOUBLE;
	info.next_data_size=sizeof(double)*GET_NUMBER_ACT_TYPES(gen_top);
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(info));

	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_GENERAL_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,gen_top->probOfActuators,info.next_data_size);
}

/// The master node will schedule topology messages for itself to minimize memory consuption.
void setup_master(int n_prc_tot){
	int i;
	double ts_skew;
	setup_info info;
	Element_topology *elem_top=NULL;
	//we use the parser to get informations about the general topology
	total_topology *tot_top=GET_TOPOLOGY("topology.txt");
	//then we need to get the general topology
	general_topology *gen_top=GET_GEN_TOPOLOGY(tot_top);
	//now we can check if the number of LPs is right
	int num_required_lps=GET_TOTAL_NODES(gen_top)+GET_SENSOR_NODES(gen_top)+GET_ACTUATOR_NODES(gen_top)+GET_NUMBER_OF_LANS(gen_top)+GET_NUMBER_OF_WANS(gen_top);
	if(num_required_lps > n_prc_tot){
		printf("Error: too few LPs, add more LPs\n");
		exit(EXIT_FAILURE);
	}
	if(num_required_lps<n_prc_tot){
		printf("Warning: Too much LPs, simulation will continue, but %d LPs will not be actively used.\n",n_prc_tot-num_required_lps);
	}
	//we send the topology data to each LP.
	///we assume that the ID of the LP matches with the id of the element __This is a heavy assumption__.
	///The master node will schedule event for all the nodes and even itself
	for(i=0;i<num_required_lps;i++){
		send_general_topology(gen_top,i);
		send_element_topology(tot_top,i,num_required_lps);
		//after having sent all the topology info to the node we can activate it
		ScheduleNewEvent(i,TS_START_SIMULATION,START_SIMULATION,NULL,0);
		//TODO free Element_topology.
	}
	//TODO free general_topology and total_topology.
}
