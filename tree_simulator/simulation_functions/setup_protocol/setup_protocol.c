/** \file setup_protocol.c
 * \brief Implementation of the LP setup functions.
 */
#include "setup_protocol.h"
#include "setup_send_functions.h"
#include "setup_recv_functions.h"
#include <string.h>

/// The master node will schedule topology messages for itself to minimize memory consuption.
void setup_master(int n_prc_tot){
	int lp;
	//we use the parser to get informations about the general topology
	total_topology *tot_top=GET_TOPOLOGY("topology.txt","LP.txt", LP_AGGR_NONE);
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
		printf("Warning: Too much LPs, simulation will continue, but %d LPs will not be actively used, simulation could abruptly terminate during setup.\n",n_prc_tot-num_required_lps);
		for(lp=num_required_lps;lp<n_prc_tot;lp++){
			ScheduleNewEvent(lp,TS_DISABLE_UNUSED_LP,DISABLE_UNUSED_LP,NULL,0);
		}
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
	DESTROY_TOTAL_TOPOLOGY(tot_top);
	printf("setup messages sent\n");
}

void setup_independent(int lp,lp_state* state, int n_prc_tot){
	int i,map_len;
	///Each LP reads from file the information about the topology.
	state->general_topology= parseBinaryGeneralTopology();
	/// Then checks if the number of LPs is right
	lp_topology* lp_top=parseBinaryLpTopologyOneLPStripped(lp);
	int num_required_lps=lp_top->numLP;
	if(num_required_lps > n_prc_tot){
		printf("Error: too few LPs, add more LPs\n");
		exit(EXIT_FAILURE);
	}
		/// If there LP in question is not necessary it will disable itself.
	if(lp>=num_required_lps){
		state->lp_enabled=LP_DISABLED;
		destroyBinaryLpTopologyOneLPStripped(lp_top);
		printf("Warning: Too much LPs (%d more than necessary), simulation will continue, but LP %d will not be actively used.\n",n_prc_tot-num_required_lps,lp);
	} else {
		///Otherwise it will continue the setup.
		//we need to get the general topology

		//we send the number of used LPs.
		state->number_lps_enabled=num_required_lps;
		//we allocate the devices array
		int num_devices=lp_top->amountsOfElementsInLP[lp];
		state->num_devices=num_devices;
		state->devices_array=malloc(sizeof(device_state*)*num_devices);
		memset(state->devices_array,0,sizeof(device_state*)*num_devices);
		//we initialize the idmap
		int *indexes=malloc(sizeof(int)*num_devices);
		int *devices=lp_top->LPtoElementMapping[0];
		for(i=0;i<num_devices;i++){
			indexes[i]=i;
		}
		state->element_to_index=create_idmap(devices,indexes,num_devices,&map_len);
		free(indexes);
		//we initialize the devices array
		Element_topology** elem_top_array=parseBinaryElementTopology(lp,num_devices);
		for(i=0;i<state->num_devices;i++){
			state->devices_array[i]=malloc(sizeof(device_state));
			memset(state->devices_array[i],0,sizeof(device_state));
			state->devices_array[i]->topology=elem_top_array[i];
		}
		destroyBinaryLpTopologyOneLPStripped(lp_top);
	}
	/// Finally, when the setup is completed, the LP will schedule the START_SIMULATION event for itself.
	ScheduleNewEvent(lp,TS_START_SIMULATION,START_SIMULATION,NULL,0);
	//printf("LP %d has completed the setup.\n",lp);
}
