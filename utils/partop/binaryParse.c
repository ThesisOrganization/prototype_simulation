#include "header.h"
#include <stdlib.h>
#include <stdio.h>

Element_topology ** parseBinaryElementTopology(int id, int numElements){
    Element_topology ** temp_elem_top = malloc(sizeof(Element_topology*)*numElements);
    char file_name[] = "bin/lp";
    char end_file_name[] = ".bin";

    char file_name_complete[64];
    sprintf(file_name_complete, "%s%d%s" ,file_name,id,end_file_name);
    //printf("file name %s\n",file_name_complete);
    FILE * output_file = fopen(file_name_complete, "r");

    for(int c = 0; c < numElements; c++){
      temp_elem_top[c] = malloc(sizeof(Element_topology));
      fread(&temp_elem_top[c]->id,sizeof(int),1,output_file);

      int at = 0;
      fread(&at, sizeof(int), 1, output_file);

      int st = 0;
      fread(&st, sizeof(int), 1, output_file);

      int lp_type = 0;
      //printf("type : %d \n",lp_type);
      fread(&lp_type, sizeof(int), 1, output_file);
      temp_elem_top[c]->lp_type = lp_type;

      int upperNode = 0;
      fread(&upperNode, sizeof(int), 1, output_file);
      temp_elem_top[c]->upperNode = upperNode;

      //printf("upp %d\n",temp_elem_top[c]->upperNode);

      fread(&temp_elem_top[c]->numberOfLowerElements, sizeof(int), 1, output_file);

      //printf("num low ele %d\n",temp_elem_top[c]->numberOfLowerElements);

      //temp_elem_top[c]->lowerElements = malloc(sizeof(int)*temp_elem_top[c]->numberOfLowerElements);
      //fread(temp_elem_top[c]->lowerElements, sizeof(int), temp_elem_top[c]->numberOfLowerElements, output_file);

      temp_elem_top[c]->lowerElements = NULL;
      fseek(output_file, sizeof(int)*temp_elem_top[c]->numberOfLowerElements, SEEK_CUR);

      fread(&temp_elem_top[c]->numberOfLANS, sizeof(int),1,output_file);

      //temp_elem_top[c]->connectedLans = malloc(sizeof(int)*temp_elem_top[c]->numberOfLANS);
      //fread(temp_elem_top[c]->connectedLans, sizeof(int),temp_elem_top[c]->numberOfLANS,output_file);

      temp_elem_top[c]->connectedLans = NULL;
      fseek(output_file, sizeof(int)*temp_elem_top[c]->numberOfLANS, SEEK_CUR);


      int val_act;
      fread(&val_act,sizeof(int),1, output_file);
      temp_elem_top[c]->numValidActuatorPaths = val_act;

      temp_elem_top[c]->actuatorPaths = malloc(sizeof(idmap*)*val_act);
      fread(temp_elem_top[c]->actuatorPaths,sizeof(idmap),val_act,output_file);


      int nvel;
      fread(&nvel,sizeof(int),1, output_file);
      temp_elem_top[c]->numValidElToLP = nvel;

      temp_elem_top[c]->ElementToLPMapping = malloc(sizeof(idmap)*nvel);
      fread(temp_elem_top[c]->ElementToLPMapping,sizeof(idmap),nvel,output_file);

      fread(&temp_elem_top[c]->cost,sizeof(double),1,output_file);
      if(lp_type == 0){//node
        node_topology * nodeTop = malloc(sizeof(node_topology));

        int node_type;
        fread(&node_type,sizeof(int),1,output_file);
        nodeTop->node_type = node_type;
        int scheduler;
        fread(&scheduler,sizeof(int),1,output_file);
        nodeTop->scheduler = scheduler;

        if(node_type == 0){//Central
          fread(&nodeTop->disk_cost,sizeof(double),1,output_file);

          fread(&nodeTop->disk_type,sizeof(int),1,output_file);
          nodeTop->diskServices = malloc(sizeof(double)*4);

          fread(nodeTop->diskServices,sizeof(double),4,output_file);

        }

        fread(&(nodeTop->cores),sizeof(int),1,output_file);

        nodeTop->aggregation_rate = malloc(sizeof(int)*4);
        fread(nodeTop->aggregation_rate,sizeof(int),4,output_file);

        fread(&nodeTop->delay_upper_router,sizeof(float),1,output_file);
        fread(&nodeTop->delay_lower_router,sizeof(float),1,output_file);

        nodeTop->service_time = malloc(sizeof(double)*5);
        fread(nodeTop->service_time,sizeof(double),5,output_file);

        fread(&nodeTop->probCommandResponse,sizeof(float),1,output_file);
        fread(&nodeTop->id_WAN_up,sizeof(int),1,output_file);
        fread(&nodeTop->id_WAN_down,sizeof(int),1,output_file);

        fread(&nodeTop->numberOfBelowActuators,sizeof(int),1,output_file);
        nodeTop->actuatorsTypesBelow = malloc(sizeof(int)*at);
        fread(nodeTop->actuatorsTypesBelow,sizeof(int),at,output_file);

        fread(&nodeTop->numberOfBelowSensors,sizeof(int),1,output_file);
        //nodeTop->sensorsTypesBelow = malloc(sizeof(int)*st);
        //fread(nodeTop->sensorsTypesBelow,sizeof(int),st,output_file);

        nodeTop->sensorsTypesBelow = NULL;
        fseek(output_file,sizeof(int)*st,SEEK_CUR);

        //nodeTop->ListSensorsByType = malloc(sizeof(int)*nodeTop->numberOfBelowSensors);
        //fread(nodeTop->ListSensorsByType,sizeof(int),nodeTop->numberOfBelowSensors,output_file);

        nodeTop->ListSensorsByType = NULL;
        fseek(output_file,sizeof(int)*nodeTop->numberOfBelowSensors,SEEK_CUR);

        nodeTop->ListActuatorsByType = malloc(sizeof(int)*nodeTop->numberOfBelowActuators);
        fread(nodeTop->ListActuatorsByType,sizeof(int),nodeTop->numberOfBelowActuators,output_file);

        temp_elem_top[c]->spec_top.node = nodeTop;
      }

      else if(lp_type == 1){//sensor
        sensor_topology * sensorTop = malloc(sizeof(sensor_topology));

        fread(&sensorTop->type_job,sizeof(int),1,output_file);
        fread(&sensorTop->sensor_type,sizeof(int),1,output_file);
        fread(&sensorTop->measure_type,sizeof(int),1,output_file);
        fread(&sensorTop->id_LAN_up,sizeof(int),1,output_file);

        sensorTop->sensorRates = malloc(sizeof(double)*2);
        fread(sensorTop->sensorRates,sizeof(double)*2,1,output_file);

        temp_elem_top[c]->spec_top.sensor = sensorTop;
      }

      else if(lp_type == 2){
        actuator_topology * actuatorTop = malloc(sizeof(actuator_topology));
        fread(&actuatorTop->type_job,sizeof(int),1,output_file);
        fread(&actuatorTop->actuator_type,sizeof(int),1,output_file);
        fread(&actuatorTop->measure_type,sizeof(int),1,output_file);
        fread(&actuatorTop->id_LAN_up,sizeof(int),1,output_file);
        fread(&actuatorTop->rateTransition,sizeof(double),1,output_file);
        fread(&actuatorTop->serviceTimeCommand,sizeof(double),1,output_file);
        temp_elem_top[c]->spec_top.actuator = actuatorTop;
      }

      else if(lp_type == 3){//wan
        wan_topology * wanTop = malloc(sizeof(wan_topology));

        fread(&wanTop->wan_type,sizeof(int),1,output_file);
        fread(&wanTop->delay,sizeof(float),1,output_file);

        fread(&wanTop->numberOfBelowActuators,sizeof(int),1,output_file);

        wanTop->actuatorsTypesBelow = malloc(sizeof(int)*at);
        fread(wanTop->actuatorsTypesBelow,sizeof(int),at,output_file);
        fread(&wanTop->numberOfBelowSensors,sizeof(int),1,output_file);

        //wanTop->sensorsTypesBelow = malloc(sizeof(int)*st);
        //fread(wanTop->sensorsTypesBelow,sizeof(int),st,output_file);

        //wanTop->ListSensorsByType = malloc(sizeof(int)*wanTop->numberOfBelowSensors);

        wanTop->sensorsTypesBelow = NULL;
        fseek(output_file,sizeof(int)*st,SEEK_CUR);

        wanTop->ListSensorsByType = NULL;
        fseek(output_file,sizeof(int)*wanTop->numberOfBelowSensors,SEEK_CUR);

        wanTop->ListActuatorsByType = malloc(sizeof(int)*wanTop->numberOfBelowActuators);

        //fread(wanTop->ListSensorsByType,sizeof(int),wanTop->numberOfBelowSensors,output_file);
        fread(wanTop->ListActuatorsByType,sizeof(int),wanTop->numberOfBelowActuators,output_file);

        temp_elem_top[c]->spec_top.wan = wanTop;
      }

      else if(lp_type == 4){//lan
        lan_topology * lanTop = malloc(sizeof(lan_topology));

        fread(&lanTop->lan_type,sizeof(int),1,output_file);
        fread(&lanTop->delay,sizeof(float),1,output_file);

        lanTop->LANsINserviceTimes = malloc(sizeof(double)*5);
        lanTop->LANsOUTserviceTimes = malloc(sizeof(double)*5);
        fread(lanTop->LANsINserviceTimes,sizeof(double),5,output_file);
        fread(lanTop->LANsOUTserviceTimes,sizeof(double),5,output_file);

        fread(&lanTop->numberOfBelowActuators,sizeof(int),1,output_file);

        lanTop->actuatorsTypesBelow = malloc(sizeof(int)*at);
        fread(lanTop->actuatorsTypesBelow,sizeof(int),at,output_file);
        fread(&lanTop->numberOfBelowSensors,sizeof(int),1,output_file);

        //fread(lanTop->sensorsTypesBelow,sizeof(int),st,output_file);

        lanTop->sensorsTypesBelow = NULL;
        fseek(output_file,sizeof(int)*st,SEEK_CUR);

        lanTop->ListSensorsByType = NULL;
        fseek(output_file,sizeof(int)*lanTop->numberOfBelowSensors,SEEK_CUR);

        lanTop->ListActuatorsByType = malloc(sizeof(int)*lanTop->numberOfBelowActuators);

        //fread(lanTop->ListSensorsByType,sizeof(int),lanTop->numberOfBelowSensors,output_file);
        fread(lanTop->ListActuatorsByType,sizeof(int),lanTop->numberOfBelowActuators,output_file);

        temp_elem_top[c]->spec_top.lan = lanTop;

      }
    }

    fclose(output_file);
    return(temp_elem_top);
}


