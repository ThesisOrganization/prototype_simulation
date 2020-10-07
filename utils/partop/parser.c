#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>
#include "../application_datatypes.h"

void * parse_strings(char ** strings, int types){
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

        int WAN_up = atoi(strings[3]);
        infos->id_WAN_up = WAN_up;

        int WAN_down = atoi(strings[4]);
        infos->id_WAN_down = WAN_down;

        int aggregation_rate = atoi(strings[5]);
        infos->aggregation_rate = aggregation_rate;

        float delayUP = strtod(strings[6],&ptr);
        infos->delay_upper_router = delayUP;
        float delayDOWN = atof(strings[7]);
        infos->delay_lower_router = delayDOWN;

        char * end_str;
        char * end_ptr;
        char * ptr = strtok_r(strings[8], "/", &end_str);
        double * serviceArray = malloc((sizeof(double)) * 5); //fixed, 5 type of data.
        int counter = 0;
        while(ptr){
          serviceArray[counter] = strtod(ptr, &end_ptr);
          ptr = strtok_r(NULL,"/",&end_str);
          counter+=1;
        }
        infos->service_time = serviceArray;

        ptr = strtok_r(strings[9], "/", &end_str);
        int * typesArray = malloc((sizeof(int)) * types);
        counter = 0;
        while(ptr){
          typesArray[counter] = atoi(ptr);
          ptr = strtok_r(NULL,"/",&end_str);
          counter+=1;
        }
        infos->actuatorsTypesBelow = typesArray;

        int sensors = atoi(strings[10]);
        infos->numberOfBelowSensors = sensors;
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

        double rateTransition = strtod(strings[5], &ptr);
        infos->rateTransition = rateTransition;

        double serviceTimeCommand = strtod(strings[6], &ptr);
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

        char * end_str;
        ptr = strtok_r(strings[3], "/", &end_str);
        int * typesArray = malloc((sizeof(int)) * types);
        int counter = 0;
        while(ptr){
          typesArray[counter] = atoi(ptr);
          ptr = strtok_r(NULL,"/",&end_str);
          counter+=1;
        }
        infos->actuatorsTypesBelow = typesArray;

    }
    else{
        exit(EXIT_FAILURE);
    }

    if(flag){//Either sensor or actuator
      if( !strcmp(strings[3], "MEASURE0") )
          infos->measure_type = MEASURE0;
      else if( !strcmp(strings[3], "MEASURE1") )
          infos->measure_type = MEASURE1;
      else if( !strcmp(strings[3], "MEASURE2") )
          infos->measure_type = MEASURE2;
      else
        exit(EXIT_FAILURE);

      int id_LAN_up = atoi(strings[4]);
      infos->id_LAN_up = id_LAN_up;
    }

    return infos;

}
void getUpNode(topology * top, int index, int tot, int number, int ** array,int temp, int *** result, int original){
  if(index != number){//not first run
    int temp2 = getUpperNode(top, number);
    number = temp2;
  }
  else{
    number = getUpperNode(top, index);
    array[tot][index] = index;//initialized for those who have actuators directly below
  }

  if(number < tot ){ //someone higher then me so I can't reach
    array[tot][index] = -1;
  }
  else if(number == tot){//can't go higher than that, reached
    return;
  }
  else{
    array[tot][index] = number;
    temp = number;
    getUpNode(top,index,tot,number,array,temp, result, original);
  }
}

