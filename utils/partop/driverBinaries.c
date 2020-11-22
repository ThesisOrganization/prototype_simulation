#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int main(int argc, char** argv)
{
	if(argc <4){
		printf("not enough arguments specified:\n Usage: driverBinaries [path_to_topology.txt] [path_to_LP.txt] [output_path]\n");
		exit(EXIT_FAILURE);
	}
  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data
  //char * path = "topology.txt";
  char * path = argv[1];
  char * path1 = argv[2];
  lp_topology * lptopo =  NULL;

  total_topology * totTop = getTopology(path,path1);
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
