#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>
#include "../application_datatypes.h"

void * parse_strings(char ** strings, int types, int upperNode){
    lp_infos * infos = malloc(sizeof(lp_infos));
    char * ptr;
    int flag = 0;

    if( !strcmp(strings[0], "NODE") ){

        infos->lp_type = NODE;

        if( !strcmp(strings[1], "SCHEDULER1") )
            infos->scheduler = SCHEDULER1;
        else if( !strcmp(strings[1], "SCHEDULER2") )
            infos->scheduler = SCHEDULER2;
        else if( !strcmp(strings[1], "SCHEDULER3") )
            infos->scheduler = SCHEDULER3;
        else
            exit(EXIT_FAILURE);

        if( !strcmp(strings[2], "CENTRAL") )
            infos->node_type = CENTRAL;
        else if( !strcmp(strings[2], "REGIONAL") )
            infos->node_type = REGIONAL;
        else if( !strcmp(strings[2], "LOCAL") )
            infos->node_type = LOCAL;
        else
            exit(EXIT_FAILURE);

        int aggregation_rate = atoi(strings[3]);
        infos->aggregation_rate = aggregation_rate;

        float delayUP = strtod(strings[4],&ptr);
        infos->delay_upper_router = delayUP;
        float delayDOWN = atof(strings[5]);
        infos->delay_lower_router = delayDOWN;

        char * end_str;
        char * end_ptr;
        char * ptr = strtok_r(strings[6], "/", &end_str);
        double * serviceArray = malloc((sizeof(double)) * 5); //fixed, 5 type of data.
        int counter = 0;
        while(ptr){
          serviceArray[counter] = strtod(ptr, &end_ptr);
          ptr = strtok_r(NULL,"/",&end_str);
          counter+=1;
        }
        infos->service_time = serviceArray;

    }
    else if( !strcmp(strings[0], "SENSOR") ){

        infos->lp_type = SENSOR;

        if( !strcmp(strings[1], "BATCH") )
            infos->type_job = BATCH;
        else if( !strcmp(strings[1], "REAL_TIME") )
            infos->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY") )
            infos->type_job = LOSSY;
        else
            exit(EXIT_FAILURE);

        if( !strcmp(strings[2], "SENSOR_TYPE0") )
            infos->sensor_type = SENSOR_TYPE0;
        else if( !strcmp(strings[2], "SENSOR_TYPE1") )
            infos->sensor_type = SENSOR_TYPE1;
        else
            exit(EXIT_FAILURE);

        flag = 1;

    }
    else if( !strcmp(strings[0], "ACTUATOR") ){

        infos->lp_type = ACTUATOR;

        if( !strcmp(strings[1], "BATCH") )
            infos->type_job = BATCH;
        else if( !strcmp(strings[1], "REAL_TIME") )
            infos->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY") )
            infos->type_job = LOSSY;
        else
            exit(EXIT_FAILURE);
        if( !strcmp(strings[2], "ACTUATOR_TYPE0") )
            infos->actuator_type = ACTUATOR_TYPE0;
        else if( !strcmp(strings[2], "ACTUATOR_TYPE1") )
            infos->actuator_type = ACTUATOR_TYPE1;
        else
            exit(EXIT_FAILURE);

        flag = 1;

        double rateTransition = strtod(strings[4], &ptr);
        infos->rateTransition = rateTransition;

        double serviceTimeCommand = strtod(strings[5], &ptr);
        infos->serviceTimeCommand = serviceTimeCommand;

    }
    else if( !strcmp(strings[0], "WAN") ){

        infos->lp_type = WAN;

        if( !strcmp(strings[1], "WAN_TYPE0") )
            infos->wan_type = WAN_TYPE0;
        else if( !strcmp(strings[1], "WAN_TYPE1") )
            infos->wan_type = WAN_TYPE1;
        else
            exit(EXIT_FAILURE);

        double delay = strtod(strings[2], &ptr);
        infos->delay = delay;

    }
    else if( !strcmp(strings[0], "LAN") ){

        infos->lp_type = LAN;

        if( !strcmp(strings[1], "LAN_TYPE0") )
            infos->lan_type = LAN_TYPE0;
        else if( !strcmp(strings[1], "LAN_TYPE1") )
            infos->lan_type = LAN_TYPE1;
        else if( !strcmp(strings[1], "LAN_TYPE2") )
            infos->lan_type = LAN_TYPE2;
        else
            exit(EXIT_FAILURE);

        double delay = strtod(strings[2], &ptr);
        infos->delay = delay;

    }
    else{
        exit(EXIT_FAILURE);
    }

    if(flag){//Either sensor or actuator
      if( !strcmp(strings[3], "MEASURE0") ){
        infos->measure_type = MEASURE0;
      }
      else if( !strcmp(strings[3], "MEASURE1") )
          infos->measure_type = MEASURE1;
      else if( !strcmp(strings[3], "MEASURE2") )
          infos->measure_type = MEASURE2;
      else
        exit(EXIT_FAILURE);

      infos->id_LAN_up = upperNode;

    }

    return infos;

}
void getUpNode(topology * top, int index, int tot, int number, int ** array,int temp, int original){
  if(index != number){//not first run
    int temp2 = getUpperNode(top, number);
    number = temp2;
  }
  else{
    number = getUpperNode(top, index);
    array[tot][index] = index;//initialized for those who have actuators directly below
  }

  if(number == -1 ){ //someone higher then me so I can't reach
    array[tot][index] = -1;
  }
  else if(number == tot){//can't go higher than that, reached
    return;
  }
  else{
    array[tot][index] = number;
    temp = number;
    getUpNode(top,index,tot,number,array,temp, original);
  }
}