general_topology * parseBinaryGeneralTopology(){
  general_topology * genTop = malloc(sizeof(general_topology));
  char file_name[] = "bin/gentop/";
  char end_file_name[] = "gentopology.bin";

  char file_name_complete[64];
  sprintf(file_name_complete, "%s%s" ,file_name,end_file_name);

  FILE * output_file = fopen(file_name_complete, "r");

  fread(&genTop->total_nodes,sizeof(int),1,output_file);
  fread(&genTop->sensor_nodes,sizeof(int),1,output_file);
  fread(&genTop->actuator_nodes,sizeof(int),1,output_file);
  fread(&genTop->numberOfTotalLANs,sizeof(int),1,output_file);
  fread(&genTop->numberOfTotalWANs,sizeof(int),1,output_file);
  fread(&genTop->numberOfActTypes,sizeof(int),1,output_file);
  fread(&genTop->numberOfSensTypes,sizeof(int),1,output_file);
  fread(&genTop->numberOfLANsTypes,sizeof(int),1,output_file);
  genTop->probOfActuators = malloc(sizeof(double)*genTop->numberOfActTypes);
  fread(genTop->probOfActuators,sizeof(double),genTop->numberOfActTypes,output_file);

  fclose(output_file);
  return(genTop);

}

lp_topology * parseBinaryLpTopology(){
  lp_topology * lpt = malloc(sizeof(lp_topology));
  char file_name[] = "bin/lptop/";
  char end_file_name[] = "lptopology.bin";
  char file_name_complete[64];
  sprintf(file_name_complete, "%s%s" ,file_name,end_file_name);

  FILE * output_file = fopen(file_name_complete, "r");

  int totalElements;
  fread(&totalElements,sizeof(int),1,output_file);
  fread(&lpt->numLP,sizeof(int),1,output_file);
  fread(&lpt->numValid,sizeof(int),1,output_file);

  lpt->amountsOfElementsInLP = malloc(sizeof(int)*lpt->numLP);
  fread(lpt->amountsOfElementsInLP,sizeof(int),lpt->numLP,output_file);
  lpt->ElementToLPMapping = malloc(sizeof(idmap)*totalElements);
  fread(lpt->ElementToLPMapping,sizeof(idmap),totalElements,output_file);

  lpt->LPtoElementMapping = malloc(sizeof(int*)*lpt->numLP);

  int x = 0;
  while(x < lpt->numLP){
    lpt->LPtoElementMapping[x] = malloc(sizeof(int)*lpt->amountsOfElementsInLP[x]);
    fread(lpt->LPtoElementMapping[x],sizeof(int),lpt->amountsOfElementsInLP[x],output_file);
    x+=1;
  }
  fclose(output_file);

  return(lpt);

}