void getUpNode2(topology * top, int up, int index, int *** result){
  int flag = 0;
  if(index != up){
    up = getUpperNode(top, up);
  }
  else{
    up = getUpperNode(top, index);
  }
  int type =  getType(top, up);
  if(type == 1 || type == 4){//node or lan
    flag = 1;
  }
  if(flag){
    int at = getActuatorType(top,index);
    int * te = getActType(top,up);
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
  if(up == 0 ){ //someone higher then me so I can't reach
    return;
  }
  else{
    getUpNode2(top,up,index,result);
  }
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
  //read first line, assess number of nodes, save
  read = getline(&temp, &len, fp);
  int nn = atoi(temp);
  //same for 2nd line, number of sensors
  read = getline(&temp, &len, fp);
  int ns = atoi(temp);
  //same for 2nd line, number of actuators
  read = getline(&temp, &len, fp);
  int na = atoi(temp);
  //4th line, number of WANs
  read = getline(&temp, &len, fp);
  int nw = atoi(temp);
  //5th line, number of LANs
  read = getline(&temp, &len, fp);
  int nl = atoi(temp);
  //6th line, number of types of actuators
  read = getline(&temp, &len, fp);
  int nt = atoi(temp);
  //7th line, number of types of sensors
  read = getline(&temp, &len, fp);
  int nts = atoi(temp);

  //8th line, number of types of LANs
  read = getline(&temp, &len, fp);
  int ntl = atoi(temp);

  //9th line: sensor type 0 transition rate,sensor type 0 telemetry rate;sensor type 1 tr. rate, ...
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
  //10th line, LANs service times
  index = 0;
  ptr = strtok_r(temp, ";", &end_str);
  double ** LANserviceTimes = malloc(sizeof(double*) * ntl);
  while(ptr){
    ptr2 = strtok_r(ptr,",",&end_token);
    //tokenize each ";" token through ",", iterate until end of the line
    counter = 0;//used to keep track of how many "," we iterated on

    while(ptr2){
      if(counter == 0){
        LANserviceTimes[index] = malloc(sizeof(double) * 5);//fixed, 5 types messages
        LANserviceTimes[index][counter] = strtod(ptr2, &end_ptr);
      }
      else{
        LANserviceTimes[index][counter] = strtod(ptr2, &end_ptr);
      }
      counter+=1;
      ptr2 = strtok_r(NULL,",",&end_token);
    }
    index+=1;
    ptr=strtok_r(NULL, ";",&end_str);
  }

  //11th line: probabilities command receiver
  read = getline(&temp, &len, fp);

  ptr = strtok_r(temp, ";", &end_str);
  double * probArray = malloc((sizeof(double)) * nt); //fixed, 4 type of data.
  counter = 0;
  while(ptr){
    probArray[counter] = strtod(ptr, &end_ptr);
    ptr = strtok_r(NULL,"/",&end_str);
    counter+=1;
  }
  //printf("There are %d nodes.\n",nn);
  //printf("There are %d sensor/actuator nodes.\n",ns);

  topology * genTop = malloc(sizeof(topology));
  topArray ** returnArray = malloc(sizeof(topArray *) * (nn + ns + na + nw + nl));
  //iterate through the remaining lines, having the following syntax:

  while ((read = getline(&line, &len, fp)) != -1) {
    topArray * tp = malloc(sizeof(topArray));

    //tokenize the line using ";" as a separator
    char *end_str;
    ptr = strtok_r(line, ";", &end_str);
    //first element is the sender
    int temp = atoi(ptr);

    int numberOfLowerElements = 0;
    int numberOfLANS = 0;
    int numberOfInfos = 0;
    index = 0;//keep track of how many ";" token we iterated on so we know
    //which kind data we are analyzing

    int * lowerElementsArray = NULL;
    int upperNode = 0;
    char ** infoArray = NULL;
    int * LANSarray = NULL;

    ptr = strtok_r(NULL, ";", &end_str);
    //loop through all the other tokens in the line

    while(ptr){
      char * end_token;
      char *ptr2 = strtok_r(ptr,",",&end_token);
      //tokenize each ";" token through ",", iterate until end of the line
      counter = 0;//used to keep track of how many "," we iterated on

      while(ptr2){
        if(index == 0){//#receivers
          numberOfLowerElements = atoi(ptr2);
          lowerElementsArray = malloc(sizeof(int *)*numberOfLowerElements);
        }
        else if(index == 1){//receivers
          lowerElementsArray[counter] = atoi(ptr2);
        }
        else if(index == 2){//sender
          upperNode = atoi(ptr2);
          //printf("Node %d has %d informations.\n",temp,numberOfInfos);
        }
        else if(index == 3){//#lans
          numberOfLANS = atoi(ptr2);
          LANSarray = malloc(sizeof(int *)*numberOfLANS);

        }
        else if(index == 4){//LANS
          LANSarray[counter] = atoi(ptr2);
        }
        else if(index == 5){//#info
          numberOfInfos = atoi(ptr2);
          infoArray = malloc(sizeof(char *)*numberOfInfos);
        }
        else{//#info
          char * tempString = strdup(ptr2);
          //remove endline char from the info
          if(counter == numberOfInfos - 1){
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

    tp->numberOfLowerElements = numberOfLowerElements;
    tp->lowerElements = lowerElementsArray;
    tp->upperNode = upperNode;
    tp->numberOfLANS = numberOfLANS;
    tp->connectedLans = LANSarray;

    tp->info = parse_strings(infoArray, nt);


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
  genTop->LANServiceTimesByType = LANserviceTimes;
  genTop->probOfActuators = probArray;
  genTop->topArr = returnArray;

  int totalNumberOfElements =  nn+ns+na+nl+nw;

  int *** result = malloc(sizeof(int**)*totalNumberOfElements);
  index = 0;
  int index2 = 0;

  while(index < totalNumberOfElements){
    int type =  getType(genTop, index);
    if(type == 0 || type == 4){//node or lan

      result[index] = malloc(sizeof(int*)*nt);
      int * te = getActType(genTop, index);
      index2 = 0;
      while(index2 < nt){
        result[index][index2] = malloc(sizeof(int) * te[index2]);
        for(int js = 0; js < te[index2];js+=1){
          result[index][index2][js] = -1;
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
        getUpNode(genTop, ind, tot, ind, arrayActuatorPaths,-1, result, ind);
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
      getUpNode2(genTop, ind, ind, result);
      }
    }
    /*
    index = 0;
    index2 = 0;
    while(index < nn){
      int * te = getActType(genTop, index);
      index2 = 0;
      while(index2 < nt){
        for(int js = 0; js < te[index2];js+=1){
          printf("%d %d %d %d\n", index, index2, js, result[index][index2][js]);
        }
        index2+=1;
      }
      index+=1;
    }
    */




    tot = 0;
    while(tot < totalNumberOfElements){
      for(ind = nn; ind < totalNumberOfElements; ind+=1){
        if(arrayActuatorPaths[tot][ind] != -1){
          //printf("array[%d][%d] = %d\n",tot, ind,arrayActuatorPaths[tot][ind]);
        }
      }
      tot+=1;
    }
  genTop->actuatorPaths = arrayActuatorPaths;
  genTop->uglyStructContainingMatrixNodesActuatorTypesAndWhichOneAreThem = result;

  return genTop;
}
