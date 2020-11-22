#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <string.h>
#include "../application_datatypes.h"
#include "../idmap/idmap.h"

void parse_strings(char ** strings,Element_topology * this_lpt, int upperNode){
    char * ptr;
    int counter = 0;
    if( !strcmp(strings[0], "NODE") ){

        node_topology * nodeTop = malloc(sizeof(node_topology));

        this_lpt->lp_type = NODE;
        char * end_str;
        char * end_ptr;

        nodeTop->scheduler = atoi(strings[1]);
        if( !strcmp(strings[2], "CENTRAL") ){
            nodeTop->node_type = CENTRAL;

            nodeTop->disk_type = atoi(strings[8]);

            char * ptr = strtok_r(strings[9], "/", &end_str);
            double * diskServiceArray = malloc((sizeof(double)) * 4); //fixed, 4 type of data.
            counter = 0;
            while(ptr){
              diskServiceArray[counter] = strtod(ptr, &end_ptr);
              ptr = strtok_r(NULL,"/",&end_str);
              counter+=1;
            }
            nodeTop->diskServices = diskServiceArray;

        }

        else if( !strcmp(strings[2], "REGIONAL") )
            nodeTop->node_type = REGIONAL;
        else if( !strcmp(strings[2], "LOCAL") )
            nodeTop->node_type = LOCAL;
        else
            exit(EXIT_FAILURE);

        ptr = strtok_r(strings[3], "/", &end_str);
        int * aggregation_rates = malloc(sizeof(int) * 4);
        counter = 0;
        while(ptr){
          aggregation_rates[counter] = strtod(ptr, &end_ptr);
          ptr = strtok_r(NULL,"/",&end_str);
          counter+=1;
        }

        nodeTop->aggregation_rate = aggregation_rates;
        float delayUP = strtod(strings[4],&ptr);
        nodeTop->delay_upper_router = delayUP;

        float delayDOWN = atof(strings[5]);
        nodeTop->delay_lower_router = delayDOWN;

        ptr = strtok_r(strings[6], "/", &end_str);
        double * serviceArray = malloc((sizeof(double)) * 5); //fixed, 5 type of data.
        counter = 0;
        while(ptr){
          serviceArray[counter] = strtod(ptr, &end_ptr);
          ptr = strtok_r(NULL,"/",&end_str);
          counter+=1;
        }
        nodeTop->service_time = serviceArray;

        float probCommand = strtod(strings[7],&ptr);
        nodeTop->probCommandResponse = probCommand;
        this_lpt->spec_top.node = nodeTop;
    }
    else if( !strcmp(strings[0], "SENSOR") ){
        //specific_topology * specTop = malloc(sizeof(sensor_topology));
        sensor_topology * sensorTop = malloc(sizeof(sensor_topology));

        this_lpt->lp_type = SENSOR;

        if( !strcmp(strings[1], "BATCH") )
            sensorTop->type_job = BATCH;
        else if( !strcmp(strings[1], "REAL_TIME") )
            sensorTop->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY") )
            sensorTop->type_job = LOSSY;
        else
            exit(EXIT_FAILURE);

        sensorTop->sensor_type = atoi(strings[2]);
        sensorTop->measure_type = atoi(strings[3]);

        sensorTop->id_LAN_up = upperNode;
        this_lpt->spec_top.sensor = sensorTop;

    }
    else if( !strcmp(strings[0], "ACTUATOR") ){
        actuator_topology * actuatorTop = malloc(sizeof(actuator_topology));

        this_lpt->lp_type = ACTUATOR;

        if( !strcmp(strings[1], "BATCH") )
            actuatorTop->type_job = BATCH;
        else if( !strcmp(strings[1], "REAL_TIME") )
            actuatorTop->type_job = REAL_TIME;
        else if( !strcmp(strings[1], "LOSSY") )
            actuatorTop->type_job = LOSSY;
        else
            exit(EXIT_FAILURE);

        actuatorTop->actuator_type = atoi(strings[2]);
        actuatorTop->measure_type = atoi(strings[3]);

        actuatorTop->id_LAN_up = upperNode;

        double rateTransition = strtod(strings[4], &ptr);
        actuatorTop->rateTransition = rateTransition;

        double serviceTimeCommand = strtod(strings[5], &ptr);
        actuatorTop->serviceTimeCommand = serviceTimeCommand;
        this_lpt->spec_top.actuator = actuatorTop;

    }
    else if( !strcmp(strings[0], "WAN") ){
        wan_topology * actuatorWan = malloc(sizeof(wan_topology));

        this_lpt->lp_type = WAN;

        actuatorWan->wan_type = atoi(strings[1]);

        double delay = strtod(strings[2], &ptr);
        actuatorWan->delay = delay;

        this_lpt->spec_top.wan = actuatorWan;

    }
    else if( !strcmp(strings[0], "LAN") ){
        lan_topology * actuatorLan = malloc(sizeof(lan_topology));

        this_lpt->lp_type = LAN;

        actuatorLan->lan_type = atoi(strings[1]);
        double delay = strtod(strings[2], &ptr);
        actuatorLan->delay = delay;
        this_lpt->spec_top.lan = actuatorLan;

    }
    else{
        exit(EXIT_FAILURE);
    }

}

