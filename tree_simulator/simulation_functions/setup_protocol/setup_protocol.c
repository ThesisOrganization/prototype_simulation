/** \file setup_protocol.c
 * \brief Implementation of the LP setup functions.
 */
#include "setup_protocol.h"
#include "setup_send_functions.h"
#include "setup_recv_functions.h"

/// The master node will schedule topology messages for itself to minimize memory consuption.
void setup_master(int n_prc_tot){
	int lp;
	//we use the parser to get informations about the general topology
	total_topology *tot_top=GET_TOPOLOGY("topology.txt","LP.txt");
	printf("Loaded topology\n");
	//then we need to get the general topology
	general_topology *gen_top=GET_GEN_TOPOLOGY(tot_top);
	//now we can check if the number of LPs is right
	int num_required_lps=GET_NUM_LPS(tot_top->lp_topology);
	if(num_required_lps > n_prc_tot){
		printf("Error: too few LPs, add more LPs\n");
		exit(EXIT_FAILURE);
	}
	if(num_required_lps<n_prc_tot){
		printf("Warning: Too much LPs, simulation will continue, but %d LPs will not be actively used.\n",n_prc_tot-num_required_lps);
	}
	printf("sending topology information to other LPs\n");
	//we send the topology data to each LP.
	///we assume that the ID of the LP matches with the id of the element __This is a heavy assumption__.
	///The master node will schedule event for all the nodes and even itself
	for(lp=0;lp<num_required_lps;lp++){
		send_general_topology(gen_top,lp);
		//we send the info message that will setup the devices_array in the local node.
		send_lp_info(lp,tot_top);
		//after having sent all the topology info to the node we can activate it
		ScheduleNewEvent(lp,TS_START_SIMULATION,START_SIMULATION,NULL,0);
	}
	//we free the total topology since it's not needed
	//DESTROY_TOTAL_TOPOLOGY(tot_top);
	printf("setup messsages sent\n");
}