void getUpNode2(topology * top, int up, int index, int *** result){
  //index : starting actuator
  if(index != up){
    up = getUpperNode(top, up);
  }
  else{
    up = getUpperNode(top, index);
  }
  if(up == -1){ //someone higher then me so I can't reach
    return;
  }

  int type = getType(top, up);
  if(type == 0 || type == 3 || type == 4){//node or lan

    int typeSenAct = getType(top, index);
    int at;
    int * te;
    if(typeSenAct == 1){//sensor
      at = getSensorType(top,index);
      te = getSensType(top,up);
    }
    else{
      at = getActuatorType(top,index);
      te = getActType(top,up);
    }
    for(int js = 0; js < te[at]; js++){

      if(result[up][at][js] == index){
        js = te[at];
      }
      else if(result[up][at][js] == -1){
        result[up][at][js] = index;

        js = te[at];
      }
    }
  }

  getUpNode2(top,up,index,result);

}

void getUpNode3(topology * top, int up, int index, int ** array){
  if(index != up){
    up = getUpperNode(top, up);
  }
  else{
    up = getUpperNode(top, index);
  }
  if(up == -1){ //Can't go higher
    return;
  }
  int typeSenAct = getType(top, index);
  int at;
  if(typeSenAct == 1){//sensor
    at = getSensorType(top,index);
  }
  else{
    at = getActuatorType(top,index);
  }
  array[up][at] +=1 ;
  getUpNode3(top,up,index,array);
}

