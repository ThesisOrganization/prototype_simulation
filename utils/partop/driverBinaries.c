#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int main()
{
  //We retrieve the topology, answer the question of who needs to receive for each sender node in the data
  //char * path = "topology.txt";
  char * path = "../../tree_simulator/topology.txt";
  char * path1 = "../../tree_simulator/LP.txt";
  lp_topology * lptopo =  getLPtopoogy(path1);

  total_topology * totTop = getTopology(path,path1);
  general_topology * genTop = getGenTopology(totTop);

  int nn = getTotalNodes(genTop);
  int ns = getSensorNodes(genTop);
  int na = getActuatorNodes(genTop);
  int nl = getNumberOfTotalLANs(genTop);
  int nw = getNumberOfTotalWANs(genTop);

  int totalElements = nn+ns+na+nl+nw;

  //int lt = getNumberOfLANsTypes(genTop);
  generateBinaries(genTop,totTop,totalElements, lptopo);
  generateBinaries_lpTopology(lptopo,totalElements);

  destroyTotalTopology(totTop);

  //lp_topology * lptop = parseBinaryLpTopology();
  //int nael = getAmountsOfElementsInOneLP(lptop,0);
  //Element_topology ** returnTopology = parseBinaryElementTopology(0,nael);

  //for(int i = 0; i < totalElements; i++){
    //Element_topology * returnTopology = parseBinaryElementTopology(i);
  //}
  //general_topology * gnt = parseBinaryGeneralTopology();

}
