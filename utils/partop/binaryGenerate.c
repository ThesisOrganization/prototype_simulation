#include "header.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
void generateBinaries_elementTopology(general_topology * genTop,total_topology * totTop, int * elonelp, int a, int num,char* path){

  int at = getNumberOfActTypes(genTop);
  int st = getNumberOfSensTypes(genTop);

	int len=snprintf(NULL,0,"%s/bin/lp%d",path,a)+1;
  char* file_name=malloc(sizeof(char)*len);
	snprintf(file_name,len,"%s/bin/lp%d",path,a);
  char end_file_name[] = ".bin";

	int len_complete=len+strlen(end_file_name);
  char *file_name_complete=malloc(sizeof(char)*len_complete);
  snprintf(file_name_complete,len_complete, "%s%s" ,file_name,end_file_name);
	free(file_name);

  FILE * output_file = fopen(file_name_complete, "w");
    for(int id = 0; id < num; id++){
      int i = elonelp[id];
      fwrite(&i,sizeof(int),1,output_file);

      Element_topology * temp_lpt =  getLPTopology(totTop,i);

      fwrite(&at, sizeof(int), 1, output_file);

      fwrite(&st, sizeof(int), 1, output_file);

      int lp_type = getType(temp_lpt);
      fwrite(&lp_type, sizeof(int), 1, output_file);

      int upp = getUpperNode(temp_lpt);
      fwrite(&upp, sizeof(int), 1, output_file);

      int numLow = getNumberLower(temp_lpt);

      fwrite(&numLow, sizeof(int),1, output_file);

      fwrite(getLowers(temp_lpt), sizeof(int),numLow, output_file);


      int numLANS = getNumberLANS(temp_lpt);
      fwrite(&numLANS, sizeof(int),1, output_file);

      int * LANS = getLANS(temp_lpt);
      int c = 0;
      if(LANS[0] != -1){
        fwrite(&LANS[0], sizeof(int),1, output_file);
        c+=1;
        while(c < numLANS){
          int tempLan = LANS[c];
          c+=1;
          fwrite(&tempLan, sizeof(int),1, output_file);
        }

      }

      int val_act = getNumValidActuatorPaths(temp_lpt);
      fwrite(&val_act,sizeof(int),1, output_file);
      idmap * idmAct = getActuatorPaths(temp_lpt);
      for(int temp_index = 0; temp_index < val_act; temp_index++){
        fwrite(&idmAct[temp_index],sizeof(idmap),1,output_file);
      }

      int nvel = getNumValidElToLP(temp_lpt);
      fwrite(&nvel,sizeof(int),1, output_file);
      idmap *  eltomp = getElToLPMapping(temp_lpt);
      for(int temp_index = 0; temp_index < nvel; temp_index++){
        fwrite(&eltomp[temp_index],sizeof(idmap),1,output_file);
      }

      //int numValidElToLP;
      //idmap * ElementToLPMapping;
      if(lp_type == 0){//node
        int node_type = getNodeType(temp_lpt);
        fwrite(&node_type,sizeof(int),1,output_file);
        int scheduler = getScheduler(temp_lpt);
        fwrite(&scheduler,sizeof(int),1,output_file);

        if(node_type == 0){//Central
          int disk_type = getDiskType(temp_lpt);
          fwrite(&disk_type,sizeof(int),1,output_file);
          double * diskServices = getDiskServices(temp_lpt);
          for(int k = 0;k < 4;k++){
            fwrite(&diskServices[k],sizeof(double),1,output_file);
            }
          }
          int cores=getNumberOfCores(temp_lpt);
          fwrite(&cores,sizeof(int),1,output_file);

          int * aggregation_rates = getAggregationRate(temp_lpt);
          for(int k = 0;k < 4;k++){
            fwrite(&aggregation_rates[k],sizeof(int),1,output_file);
          }

          float delay_upper_router = getDelayUpperRouter(temp_lpt);
          fwrite(&delay_upper_router,sizeof(float),1,output_file);

          float delay_lower_router = getDelayLowerRouter(temp_lpt);
          fwrite(&delay_lower_router,sizeof(float),1,output_file);

          double * serviceTimes = getServiceTimesNodes(temp_lpt);
          for(int k = 0;k < 5;k++){
            fwrite(&serviceTimes[k],sizeof(double),1,output_file);
          }

          float pr = getProbCommandResponse(temp_lpt);
          fwrite(&pr,sizeof(float),1,output_file);

          int id_WAN_up = getWanUp(temp_lpt);
          fwrite(&id_WAN_up,sizeof(int),1,output_file);

          int id_WAN_down = getWanDown(temp_lpt);
          fwrite(&id_WAN_down,sizeof(int),1,output_file);

      }

      else if(lp_type == 1){//sensor

          int type_job = getTypeJob(temp_lpt);
          fwrite(&type_job,sizeof(int),1,output_file);

          int sensor_type = getSensorType(temp_lpt);
          fwrite(&sensor_type,sizeof(int),1,output_file);

          int measure_type = getMeasureType(temp_lpt);
          fwrite(&measure_type,sizeof(int),1,output_file);

          int id_LAN_up = getLANup(temp_lpt);
          fwrite(&id_LAN_up,sizeof(int),1,output_file);

          double * sensorRates = getSensorRates(temp_lpt);
          for(int k = 0; k < 2; k++){
            fwrite(&sensorRates[k],sizeof(double),1,output_file);
          }

      }

      else if(lp_type == 2){

        int type_job = getTypeJob(temp_lpt);
        fwrite(&type_job,sizeof(int),1,output_file);

        int actuator_type = getActuatorType(temp_lpt);
        fwrite(&actuator_type,sizeof(int),1,output_file);

        int measure_type = getMeasureType(temp_lpt);
        fwrite(&measure_type,sizeof(int),1,output_file);

        int id_LAN_up = getLANup(temp_lpt);
        fwrite(&id_LAN_up,sizeof(int),1,output_file);

        double rateTransition = getRateTransition(temp_lpt);
        fwrite(&rateTransition,sizeof(double),1,output_file);

        double serviceTimeCommand = getServiceTimeCommand(temp_lpt);
        fwrite(&serviceTimeCommand,sizeof(double),1,output_file);

      }

      else if(lp_type == 3){//wan
        int wan_type = getWanType(temp_lpt);
        fwrite(&wan_type,sizeof(int),1,output_file);

        float delay = getDelay(temp_lpt);
        fwrite(&delay,sizeof(float),1,output_file);

        }

        else if(lp_type == 4){//lan
          int lan_type = getLanType(temp_lpt);
          fwrite(&lan_type,sizeof(int),1,output_file);

          float delay = getDelay(temp_lpt);
          fwrite(&delay,sizeof(float),1,output_file);

          double * LANsINserviceTimes = getLANsINserviceTimesByType(temp_lpt);
          double * LANsOUTserviceTimes = getLANsOUTserviceTimesByType(temp_lpt);
          fwrite(LANsINserviceTimes,sizeof(double)*5,1,output_file);
          fwrite(LANsOUTserviceTimes,sizeof(double)*5,1,output_file);

      }


      if(lp_type == 0 || lp_type == 3 || lp_type == 4){

        int numberOfBelowActuators = getNumberOfBelowActuators(temp_lpt);
        fwrite(&numberOfBelowActuators,sizeof(int),1,output_file);

        int * actuatorsTypesBelow = getActuatorTypesBelowList(temp_lpt);
        for(int k = 0; k < at; k++){
          fwrite(&actuatorsTypesBelow[k],sizeof(int),1,output_file);
        }

        int numberOfBelowSensors = getNumberOfBelowSensors(temp_lpt);
        fwrite(&numberOfBelowSensors,sizeof(int),1,output_file);

        int * sensorsTypesBelow = getSensorsTypesBelowList(temp_lpt);
        for(int k = 0; k < st; k ++){
          fwrite(&sensorsTypesBelow[k],sizeof(int),1,output_file);
        }

        for(int k = 0;k < st;k++){
          int * ListSensorsByType = getListSensorsByType(temp_lpt, k);
          if(sensorsTypesBelow[k] != 0){
            fwrite(&ListSensorsByType[0],sizeof(int),1,output_file);

            for(int t = 1; t < sensorsTypesBelow[k]; t++){
              fwrite(&ListSensorsByType[t],sizeof(int),1,output_file);

            }
          }

        }

        for(int k = 0;k < at;k++){
          int * ListActuatorsByType = getListActuatorsByType(temp_lpt, k);
          if(actuatorsTypesBelow[k] != 0){
            fwrite(&ListActuatorsByType[0],sizeof(int),1,output_file);
            for(int t = 1; t < actuatorsTypesBelow[k]; t++){
              fwrite(&ListActuatorsByType[t],sizeof(int),1,output_file);
            }
          }
        }

      }

    }
    free(file_name_complete);
    fclose(output_file);

}