topology * getTopology(char * path){
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int i=0;

  fp = fopen(path, "r");

  if (fp == NULL)
      exit(EXIT_FAILURE);

  char * temp = NULL;
  //read first line, total number of elements
  read = getline(&temp, &len, fp);
  int totalNumberOfElements = atoi(temp);

  int * arrayNumberLowerElements = malloc(sizeof(int) * totalNumberOfElements);
  for(int arrCount = 0; arrCount < totalNumberOfElements; arrCount+=1){
    arrayNumberLowerElements[arrCount] = 0;
  }


  int nn = 0;
  //number of sensors
  int ns = 0;
  //number of actuators
  int na = 0;
  //number of WANs
  int nw = 0;
  //number of LANs
  int nl = 0;
  //2n line, number of types of actuators
  read = getline(&temp, &len, fp);
  int nt = atoi(temp);

  //3rd line, number of types of sensors
  read = getline(&temp, &len, fp);
  int nts = atoi(temp);

  //4th line, number of types of LANs
  read = getline(&temp, &len, fp);
  int ntl = atoi(temp);

  //5th line: sensor type 0 transition rate,sensor type 0 telemetry rate;sensor type 1 tr. rate, ...
  read = getline(&temp, &len, fp);

  //loop through all the other tokens in the line
  char * end_ptr;
  char *end_str;
  char * end_token;
  char *ptr2;
  char * ptr = strtok_r(temp, ";", &end_str);
  double ** sensor_rates = malloc(sizeof(double*) * 2); //fixed, two types of messages
  int index = 0;
  int counter = 0;
  while(ptr){
    ptr2 = strtok_r(ptr,",",&end_token);
    //tokenize each ";" token through ",", iterate until end of the line
    counter = 0;//used to keep track of how many "," we iterated on

    while(ptr2){
      if(counter == 0){
        sensor_rates[index] = malloc(sizeof(double) * nts);
        sensor_rates[index][counter] = strtod(ptr2, &end_ptr);
      }
      else{
        sensor_rates[index][counter] = strtod(ptr2, &end_ptr);
      }
      counter+=1;
      ptr2 = strtok_r(NULL,",",&end_token);
    }
    index+=1;
    ptr=strtok_r(NULL, ";",&end_str);
  }

  read = getline(&temp, &len, fp);
  //6th line, LANs IN service times
  index = 0;
  ptr = strtok_r(temp, ";", &end_str);
  double ** LANsINserviceTimes = malloc(sizeof(double*) * ntl);
  while(ptr){
    ptr2 = strtok_r(ptr,",",&end_token);
    //tokenize each ";" token through ",", iterate until end of the line
    counter = 0;//used to keep track of how many "," we iterated on

    while(ptr2){
      if(counter == 0){
        LANsINserviceTimes[index] = malloc(sizeof(double) * 5);//fixed, 5 types messages
        LANsINserviceTimes[index][counter] = strtod(ptr2, &end_ptr);
      }
      else{
        LANsINserviceTimes[index][counter] = strtod(ptr2, &end_ptr);
      }
      counter+=1;
      ptr2 = strtok_r(NULL,",",&end_token);
    }
    index+=1;
    ptr=strtok_r(NULL, ";",&end_str);
  }
  read = getline(&temp, &len, fp);
  //7th line, LANs OUT service times
  index = 0;
  ptr = strtok_r(temp, ";", &end_str);
  double ** LANsOUTserviceTimes = malloc(sizeof(double*) * ntl);
  while(ptr){
    ptr2 = strtok_r(ptr,",",&end_token);
    //tokenize each ";" token through ",", iterate until end of the line
    counter = 0;//used to keep track of how many "," we iterated on

    while(ptr2){
      if(counter == 0){
        LANsOUTserviceTimes[index] = malloc(sizeof(double) * 5);//fixed, 5 types messages
        LANsOUTserviceTimes[index][counter] = strtod(ptr2, &end_ptr);
      }
      else{
        LANsOUTserviceTimes[index][counter] = strtod(ptr2, &end_ptr);
      }
      counter+=1;
      ptr2 = strtok_r(NULL,",",&end_token);
    }
    index+=1;
    ptr=strtok_r(NULL, ";",&end_str);
  }
  //8th line: probabilities command receiver
  read = getline(&temp, &len, fp);

  ptr = strtok_r(temp, ";", &end_str);
  double * probArray = malloc((sizeof(double)) * nt); //fixed, 4 type of data.
  counter = 0;
  while(ptr){
    probArray[counter] = strtod(ptr, &end_ptr);
    ptr = strtok_r(NULL,";",&end_str);
    counter+=1;
  }
  //9th line: probabilities generating command
  read = getline(&temp, &len, fp);

  ptr = strtok_r(temp, ";", &end_str);
  double * probCommandSendArray = malloc((sizeof(double)) * 3); //fixed, 3 types of nodes
  counter = 0;
  while(ptr){
    probCommandSendArray[counter] = strtod(ptr, &end_ptr);
    counter+=1;
    ptr = strtok_r(NULL,";",&end_str);
  }

  topology * genTop = malloc(sizeof(topology));
  topArray ** returnArray = malloc(sizeof(topArray *) * (totalNumberOfElements));
  //9th and onward
  while ((read = getline(&line, &len, fp)) != -1) {
    topArray * tp = malloc(sizeof(topArray));

    //tokenize the line using ";" as a separator
    char *end_str;
    ptr = strtok_r(line, ";", &end_str);
    //first element is the sender
    int temp = atoi(ptr);

    int numberOfInfos = 0;
    index = 0;//keep track of how many ";" token we iterated on so we know
    //which kind data we are analyzing

    int upperNode = 0;
    char ** infoArray = NULL;

    ptr = strtok_r(NULL, ";", &end_str);
    //loop through all the other tokens in the line
    while(ptr){
      char * end_token;
      char *ptr2 = strtok_r(ptr,",",&end_token);
      //tokenize each ";" token through ",", iterate until end of the line
      counter = 0;//used to keep track of how many "," we iterated on

      while(ptr2){
        if(index == 0){//sender
          upperNode = atoi(ptr2);
          if(upperNode != -1){
            arrayNumberLowerElements[upperNode]+=1;
          }
          //printf("Node %d has %d informations.\n",temp,numberOfInfos);
        }
        else if(index == 1){//#info
          numberOfInfos = atoi(ptr2);
          infoArray = malloc(sizeof(char *)*numberOfInfos);
        }
        else{//infos
          char * tempString = strdup(ptr2);
          //remove endline char from the info
          if(counter == 0){
            if(!strcmp(tempString,"NODE")){
              nn+=1;
            }
            else if (!strcmp(tempString,"SENSOR")){
              ns+=1;
            }
            else if (!strcmp(tempString,"ACTUATOR")){
              na+=1;
            }
            else if (!strcmp(tempString, "WAN")){
              nw+=1;
            }
            else if (!strcmp(tempString, "LAN")){
              nl+=1;
            }
          }
          else if(counter == numberOfInfos - 1){
            tempString[strlen(tempString)-1] = 0;
          }
          infoArray[counter] = tempString;
        }
        counter+=1;
        ptr2 = strtok_r(NULL,",",&end_token);
      }
      //finished one ";" token
      index+=1;
      ptr=strtok_r(NULL, ";",&end_str);
    }

    //tp->numberOfLowerElements = numberOfLowerElements;
    //tp->lowerElements = lowerElementsArray;
    tp->upperNode = upperNode;
    //tp->numberOfLANS = numberOfLANS;
    //tp->connectedLans = LANSarray;

    tp->info = parse_strings(infoArray, nt, upperNode);

		for(i=0;i<counter;i++){
				free(infoArray[i]);
		}
		free(infoArray);
    returnArray[temp] = tp;

  }

  fclose(fp);
  genTop->total_nodes = nn;
  genTop->sensor_nodes = ns;
  genTop->actuator_nodes = na;
  genTop->numberOfTotalLANs = nl;
  genTop->numberOfTotalWANs = nw;
  genTop->numberOfActTypes = nt;
  genTop->numberOfSensTypes = nts;
  genTop->numberOfLANsTypes = ntl;
  genTop->sensorRatesByType = sensor_rates;
  genTop->LANsINserviceTimes = LANsINserviceTimes;
  genTop->LANsOUTserviceTimes = LANsOUTserviceTimes;
  genTop->probOfActuators = probArray;
  genTop->probNodeCommandArray = probCommandSendArray;
  genTop->topArr = returnArray;



  int ** typesSensArray = malloc(sizeof(int*) * totalNumberOfElements);
  int ** typesActArray = malloc(sizeof(int*) * totalNumberOfElements);
  index = 0;
  int index2 = 0;
  int index3 = 0;
  int type = 0;
  while(index < totalNumberOfElements){
    typesSensArray[index] = malloc(sizeof(int) * nts);
    typesActArray[index] = malloc(sizeof(int) * nt);
    while(index2 < nts){
      typesSensArray[index][index2] = 0;
      index2+=1;
    }
    index2 = 0;
    while(index3 < nt){
      typesActArray[index][index3] = 0;
      index3+=1;
    }
    index3 = 0;
    index+=1;
  }
  index = 0;
  while(index < totalNumberOfElements){
    type = getType(genTop,index);
    if(type == 1){
      getUpNode3(genTop,index,index,typesSensArray);
    }
    else if (type == 2){
      getUpNode3(genTop,index,index,typesActArray);
    }
    index+=1;
  }

  index = 0;
  while(index < totalNumberOfElements){
    setSensorTypes(genTop,typesSensArray[index],index, nts);
    setActuatorTypes(genTop,typesActArray[index],index, nt);
    index+=1;
  }

  /*
  index = 0;
  while(index < totalNumberOfElements){
    int * actTy = getActType(genTop, index);
    int * senTy = getSensType(genTop, index);
    index2 = 0;
    index3 = 0;
    printf("\n Node %d has ACTUATORS:",index);
    while(index2 < nt){
      printf("%d/",actTy[index2]);
      index2+=1;
    }
    printf(" SENSORS:");
    while(index3 < nts){
      printf("%d/",senTy[index3]);
      index3+=1;
    }
    index+=1;
  }
*/
  int *** resultAct = malloc(sizeof(int**)*totalNumberOfElements);
  int *** resultSens = malloc(sizeof(int**)*totalNumberOfElements);
  index = 0;
  index2 = 0;

  while(index < totalNumberOfElements){
    int type =  getType(genTop, index);
    if(type == 0  || type == 3 || type == 4){//node or lan
      resultAct[index] = malloc(sizeof(int*)*nt);
      resultSens[index] = malloc(sizeof(int*)*nts);
      int * teA = getActType(genTop, index);
      index2 = 0;
      while(index2 < nt){
        if(teA[index2] == 0){
          resultAct[index][index2] = malloc(sizeof(int));
          resultAct[index][index2][0] = -1;
        }
        resultAct[index][index2] = malloc(sizeof(int) * teA[index2]);
        for(int js = 0; js < teA[index2];js+=1){
          resultAct[index][index2][js] = -1;
        }
        index2+=1;
      }
      int * teS = getSensType(genTop, index);
      index2 = 0;
      while(index2 < nts){
        if(teS[index2] == 0){
          resultSens[index][index2] = malloc(sizeof(int));
          resultSens[index][index2][0] = -1;
        }
        else{
          resultSens[index][index2] = malloc(sizeof(int) * teS[index2]);
          for(int js = 0; js < teS[index2];js+=1){
            resultSens[index][index2][js] = -1;
          }
        }
        index2+=1;
      }
    }
    index+=1;
  }


  int ** arrayActuatorPaths = malloc(sizeof(int *) * (totalNumberOfElements));
  int tot = 0;
  int ind;
  while(tot < totalNumberOfElements){
    arrayActuatorPaths[tot] = malloc(sizeof(int)*(totalNumberOfElements));
    for(ind = 0; ind < totalNumberOfElements; ind+=1){
      int type =  getType(genTop, ind);
      if(type == 2){//Is an actuator
        getUpNode(genTop, ind, tot, ind, arrayActuatorPaths,-1, ind);
      }
      else{
        arrayActuatorPaths[tot][ind] = -1;
      }
    }
    tot+=1;
  }


  for(ind = 0; ind < totalNumberOfElements; ind+=1){
    int type =  getType(genTop, ind);
    if(type == 2){//Is an actuator
      getUpNode2(genTop, ind, ind, resultAct);
      }
    else if(type == 1){
      getUpNode2(genTop, ind, ind, resultSens);
      }
    }
/*
    index = 0;
    index2 = 0;
    while(index < totalNumberOfElements){
      int type =  getType(genTop, index);
      if(type == 0 || type == 3 || type == 4){//node or lan
      int * te = getActType(genTop, index);
      index2 = 0;
      while(index2 < nt){
        for(int js = 0; js < te[index2];js+=1){
          printf("resultAct[%d][%d][%d] = %d\n", index, index2, js, resultAct[index][index2][js]);
        }
        index2+=1;
      }
      int * teS = getSensType(genTop, index);
      index2 = 0;
      while(index2 < nts){
        for(int js = 0; js < teS[index2];js+=1){
          printf("resultSens[%d][%d][%d] = %d\n", index, index2, js, resultSens[index][index2][js]);
        }
        index2+=1;
        }
      }
      index+=1;

    }


    tot = 0;
    while(tot < totalNumberOfElements){
      for(ind = nn; ind < totalNumberOfElements; ind+=1){
        if(arrayActuatorPaths[tot][ind] != -1){
          printf("array[%d][%d] = %d\n",tot, ind,arrayActuatorPaths[tot][ind]);
        }
      }
      tot+=1;
    }
    printf("array[%d][%d] = %d\n",17, 15,arrayActuatorPaths[17][15]);

*/
  int * numberofLANs = malloc(sizeof(int)*totalNumberOfElements);
  for(int c = 0; c < totalNumberOfElements; c+=1){
    numberofLANs[c] = 0;
  }

  int ** lowerElementsArray = malloc(sizeof(int*)*totalNumberOfElements);
  for(int c = 0; c < totalNumberOfElements; c+=1){
    if(arrayNumberLowerElements[c] != 0){
      lowerElementsArray[c] = malloc(sizeof(int)*arrayNumberLowerElements[c]);
      for(int c2 = 0; c2 < arrayNumberLowerElements[c];c2+=1){
        lowerElementsArray[c][c2] = -1;
      }
    }
    else{
      lowerElementsArray[c] = malloc(sizeof(int));
      lowerElementsArray[c][0] = -1;
    }
  }

  for(int c = 0; c < totalNumberOfElements; c+=1){
    int uppNode = getUpperNode(genTop,c);
		if(uppNode!=-1){
			for(int c2 = 0; c2 < arrayNumberLowerElements[uppNode];c2+=1){
				if(lowerElementsArray[uppNode][c2] == -1){
					lowerElementsArray[uppNode][c2] = c;
					//printf("array[%d][%d] = %d.\n",uppNode,c2,lowerElementsArray[uppNode][c2]);
					c2 = arrayNumberLowerElements[uppNode];
					int typeC = getType(genTop,c);
					if(typeC == 4){ //LAN
						numberofLANs[uppNode]+=1;
					}
				}
			}
		}
  }
  int ** LANSArray = malloc(sizeof(int*) * totalNumberOfElements);
  for(int c = 0; c < totalNumberOfElements; c+=1){
    if(numberofLANs[c] == 0){
      LANSArray[c] = malloc(sizeof(int));
      LANSArray[c][0] = -1;
    }
    else{
      LANSArray[c] = malloc(sizeof(int)*numberofLANs[c]);
      for(int c2 = 0; c2 < numberofLANs[c];c2+=1){
        LANSArray[c][c2] = -1;
      }
    }
  }

  int * WANsDownArray = malloc(sizeof(int) * totalNumberOfElements);
  for(int c = 0; c < totalNumberOfElements; c+=1){
    WANsDownArray[c] = -1;
  }

  for(int c = 0; c < totalNumberOfElements; c+=1){
    for(int c2 = 0; c2 < arrayNumberLowerElements[c];c2+=1){
      int low = lowerElementsArray[c][c2];
      int typeLow= getType(genTop,low);
      if(typeLow == 4){ //LAN
        for(int c3 = 0; c3 < numberofLANs[c];c3+=1){
          if(LANSArray[c][c3] == -1){
            LANSArray[c][c3] = low;
         }
       }
      }
      if(typeLow == 3){ //LAN
          WANsDownArray[c] = low;
      }
    }
  }


  for(int c = 0; c < totalNumberOfElements; c+=1){
    setLowerElements(genTop, lowerElementsArray[c],arrayNumberLowerElements[c],c);
    setLANs(genTop, LANSArray[c],numberofLANs[c],c);
    setWANdown(genTop, WANsDownArray[c],c);
    setWANup(genTop,c);
  }

  genTop->actuatorPaths = arrayActuatorPaths;
  genTop->ListActuatorsByType = resultAct;
  genTop->ListSensorsByType = resultSens;

  return genTop;
}
