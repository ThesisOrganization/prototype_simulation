#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int main(int argc, char** argv)
{
	if(argc < 4){
		printf("not enough arguments specified:\n Usage: driverBinaries [path_to_topology.txt] [path_to_LP.txt]  [output_path]\n optional positional argument (to use as last argument):[--lp_aggregation_criteria=[central,regional,local,lan]]\n the lp aggregation criteria controls how the elements are aggregated in LPs, refere to header.h documentation for more info.");
		exit(EXIT_FAILURE);
	}
  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data
  //char * path = "topology.txt";
  char * path = argv[1];
  char * path1 = argv[2];
	lp_aggregation_criteria criteria=LP_AGGR_REGIONAL;
	if(argc > 4){
		char* chosen_criteria=argv[4];
		if(strncmp(chosen_criteria,"--lp_aggregation_criteria=local",strlen("--lp_aggregation_criteria=local"))!=0 && strncmp(chosen_criteria,"--lp_aggregation_criteria=lan",strlen("--lp_aggregation_criteria=lan"))!=0 && strncmp(chosen_criteria,"--lp_aggregation_criteria=regional",strlen("--lp_aggregation_criteria=regional"))!=0 && strncmp(chosen_criteria,"--lp_aggregation_criteria=central",strlen("--lp_aggregation_criteria=central"))!=0){
			printf("unrecognized lp aggregation criteria\n");
			exit(EXIT_FAILURE);
		}
		if(strncmp(chosen_criteria,"--lp_aggregation_criteria=central",strlen("--lp_aggregation_criteria=central"))==0){
			criteria = LP_AGGR_CENTRAL;
		}
		if(strncmp(chosen_criteria,"--lp_aggregation_criteria=local",strlen("--lp_aggregation_criteria=local"))==0){
			criteria = LP_AGGR_LOCAL;
		}
		if(strncmp(chosen_criteria,"--lp_aggregation_criteria=lan",strlen("--lp_aggregation_criteria=lan"))==0){
			criteria = LP_AGGR_LAN;
		}
	}
	lp_topology * lptopo =  NULL;

  total_topology * totTop = getTopology(path,path1,criteria);
  general_topology * genTop = getGenTopology(totTop);
	lptopo=totTop->lp_topology;

  int nn = getTotalNodes(genTop);
  int ns = getSensorNodes(genTop);
  int na = getActuatorNodes(genTop);
  int nl = getNumberOfTotalLANs(genTop);
  int nw = getNumberOfTotalWANs(genTop);

  int totalElements = nn+ns+na+nl+nw;

  //int lt = getNumberOfLANsTypes(genTop);
  generateBinaries(genTop,totTop,totalElements, lptopo,argv[3]);
  generateBinaries_lpTopology(lptopo,totalElements,argv[3]);

  destroyTotalTopology(totTop);

  //lp_topology * lptop = parseBinaryLpTopology();
  //int nael = getAmountsOfElementsInOneLP(lptop,0);
  //Element_topology ** returnTopology = parseBinaryElementTopology(0,nael);

  //for(int i = 0; i < totalElements; i++){
    //Element_topology * returnTopology = parseBinaryElementTopology(i);
  //}
  //general_topology * gnt = parseBinaryGeneralTopology();

}