void upwardSearchActSensType(Element_topology ** lpt, int up, int index, int ** array){

  if(index != up){
    up = getUpperNode(lpt[up]);
  }
  else{
    up =  getUpperNode(lpt[index]);
  }
  if(up == -1){ //Can't go higher
    return;
  }
  int typeSenAct = getType(lpt[index]);
  int at;
  fflush(stdout);
  if(typeSenAct == 1){//sensor

    at = getSensorType(lpt[index]);
  }
  else{
    at = getActuatorType(lpt[index]);
  }

  array[up][at] +=1 ;

  upwardSearchActSensType(lpt,up,index,array);
}

//recursive call where we travel upward until the central node
void upwardSearchSubtreeFilling(Element_topology ** lpt, int up, int index, int *** result){
  //index : starting actuator
  if(index != up){
    up = getUpperNode(lpt[up]);
  }
  else{
    up = getUpperNode(lpt[index]);
  }

  if(up == -1){ //central node reached, processed in the last recursive call. Return.
    return;
  }
  //check upper node type
  int type = getType(lpt[up]);
  if(type == 0 || type == 3 || type == 4){//node,lan, wan
    //check if index refers to a sensor or actuator
    int typeSenAct = getType(lpt[index]);
    int at;
    int * te;
    //based on typeSenAct, initialize values, could also be done at first call
    if(typeSenAct == 1){//sensor
      at = getSensorType(lpt[index]);
      te = getSensType(lpt[up]);
    }
    else{//actuator
      at = getActuatorType(lpt[index]);
      te = getActType(lpt[up]);
    }
    //te[at] gives us how many actuators of type at the node up has, we cycle them
    for(int js = 0; js < te[at]; js++){
      //So there are three cases, we meet ourself, thus we need to skip
      //We meet another node, simply we go on with the cycle
      //We meet an empty space(-1) and fill it with ourself.
      if(result[up][at][js] == index){//already inserted, skip
        js = te[at];
      }
      else if(result[up][at][js] == -1){//empty space, fill it and skip the rest of the cycle
        result[up][at][js] = index;
        js = te[at];
      }
    }
  }

  upwardSearchSubtreeFilling(lpt,up,index,result);
}

//search the graph upward, trying to reach the destination node from the index node
void upwardSearchActuatorPaths(Element_topology ** lpt, int index, int destination, int number, int ** array){
  if(index != number){//not first run
    number = getUpperNode(lpt[number]);
  }
  else{
    number = getUpperNode(lpt[index]);
    array[destination][index] = index;//initialized for those who have actuators directly below
  }

  if(number == -1 ){ //reached central node, processed it and didn't find a match, can't reach destination from index!
    array[destination][index] = -1;
  }
  else if(number == destination){//Reached the destination, last step of previous iteration contains the correct next step
    return;
  }
  else{
    array[destination][index] = number;
    upwardSearchActuatorPaths(lpt,index,destination,number,array);
  }
}