void generateBinaries_generalTopology(total_topology * totTop,char* path){
  general_topology * genTop = getGenTopology(totTop);

	int len=snprintf(NULL,0,"%s/bin/gentop/",path)+1;

	char* file_name=malloc(sizeof(char)*len);
	snprintf(file_name,len,"%s/bin/gentop/",path);

  char end_file_name[] = "gentopology.bin";
	int len_complete=len+strlen(end_file_name);
  char *file_name_complete=malloc(sizeof(char)*len_complete);
  snprintf(file_name_complete,len_complete, "%s%s" ,file_name,end_file_name);
	free(file_name);

  FILE * output_file = fopen(file_name_complete, "w");

  int nn = getTotalNodes(genTop);
  fwrite(&nn,sizeof(int),1,output_file);

  int ns = getSensorNodes(genTop);
  fwrite(&ns,sizeof(int),1,output_file);

  int na = getActuatorNodes(genTop);
  fwrite(&na,sizeof(int),1,output_file);

  int nl = getNumberOfTotalLANs(genTop);
  fwrite(&nl,sizeof(int),1,output_file);

  int nw = getNumberOfTotalWANs(genTop);
  fwrite(&nw,sizeof(int),1,output_file);

  int at = getNumberOfActTypes(genTop);
  fwrite(&at,sizeof(int),1,output_file);

  int st = getNumberOfSensTypes(genTop);
  fwrite(&st,sizeof(int),1,output_file);

  int lt = getNumberOfLANsTypes(genTop);
  fwrite(&lt,sizeof(int),1,output_file);

  double * probAct = getProbOfActuators(genTop);
  fwrite(probAct,sizeof(double),at,output_file);

	free(file_name_complete);
  fclose(output_file);
}