lp_topology * parseBinaryLpTopologyOneLPStripped(int lp){
	lp_topology * lpt = malloc(sizeof(lp_topology));
	char file_name[] = "bin/lptop/";
	char end_file_name[] = "lptopology.bin";
	char file_name_complete[64];
	sprintf(file_name_complete, "%s%s" ,file_name,end_file_name);

	FILE * output_file = fopen(file_name_complete, "r");

	int totalElements;
	fread(&totalElements,sizeof(int),1,output_file);
	fread(&lpt->numLP,sizeof(int),1,output_file);
	fread(&lpt->numValid,sizeof(int),1,output_file);

	lpt->amountsOfElementsInLP = malloc(sizeof(int)*lpt->numLP);
	fread(lpt->amountsOfElementsInLP,sizeof(int),lpt->numLP,output_file);
	//lpt->ElementToLPMapping = malloc(sizeof(idmap)*totalElements);
	//fread(lpt->ElementToLPMapping,sizeof(idmap),totalElements,output_file);
	lpt->ElementToLPMapping=NULL;
	fseek(output_file,sizeof(idmap)*totalElements,SEEK_CUR);

	lpt->LPtoElementMapping = malloc(sizeof(int*));

	int x = 0;
	while(x < lp){
		fseek(output_file,sizeof(int)*lpt->amountsOfElementsInLP[x],SEEK_CUR);
		x+=1;
	}
	lpt->LPtoElementMapping[0] = malloc(sizeof(int)*lpt->amountsOfElementsInLP[lp]);
	fread(lpt->LPtoElementMapping[0],sizeof(int),lpt->amountsOfElementsInLP[lp],output_file);
	fclose(output_file);

	return(lpt);

}

void destroyBinaryLpTopologyOneLPStripped(lp_topology* lp_top){
	free(lp_top->LPtoElementMapping[0]);
	free(lp_top->LPtoElementMapping);
	free(lp_top->amountsOfElementsInLP);
	free(lp_top);
}