lp_topology * getLPtopoogy(char * path){
  FILE * fp;
  char * line = malloc(sizeof(char)*MAX_LINE_LEN);
  char * temp = malloc(sizeof(char)*MAX_LINE_LEN);
  size_t len = MAX_LINE_LEN;
  ssize_t read;
  char *ptr;

  fp = fopen(path, "r");

  if (fp == NULL)
      exit(EXIT_FAILURE);

  //read first line, total number of elements
  read = getline(&temp, &len, fp);
  int numberOfElements = atoi(temp);
  read = getline(&temp, &len, fp);
  int numberOfLPs = atoi(temp);
	free(temp);

  int ** result = malloc(sizeof(int*)*numberOfLPs);
  int * EleToLP = malloc(sizeof(int)*numberOfElements);
  int * amountsOfElementsInLP = malloc(sizeof(int)*numberOfLPs);
  while ((read = getline(&line, &len, fp)) != -1) {
    //tokenize the line using ";" as a separator
    char *end_str;
    ptr = strtok_r(line, ";", &end_str);
    //first element is the node we are analyzing
    int LP_id = atoi(ptr);
    ptr = strtok_r(NULL, ";", &end_str);
    int numElements = atoi(ptr);
    amountsOfElementsInLP[LP_id] = numElements;
    result[LP_id] = malloc(sizeof(int)*numElements);

    int counter = 0;//keep track of how many ";" token we iterated on

    ptr = strtok_r(NULL, ",", &end_str);
    //loop through all the other tokens in the line
    while(ptr){
      int element = atoi(ptr);
      result[LP_id][counter] = element;
      EleToLP[element] = LP_id;
      counter+=1;
      ptr = strtok_r(NULL,",",&end_str);
    }
  }
  free(line);
  lp_topology * LPreturn = malloc(sizeof(lp_topology));
  LPreturn->numLP=numberOfLPs;

  LPreturn->LPtoElementMapping = result;

  int * idArray = malloc(sizeof(int)*numberOfElements);
  for(int i = 0; i < numberOfElements;i++){
    idArray[i] = i;
  }

  int valid = 0;
  idmap* ElementToLP = create_idmap(idArray,EleToLP,numberOfElements,&valid);
  free(idArray);
  free(EleToLP);
  LPreturn->amountsOfElementsInLP = amountsOfElementsInLP;
  LPreturn->ElementToLPMapping = ElementToLP;
  LPreturn->numValid = valid;
	fclose(fp);
  return(LPreturn);

}