void generateBinaries_lpTopology(lp_topology * lpTop,int totalElements,char* path){
	int len=snprintf(NULL,0,"%s/bin/lptop/",path)+1;
	char* file_name=malloc(sizeof(char)*len);
	snprintf(file_name,len,"%s/bin/lptop/",path);
  char end_file_name[] = "lptopology.bin";
	int len_complete=len+strlen(end_file_name);
  char *file_name_complete=malloc(sizeof(char)*len_complete);
  snprintf(file_name_complete,len_complete,"%s%s" ,file_name,end_file_name);
	free(file_name);
  FILE * output_file = fopen(file_name_complete, "w");


  fwrite(&totalElements,sizeof(int),1,output_file);
  int numLP = getNumLP(lpTop);
  fwrite(&numLP,sizeof(int),1,output_file);
  int numValid = getNumValid(lpTop);
  fwrite(&numValid,sizeof(int),1,output_file);
  fwrite(getAmountsOfElements(lpTop),sizeof(int),numLP,output_file);
  fwrite(getElementToLPMapping(lpTop),sizeof(idmap),totalElements,output_file);

  int x = 0;
  while(x < numLP){
    int * elementArray = getLPtoElementMappingOneLP(lpTop,x);
    int amount = getAmountsOfElementsInOneLP(lpTop,x);
    fwrite(elementArray,sizeof(int),amount,output_file);
    x+=1;
  }
  free(file_name_complete);
  fclose(output_file);
}

void generateBinaries(general_topology * genTop,total_topology * totTop,int totalElements, lp_topology * lpTop,char* path){
  generateBinaries_generalTopology(totTop,path);
  int LpNum = getNumLP(lpTop);
  int a = 0;
  while (a < LpNum){
    int * elonelp = getLPtoElementMappingOneLP(lpTop,a);
    int num = getAmountsOfElementsInOneLP(lpTop,a);
    generateBinaries_elementTopology(genTop,totTop,elonelp,a, num,path);
    a++;
  }

}
