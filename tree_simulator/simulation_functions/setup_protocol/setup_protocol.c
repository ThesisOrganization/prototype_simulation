#include "setup_protocol.h"
#include<stdio.h>


/**
*/
void* alloc_data(setup_data_types data_type, size_t data_size){

}


void send_sensor_topology();
void send_actuator_topology();
void send_lan_topology();
void send_wan_topology();
void send_node_topology();

/** \brief Sends an Element_topology struct to the destination LP.
 * \param[in] tot_top Total topology struct.
 * \param[in] element_id The id of __BOTH__ the destination LP and the elemento of the topology
 */
void send_element_topology(total_topology* tot_top,int element_id){
	Element_topology *elem_top;
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
	info.next_data_size=sizeof(int*)*GET_NUMBER_ACT_BELOW(elem_top);
	info.next_data_type=SETUP_DATA_PINT;
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_INFO,&info,sizeof(setup_info));
	ts_skew+=0.01;
	ScheduleNewEvent(element_id,TS_RECV_ELEMENT_TOPOLOGY+ts_skew,RECEIVE_SETUP_DATA,elem_top->actuatorPaths,sizeof(info.next_data_size));
	//to send the specific_topology we need to check the element type and access the union accordingly
	switch(GET_TYPE(elem_top)){
		case SENSOR:
			send_sensor_topology(elem_top->spec_top);
			break;
		case ACTUATOR:
			send_actuator_topology(elem_top->spec_top);
			break;
		case LAN:
			send_lan_topology(elem_top->spec_top);
		case WAN:
			send_wan_topology(elem_top->spec_top);
			break;
		case NODE:
			send_node_topology(elem_top->spec_top);
			break;
		default:
			printf("Error: unknown element type\n");
			exit(EXIT_FAILURE);
	}
}

/**
*/
void setup_master(int me,int n_prc_tot,lp_state* master_state){
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
		send_element_topology(tot_top,i);
	}

	ScheduleNewEvent();
	//finally we get the topology info of the node which the master LP will simulate after the setup is complete
}