total_topology * getTopology(char * path, char * path1){
  FILE * fp;
  char * line = malloc(sizeof(char)*MAX_LINE_LEN);
	char * temp = malloc(sizeof(char)*MAX_LINE_LEN);
	size_t len = MAX_LINE_LEN;
  ssize_t read;
  char * end_ptr;
  char *end_str;
  char * end_token;
  char *ptr2;
  int i=0;

  //open topology txt
  fp = fopen(path, "r");

  if (fp == NULL)
      exit(EXIT_FAILURE);

  //read first line, total number of elements
  read = getline(&temp, &len, fp);
  int totalNumberOfElements = atoi(temp);
  total_topology * EST = malloc(sizeof(total_topology));

  Element_topology ** lpt = malloc(sizeof(Element_topology*) * totalNumberOfElements);

  int * arrayNumberLowerElements = malloc(sizeof(int) * totalNumberOfElements);
  for(int arrCount = 0; arrCount < totalNumberOfElements; arrCount+=1){
    arrayNumberLowerElements[arrCount] = 0;
  }
  //initialization, will be dinamically computed
  //number of nodes
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

  //5th line: sensor type 0 telemetry rate,sensor type 0 transition rate;sensor type 1 tel. rate, ...
  read = getline(&temp, &len, fp);

  //loop through all the other tokens in the line

  char * ptr = strtok_r(temp, ";", &end_str);
  double ** sensor_rates = malloc(sizeof(double*) * nts); //fixed, two types of messages

  int index = 0;
  int counter = 0;
  while(ptr){
    ptr2 = strtok_r(ptr,",",&end_token);
    //tokenize each ";" token through ",", iterate until end of the line
    counter = 0;//used to keep track of how many "," we iterated on

    while(ptr2){
      if(counter == 0){
        sensor_rates[index] = malloc(sizeof(double) * 2);
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

  //8th line: weight actuator types
  //cycle, keep the sum to find the probability
  read = getline(&temp, &len, fp);
  ptr = strtok_r(temp, ";", &end_str);
  double * weight = malloc((sizeof(double)) * nt); //fixed, 4 type of data.
  double sum = 0.0;
  counter = 0;
  while(ptr){
    weight[counter] = strtod(ptr, &end_ptr);
    sum+=weight[counter];
    ptr = strtok_r(NULL,";",&end_str);
    counter+=1;
  }
  free(temp);
  double * probArray = malloc((sizeof(double)*nt));
  for(int i = 0; i < nt; i++){
    probArray[i] = weight[i]/sum;
  }
  free(weight);

  general_topology * genTop = malloc(sizeof(general_topology));

  //9th and onward
  while ((read = getline(&line, &len, fp)) != -1) {
    //tokenize the line using ";" as a separator
    char *end_str;
    ptr = strtok_r(line, ";", &end_str);
    //first element is the node we are analyzing
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
        if(index == 0){//upper element
          upperNode = atoi(ptr2);
          if(upperNode != -1){
            arrayNumberLowerElements[upperNode]+=1;
          }
        }
        else if(index == 1){//#info
          numberOfInfos = atoi(ptr2);
          infoArray = malloc(sizeof(char *)*numberOfInfos);
        }
        else{//infos
					//we include the terminator character
					char * tempString = malloc(sizeof(char)*(strlen(ptr2)+1));
					strncpy(tempString,ptr2,strlen(ptr2)+1);
          if(counter == 0){
            //check which type of element, increase the counter for that type
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
            //remove endline char from the info
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

    lpt[temp] = malloc(sizeof(Element_topology));
    parse_strings(infoArray,lpt[temp], upperNode);
    lpt[temp]->upperNode = upperNode;
    lpt[temp]->id = temp;
    for(i=0;i<numberOfInfos && i<counter;i++){
				free(infoArray[i]);
		}
		free(infoArray);
  }
	free(line);
  fclose(fp);

  genTop->total_nodes = nn;
  genTop->sensor_nodes = ns;
  genTop->actuator_nodes = na;
  genTop->numberOfTotalLANs = nl;
  genTop->numberOfTotalWANs = nw;
  genTop->numberOfActTypes = nt;
  genTop->numberOfSensTypes = nts;
  genTop->numberOfLANsTypes = ntl;
  genTop->probOfActuators = probArray;

  EST->gn = genTop;

  int ** typesSensArray = malloc(sizeof(int*) * totalNumberOfElements);
  int ** typesActArray = malloc(sizeof(int*) * totalNumberOfElements);
  index = 0;
  int index2 = 0;
  int index3 = 0;
  int type = 0;

  //printf("??node %d,type %d\n",7,lpt[7]->spec_top.sensor->sensor_type);
  //printf("%d\n",lpt[7]->spec_top.sensor->sensor_type);
  //fflush(stdout);
  //initialization of array of sensor and actuator types
  while(index < totalNumberOfElements){
    typesSensArray[index] = malloc(sizeof(int) * nts);
    typesActArray[index] = malloc(sizeof(int) * nt);
    //printf("??node %d,type %d\n",7,lpt[7]->spec_top.sensor->sensor_type);
    //printf("%d\n",lpt[7]->spec_top.sensor->sensor_type);
    //fflush(stdout);
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
    type = getType(lpt[index]);
    if(type == 1){
      upwardSearchActSensType(lpt,index,index,typesSensArray);
    }
    else if (type == 2){
      upwardSearchActSensType(lpt,index,index,typesActArray);
    }
    index+=1;
  }

  //insert info from the search, this is done before the other at the end
  //because these info are needed for the other two searches
  index = 0;
  while(index < totalNumberOfElements){
    type = getType(lpt[index]);
    //printf("index %d, type %d",index, type);
    if(type == 0 || type == 3 || type == 4){
      setSensorTypes(lpt[index],typesSensArray[index], nts, sizeof(int)*nts);
      setActuatorTypes(lpt[index],typesActArray[index], nt, sizeof(int)*nt);
    }
    index+=1;
  }

  for( int i = 0; i < totalNumberOfElements; i++){
    free(typesSensArray[i]);
    free(typesActArray[i]);
  }
  free(typesSensArray);
  free(typesActArray);

  int *** resultAct = malloc(sizeof(int**)*totalNumberOfElements);
  int *** resultSens = malloc(sizeof(int**)*totalNumberOfElements);

  index = 0;
  //initialization
  //resultAct is a matrix where [x][y][z] is the z-th actuator of type y in the sub-tree of the node x.
  //for each element in the system, we check its type. If it is a LAN, WAN or NODE
  //we allocate an array.
  while(index < totalNumberOfElements){
    int type =  getType(lpt[index]);
    if(type == 0  || type == 3 || type == 4){//node, lan or wan
      resultAct[index] = malloc(sizeof(int*)*nt);
      resultSens[index] = malloc(sizeof(int*)*nts);
      //Retrieve array of actuators types below
      //eg. teA[0] = 3 means that element index has 3 actuators of type 0 below.
      int * teA = getActType(lpt[index]);
      index2 = 0;
      //iterate through all actuator types
      while(index2 < nt){
        if(teA[index2] == 0){ //no actuator of that type below, initialize to -1
          resultAct[index][index2] = malloc(sizeof(int));
          resultAct[index][index2][0] = -1;
        }
        else{
          //else initialize the teA[index2] values in resultAct[element][type]
          resultAct[index][index2] = malloc(sizeof(int) * teA[index2]);
          for(int js = 0; js < teA[index2];js+=1){
            resultAct[index][index2][js] = -1;
          }
        }
        index2+=1;
      }
      //repeat for sensors
      int * teS = getSensType(lpt[index]);
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

  //iterate, each time you find an actuator or sensor do an upward search
  for(int ind = 0; ind < totalNumberOfElements; ind+=1){
    int type =  getType(lpt[ind]);
    if(type == 1){//Sensor
      //the function below is explained in its definition
      upwardSearchSubtreeFilling(lpt, ind, ind, resultSens);
      }
    else if(type == 2){//Actuator
      upwardSearchSubtreeFilling(lpt, ind, ind, resultAct);
      }
    }

  //initialize and fill actuator paths
  int ** arrayActuatorPaths = malloc(sizeof(int *) * (totalNumberOfElements));
  int tot = 0;
  while(tot < totalNumberOfElements){
    arrayActuatorPaths[tot] = malloc(sizeof(int)*(totalNumberOfElements));
    for(int ind = 0; ind < totalNumberOfElements; ind+=1){
      int type =  getType(lpt[ind]);
      if(type == 2){//Is an actuator
        upwardSearchActuatorPaths(lpt, ind, tot, ind, arrayActuatorPaths);
      }
      else{
        arrayActuatorPaths[tot][ind] = -1;
      }
    }
    tot+=1;
  }
  //initilize array containing how many lans each node manages
  int * numberofLANs = malloc(sizeof(int)*totalNumberOfElements);
  for(int c = 0; c < totalNumberOfElements; c+=1){
    numberofLANs[c] = 0;
  }
  //initialize array of directly below elements
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

  //iterate through the graph
  for(int c = 0; c < totalNumberOfElements; c+=1){
    int uppNode = getUpperNode(lpt[c]);
		if(uppNode!=-1){
      //cycle through the number of lower elements of my upper node to find an empty spot
			for(int c2 = 0; c2 < arrayNumberLowerElements[uppNode];c2+=1){
        //in this case we only check for emptiness, impossible to try to put yourself in twice
				if(lowerElementsArray[uppNode][c2] == -1){
					lowerElementsArray[uppNode][c2] = c;
					c2 = arrayNumberLowerElements[uppNode];
					int typeC = getType(lpt[c]);
					if(typeC == 4) //LAN, use the search to find how many LANs a node has directly below
						numberofLANs[uppNode]+=1;
				}
			}
		}
  }
  //allocate and initialize LANs array
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

  //same for WANs down, even if for now only 1 WAN is below or upward each node->
  int * WANsDownArray = malloc(sizeof(int) * totalNumberOfElements);
  for(int c = 0; c < totalNumberOfElements; c+=1){
    WANsDownArray[c] = -1;
  }

  //iterate through the graph and through the lower elements
  //if the lower elements is a LAN, add it to LANSarray
  //else if it is a WAN, add it to WANSdown array
  for(int c = 0; c < totalNumberOfElements; c+=1){
    for(int c2 = 0; c2 < arrayNumberLowerElements[c];c2+=1){
      int low = lowerElementsArray[c][c2];
      int typeLow= getType(lpt[low]);
      if(typeLow == 3){ //WAN
          WANsDownArray[c] = low;
      }

      else if(typeLow == 4){ //LAN
        for(int c3 = 0; c3 < numberofLANs[c];c3+=1){
          if(LANSArray[c][c3] == -1){
            LANSArray[c][c3] = low;
            c3 = numberofLANs[c];
         }
       }
      }
    }
  }
  int * id_arr = malloc(sizeof(int)*totalNumberOfElements);
  int * valid = malloc(sizeof(int)*totalNumberOfElements);
  for(int c = 0; c < totalNumberOfElements; c+=1){
    id_arr[c] = c;
  }

  idmap** idmap_actPaths = malloc(sizeof(idmap*)*totalNumberOfElements);

  for(int c = 0; c < totalNumberOfElements; c+=1){
    idmap_actPaths[c] = create_idmap(id_arr,arrayActuatorPaths[c],totalNumberOfElements,&valid[c]);
  }
  free(id_arr);

  //setup some of the informations we computed
  for(int c = 0; c < totalNumberOfElements; c+=1){
    int typeC = getType(lpt[c]);
    setLowerElements(lpt[c], lowerElementsArray[c],arrayNumberLowerElements[c]);
    setLANs(lpt[c], LANSArray[c],numberofLANs[c]);
    setArrayActuatorPaths(lpt[c],idmap_actPaths[c],valid[c]);
    free(idmap_actPaths[c]);
    if(typeC == 0){
      setWANdown(lpt[c], WANsDownArray[c]);
      setWANup(lpt[c],lpt);
      setListActuatorsByType(lpt[c],resultAct[c],nt);
      setListSensorsByType(lpt[c],resultSens[c],nts);
    }
    else if(typeC == 1){
      int sensType = getSensorType(lpt[c]);
      setSensorRates(lpt[c],sensor_rates[sensType],sizeof(sensor_rates[sensType])*2);
    }
    else if(typeC == 3){ //WAN
      setListActuatorsByType(lpt[c],resultAct[c],nt);
      setListSensorsByType(lpt[c],resultSens[c],nts);
    }
    else if(typeC == 4){
      int lanType = getLanType(lpt[c]);
      setLANserviceTimes(lpt[c],LANsINserviceTimes[lanType],LANsOUTserviceTimes[lanType],sizeof(LANsINserviceTimes[lanType][0])*5,sizeof(LANsOUTserviceTimes[lanType][0])*5);
      setListActuatorsByType(lpt[c],resultAct[c],nt);
      setListSensorsByType(lpt[c],resultSens[c],nts);
    }

  }
  free(numberofLANs);
  free(LANSArray);
  free(lowerElementsArray);
  free(arrayNumberLowerElements);
  free(idmap_actPaths);
  free(valid);
  free(WANsDownArray);
  for(int i = 0; i < nts; i++){
    free(sensor_rates[i]);
  }
  free(sensor_rates);

  for(int i = 0; i < ntl; i++){
    free(LANsINserviceTimes[i]);
    free(LANsOUTserviceTimes[i]);

  }
  free(LANsINserviceTimes);
  free(LANsOUTserviceTimes);

  for(int j = 0; j  < totalNumberOfElements; j++){
    if(getType(lpt[j]) != 1 && getType(lpt[j]) != 2){
      for(int i = 0; i < nts; i++){
        free(resultSens[j][i]);
      }
    free(resultSens[j]);
    }
  }
  free(resultSens);

  for(int j = 0; j  < totalNumberOfElements; j++){
    if(getType(lpt[j]) != 1 && getType(lpt[j]) != 2){
      for(int i = 0; i < nt; i++){
        free(resultAct[j][i]);
      }
    free(resultAct[j]);
    }
  }
  free(resultAct);

  for(int i = 0; i < totalNumberOfElements; i ++){
    free(arrayActuatorPaths[i]);
  }
  free(arrayActuatorPaths);


  lp_topology * lp_element_topology = getLPtopoogy(path1);
  int ** reachableSetElement = malloc(sizeof(int*)*totalNumberOfElements);
  int ** reachableSetLP = malloc(sizeof(int*)*totalNumberOfElements);

  for(int i = 0; i < totalNumberOfElements; i ++){

    int numLower = getNumberLower(lpt[i]);
    int * Lowers = getLowers(lpt[i]);
    int upp = getUpperNode(lpt[i]);
    if(upp == -1){
      reachableSetLP[i] = malloc(sizeof(int) * (numLower));//+1 for upper
      reachableSetElement[i] = malloc(sizeof(int) * (numLower));//+1 for upper
      for(int j = 0; j < numLower; j++){
        reachableSetLP[i][j] = getElementToLPMappingOneElement(lp_element_topology,Lowers[j]);
        reachableSetElement[i][j] = Lowers[j];
      }
    }
    else{
      reachableSetLP[i] = malloc(sizeof(int) * (numLower+1));//+1 for upper
      reachableSetElement[i] = malloc(sizeof(int) * (numLower+1));//+1 for upper
      reachableSetLP[i][0] = getElementToLPMappingOneElement(lp_element_topology,upp);
      reachableSetElement[i][0] = upp;
      for(int j = 0; j < numLower; j++){
        reachableSetLP[i][j+1] = getElementToLPMappingOneElement(lp_element_topology,Lowers[j]);
        reachableSetElement[i][j+1] = Lowers[j];
      }
    }

  }

  for(int i = 0; i < totalNumberOfElements; i ++){
    int valid = 0;
    int upp = getUpperNode(lpt[i]);
    if(upp != -1){
      lpt[i]->ElementToLPMapping = create_idmap(reachableSetElement[i],reachableSetLP[i],getNumberLower(lpt[i])+1,&valid);
      lpt[i]->numValidElToLP = valid;
    }
    else {
      lpt[i]->ElementToLPMapping = create_idmap(reachableSetElement[i],reachableSetLP[i],getNumberLower(lpt[i]),&valid);
      lpt[i]->numValidElToLP = valid;
    }

  }
  for(int i = 0; i < totalNumberOfElements; i ++){
    free(reachableSetLP[i]);
    free(reachableSetElement[i]);

  }
  free(reachableSetLP);
  free(reachableSetElement);
  EST->lpt = lpt;
  EST->lp_topology = lp_element_topology;
  return EST;
}
