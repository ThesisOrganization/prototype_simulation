/** \file formulas.c
 * \brief A "simple" program to compute the queuing model parameters, given a topology file.
 * The program will write the computed parameters on a LaTeX file called results.tex, which can be directlry included in repots if necessary.
 */

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"../utils/partop/header.h"
#include"quick_sort.h"

/// The visit type of the topology graph, since we need to visit it more than once.
typedef enum{
	GRAPH_COMPUTE_RATES=0, ///< This visit will compute all the paramters for telemetry, transition and batch send messages
	GRAPH_COMPUTE_COMMANDS, ///< This visit will compute the parameters for command messages.
	GRAPH_PRINT_DATA, ///< This visit will print the computed parameters on the output file and free memory.
	NUM_GRAPH_VISITS ///< total number of visits __KEEP THIS AT LAST POSITION__.
} graph_visit_type;

///Used to avoid confusion while indexing the results array given by the ::compute_data function.
typedef enum{
	CLASS_TELEMETRY=0, ///< telemetry messages.
	CLASS_TRANSITION, ///< transition messages.
	CLASS_BATCH, ///< batch send messages.
	CLASS_COMMAND, ///< command messages.
	NUM_CLASSES
} data_classes;

/// The number of visits that a message class can perform on a queue.
typedef enum{
	NODE_SINGLE_VISIT=1,
	NODE_DOUBLE_VISIT
} node_visits_per_message;

///Paramters used to indicate that the curernt node is a LAN which has been split in an input queue and an output queue.
typedef enum {
	NODE_NOT_SPLIT=-1, ///< Not split, treat it as a single queue.
	NODE_SPLIT_IN_OUT, ///< Node split in two queues.
} node_splitting_classes;

/// Struct used to hold the parameters computed for each element of the topology.
typedef struct node_data{
	//we use the input_rates array for the lan in and the output_rates array for the lan out when the node is a lan
	double *input_rates,*service_demands,*utilization_factors,*response_times,*output_rates, total_utilization_factor; ///< Parameters to be computed
	topology* top; ///< The ::topology*.
	data_classes classes; ///< The number of classes of messages (usually this is equal to ::NUM_CLASSES).
	int* node_visits_per_class; ///< The visits that each messages does on the current node, ordered by ::data_classes
	int node_id; ///< The id of the current node in the topology.
	int num_childrens; ///< The number of below connected nodes.
	char* name; ///< The name of the node (this usually matches to the node's role, e.g. sensor,actuator,local, central and regional).
	char* type; ///< If the node is an actuator / sensor then it's type will be here.
	struct node_data** childrens; ///< The array of ids of the below connected nodes.
	struct node_data* father; ///< The above connected node.
	//the parameters below make sense only for lans
	node_splitting_classes lan_direction; ///this parameter makes sense only for lan/split elements, if its value is not ::NODE_NOT_SPLIT then we need to use the below double* arrays instead of the ones above, rates excluded.
	double *input_service_demands,*input_utilization_factors,*input_response_times, input_total_utilization_factor; ///< Input parameters.
	double output_total_utilization_factor,*output_service_demands,*output_utilization_factors,*output_response_times; ///< output parameters.
} node_data;

/** \brief Print a LaTeX table header in booktabs format.
 * \param[in] cols Number of columns of the table.
 * \param[in] table_header The header of the table.
 * \param[in] out The file in which the header must be printed.
 */
void print_table_header(int cols,char* table_header,FILE* out){
	int i;
	fprintf(out,"\\begin{table}[H]\n\\centering\n\\begin{tabular}{@{}");
	for(i=0;i<cols;i++){
		fprintf(out,"c");
	}
	fprintf(out,"@{}}\n\\toprule\n%s",table_header);
}

/** \brief Prints an array of results as one row of a LaTeX table.
 * \param[in] elem The name of the element.
 * \param[in] type The type of the element.
 * \param[in] node_id The id of the node.
 * \param[in] table_type The description of the table.
 * \param[in] table_header The header of the table.
 * \param[in] results The array of parameters to be printed.
 * \param[in] num_results the length of the parameters array.
 * \param[in] out The file in which the table must be printed.
 *
 * The table is printed with precision to the third decimal number and uses scientific notation and separators when necessary.
 */
void print_results_in_table(char* elem,char* type,int node_id,char* table_type,char* table_header,double* results,int num_results,FILE* out){
	int i;
	//we print the table header
	print_table_header(num_results,table_header,out);
	//we print the row of the table (see https://en.wikipedia.org/wiki/Printf_format_string for info on the format)
	fprintf(out,"%'#.3g",results[0]);
	for(i=1;i<num_results-1;i++){
		fprintf(out,"& %'#.3g",results[1]);
	}
	fprintf(out,"& %'#.3g \\\\\n",results[num_results-1]);
	//we print the table footer
	fprintf(out,"\\bottomrule\n\\end{tabular}\n\\caption{%s id:%d %s}\n\\label{tab:%s-%d}\n\\end{table}",type,node_id,table_type,elem,node_id);
}

/** \brief Prints the parameters of the specified node in several tables.
 * \param[in] node The node of which we want to print the parameters.
 * \param[in] out The file wher we want to print.
 */
void print_results(node_data* node,FILE* out){
	char* table_header=malloc(sizeof(char)*512);
	char* table_type=malloc(sizeof(char)*128);
	fprintf(out,"\\paragraph{%s %d}\n",node->name,node->node_id);
	if(node->type!=NULL){
		fprintf(out,"Type: %s\n",node->type);
	}
	if(getType(node->top,node->node_id)!=WAN && (getType(node->top,node->node_id)!=LAN && node->lan_direction!=NODE_NOT_SPLIT)){
		//we print the rates table
		snprintf(table_type,sizeof(char)*128,"input rates");
		snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & \\lambda_e & \\lambda_c && \\lambda_b\\\\\n\\midrule\n");
		print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->input_rates,node->classes,out);
		snprintf(table_type,sizeof(char)*128,"output rates");
		snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & \\lambda_e & \\lambda_c && \\lambda_b\\\\\n\\midrule\n");
		print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->output_rates,node->classes,out);
		//we print the service demand table
		snprintf(table_type,sizeof(char)*128,"service demands");
		snprintf(table_header,sizeof(char)*512,"$D_t$ & D_e & D_c && D_b\\\\\n\\midrule\n");
		print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->service_demands,node->classes,out);
		//we print the utilization factors
		snprintf(table_type,sizeof(char)*128,"utilization factors");
		snprintf(table_header,sizeof(char)*512,"$U_t$ & U_e & U_c && U_b\\\\\n\\midrule\n");
		print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->utilization_factors,node->classes,out);
		fprintf(out,"Total utilization factor: %'#.3g",node->total_utilization_factor);
		//we print the response times
		snprintf(table_type,sizeof(char)*128,"response times");
		snprintf(table_header,sizeof(char)*512,"$R_t$ & R_e & R_c && R_b\\\\\n\\midrule\n");
		print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->response_times,node->classes,out);
	} else{
		if(getType(node->top,node->node_id)==WAN ){
			fprintf(out,"Delay: %'#.3g",((lp_infos*)getInfo(node->top,node->node_id))->delay);
		} else {
			snprintf(table_type,sizeof(char)*128,"Lan in rates");
			snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & \\lambda_e & \\lambda_c && \\lambda_b\\\\\n\\midrule\n");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->input_rates,node->classes,out);
			snprintf(table_type,sizeof(char)*128,"Lan out rates");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->output_rates,node->classes,out);
			//we print the service demand tables
			snprintf(table_type,sizeof(char)*128,"Lan in service demands");
			snprintf(table_header,sizeof(char)*512,"$D_t$ & D_e & D_c && D_b\\\\\n\\midrule\n");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->input_service_demands,node->classes,out);
			snprintf(table_type,sizeof(char)*128,"Lan out service demands");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->output_service_demands,node->classes,out);
			//we print the utilization factors
			snprintf(table_type,sizeof(char)*128,"Lan in utilization factors");
			snprintf(table_header,sizeof(char)*512,"$U_t$ & U_e & U_c && U_b\\\\\n\\midrule\n");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->input_utilization_factors,node->classes,out);
			fprintf(out,"Lan in Total utilization factor: %'#.3g",node->input_total_utilization_factor);
			snprintf(table_type,sizeof(char)*128,"Lan out utilization factors");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->output_utilization_factors,node->classes,out);
			fprintf(out,"Lan out Total utilization factor: %'#.3g",node->output_total_utilization_factor);
			//we print the response times
			snprintf(table_type,sizeof(char)*128,"Lan in response times");
			snprintf(table_header,sizeof(char)*512,"$R_t$ & R_e & R_c && R_b\\\\\n\\midrule\n");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->input_response_times,node->classes,out);
			snprintf(table_type,sizeof(char)*128,"Lan out response times");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->output_response_times,node->classes,out);
		}
	}
	free(table_header);
	free(table_type);
}

/** \brief Locate the central node.
 * \param[in] array The ::topArray struct which contains all the nodes of the topology.
 * \param[in] elements The lenght of the ::topArray struct (usually ::topology->num_nodes).
 * \returns The id of the central node (supposed to be unique).
 */
int find_central(topArray** array,int elements){
	int i,central=-1;
	for(i=0;i<elements && central==-1;i++){
		if(((lp_infos*) array[i]->info)->lp_type==NODE && ((lp_infos*)array[i]->info)->node_type==CENTRAL){
			central=i;
		}
	}
	assert(central!=-1);
	return central;
}

/** \brief Determines which are the nodes conected below a single nodes (eliminating duplicate ids).
 * \param[in] top The ::topology* caontaining the general information of the topology.
 * \param[in] node_id The id of the node of which we want to know the lower nodes.
 * \param[out] num_lowers The number of lower nodes.
 * \return a -1 terminated array of integers, which represents the ids of the lower nodes connected to `node_id`
 */
int* find_nodes_to_visit(topology* top,int node_id,int* num_lowers){
	int i,j,k,duplicate;
	int num_lans=getNumberLANS(top,node_id);
	int num_nodes=getNumberLower(top,node_id);
	int *nodes=getLowers(top,node_id);
	int *lans=getLANS(top,node_id);
	// the array below is used to visit the lower nodes will be terminated by -1
	int *lowers=NULL;
	//when both the number of node and lans is not 0 we need to detect duplicates before exploring the subtrees to avoid doinf the same computation multiple times
	if(num_lans>0 && num_nodes>0){
		lowers=malloc(sizeof(int)*(num_lans+num_nodes+1));
		memset(lowers,-1,sizeof(int)*(num_lans+num_nodes+1));
		//we sort the lower nodes and the lans array
		quicksort(nodes,getNumberLower(top,node_id));
		quicksort(lans,getNumberLANS(top,node_id));
		//we copy the contents of the nodes array into lower
		memcpy(lowers,nodes,sizeof(int)*num_nodes);
		//then we add the non-duplicate items
		k=num_nodes;
		for(i=0;i<num_lans;i++){
			duplicate=0;
			for(j=0;j<num_nodes;j++){
				if(lans[i]==nodes[j]){
					duplicate=1;
				}
			}
			if(!duplicate){
				lowers[k]=lans[i];
				k++;
			}
		}
		*num_lowers=k+1;
	} else{
		if(num_lans==0){
			lowers=nodes;
			*num_lowers=num_nodes;
		} else {
			if(num_nodes>0){
				lowers=lans;
				*num_lowers=num_lans;
			}
		}
	}
	return lowers;
}

/** \brief Computes parameters for the selected node.
 * \param[in] node The node of which we want to compute parameters.
 * \param[in] visit_type The type of graph visit we are performing.
 */
void compute_data(node_data* node,graph_visit_type visit_type){
	double total_command_dest_weight_num=0,branch_command_dest_weight_num=0,*childrens_total_rate,service_time=0,rate_transition_sensors_below=0,rate_telemetry_sensors_below=0,rate_transition_actuators_below=0,*rates;
	node_data* father_node=NULL;
	int i,j,start_class,father_type;
	int elem_type=getType(father_node->top,father_node->node_id);
	//we get the node information struct
	lp_infos *our_info=(lp_infos*) getInfo(node->top,node->node_id);
	// if we are not the a sensor or the central node we need to compute the number of actuators that our upper node can reach with a command message, weighted by their probability to be reached
	if(elem_type!=SENSOR && (elem_type==NODE && our_info->node_type!=CENTRAL)){
		switch(elem_type){
			case NODE:
				if(our_info->node_type==LOCAL){
					father_type=REGIONAL;
				} else{
					father_type=CENTRAL;
				}
				break;
			default:
				father_type=LOCAL;
		}
		//get the father node
		father_node=node->father;
		while(elem_type!=NODE || ((lp_infos*) getInfo(father_node->top,father_node->node_id))->node_type!=father_type){
			father_node=father_node->father;
		}
		//compute the weighted number of actuators below our father
		for(i=0;i<node->top->numberOfActTypes;i++){
			total_command_dest_weight_num+=((lp_infos*)getInfo(father_node->top,father_node->node_id))->actuatorsTypesBelow[i]*father_node->top->probOfActuators[i];
		}
		//if we are not an actautor we need to compute the weighted number of actuators below us
		if(elem_type!=ACTUATOR){
			for(i=0;i<node->top->numberOfActTypes;i++){
				branch_command_dest_weight_num+=((lp_infos*)getInfo(node->top,node->node_id))->actuatorsTypesBelow[i]*node->top->probOfActuators[i];
			}
		}
	}
	// we have different rates formula for each element type
	switch(getType(node->top,node->node_id)){
		case SENSOR:
			if(visit_type==GRAPH_COMPUTE_RATES){
				//sensors can send only telemetry and transition messages
				node->output_rates[CLASS_TELEMETRY]=node->top->sensorRatesByType[our_info->sensor_type][TELEMETRY];
				node->output_rates[CLASS_TRANSITION]=node->top->sensorRatesByType[our_info->sensor_type][TRANSITION];
			}
			break;
		case ACTUATOR:
			//actuators send transitions and receive commands
			if(visit_type==GRAPH_COMPUTE_RATES){
				node->output_rates[CLASS_TRANSITION]=our_info->rateTransition;
			} else {
				node->input_rates[CLASS_COMMAND]= father_node->output_rates[CLASS_COMMAND] * ( node->top->probOfActuators[our_info->actuator_type] / total_command_dest_weight_num);
			}
			break;
		case LAN:
			//Lan rely messages from local nodes to sensors /actuators and vice versa, but are split in two queues
			if(visit_type==GRAPH_COMPUTE_RATES){
				for(i=0;i<node->top->numberOfSensTypes;i++){
					rate_telemetry_sensors_below+=our_info->sensorsTypesBelow[i]*getSensorRatesForOneSensorType(node->top,i)[TELEMETRY];
					rate_transition_sensors_below+=our_info->sensorsTypesBelow[i]*getSensorRatesForOneSensorType(node->top,i)[TRANSITION];
				}
				for(i=0;i<node->num_childrens;i++){
					if(getType(node->top,node->childrens[i]->node_id)==ACTUATOR){
						rate_transition_actuators_below+=((lp_infos*)getInfo(node->top,node->childrens[i]->node_id))->rateTransition;
					}
				}
				node->output_rates[CLASS_TELEMETRY]=rate_telemetry_sensors_below;
				node->output_rates[CLASS_TRANSITION]=rate_transition_sensors_below+rate_transition_actuators_below;
			}else{
				node->input_rates[CLASS_COMMAND]=father_node->output_rates[CLASS_COMMAND] * branch_command_dest_weight_num / total_command_dest_weight_num;
			}
			break;
		case NODE:
			//node rely telemetry and trasition message to their upper node and commands to their below node
			if(visit_type==GRAPH_COMPUTE_RATES){
				//we need to sum the output rates of all our childrens
				childrens_total_rate=calloc(NUM_CLASSES-1,sizeof(double));
				for(j=CLASS_TELEMETRY;j<CLASS_COMMAND;j++){
					for(i=0;i<node->num_childrens;i++){
						childrens_total_rate[j]+=node->childrens[i]->output_rates[j];
					}
				}
				node->input_rates[CLASS_TELEMETRY]=childrens_total_rate[CLASS_TELEMETRY];
				node->output_rates[CLASS_TELEMETRY]=childrens_total_rate[CLASS_TELEMETRY]/our_info->aggregation_rate;
				node->input_rates[CLASS_TRANSITION]=childrens_total_rate[CLASS_TRANSITION];
				node->output_rates[CLASS_TRANSITION]=node->input_rates[CLASS_TRANSITION];
				node->input_rates[CLASS_BATCH]=childrens_total_rate[CLASS_BATCH];
				node->output_rates[CLASS_BATCH]=node->input_rates[CLASS_BATCH]+node->top->probNodeCommandArray[our_info->node_type]*node->input_rates[CLASS_TRANSITION];
				free(childrens_total_rate);
			} else{
				if(our_info->node_type!=CENTRAL){
						node->input_rates[CLASS_COMMAND]=father_node->output_rates[CLASS_COMMAND] * branch_command_dest_weight_num / total_command_dest_weight_num;
						node->output_rates[CLASS_COMMAND]=node->input_rates[CLASS_COMMAND]+node->top->probNodeCommandArray[our_info->node_type]*node->input_rates[CLASS_TRANSITION];
				} else{
					node->output_rates[CLASS_COMMAND]=node->top->probNodeCommandArray[our_info->node_type]*node->input_rates[CLASS_TRANSITION];
				}
			}
			break;
	}
	//computing service time, utilization factor and response time for each message class

	if(visit_type==GRAPH_COMPUTE_RATES){
		start_class=CLASS_TELEMETRY;
		rates=node->input_rates;
	}else{
		start_class=CLASS_COMMAND;
		//when computing commands utilization factor we cannot consider only the input rate, but we must also consider the commands generated by the element, so we use the element output rates.
		rates=node->output_rates;
	}
	for(i=start_class;i<NUM_CLASSES;i++){
		// we need to convert the data_classes to job_type to use the service_time array.
		switch(i){
			case CLASS_TELEMETRY:
				service_time=our_info->service_time[TELEMETRY];
				break;
			case CLASS_TRANSITION:
				service_time=our_info->service_time[TRANSITION];
				break;
			case CLASS_COMMAND:
				service_time=our_info->service_time[COMMAND];
				break;
			case CLASS_BATCH:
				service_time=our_info->service_time[BATCH];
		}
		if(our_info->lp_type==ACTUATOR && visit_type==GRAPH_COMPUTE_COMMANDS){
			service_time=our_info->serviceTimeCommand;
		}
		//LAN are split in two queues, so we use different array to compute the queue parameters
		if(node->lan_direction==NODE_SPLIT_IN_OUT){
			//lan in
			node->input_service_demands[i]= node->node_visits_per_class[i] * node->top->LANsINserviceTimes[our_info->lan_type][i];
			node->utilization_factors[i]= node->input_rates[i] * node->input_service_demands[i];
			node->input_response_times[i]= node->input_service_demands[i] / (1- node->input_utilization_factors[i]);
			node->input_total_utilization_factor+=node->input_utilization_factors[i];
			//lan out
			node->output_service_demands[i]= node->node_visits_per_class[i] * node->top->LANsOUTserviceTimes[our_info->lan_type][i];
			node->output_utilization_factors[i]= node->output_rates[i] * node->output_service_demands[i];
			node->output_response_times[i]= node->output_service_demands[i] / (1- node->output_utilization_factors[i]);
			node->output_total_utilization_factor+=node->output_utilization_factors[i];
		} else {
			//all the other elements use only one queue, so their parameters can be generalized
			node->service_demands[i]= node->node_visits_per_class[i] * service_time;
			node->utilization_factors[i]= rates[i] * node->service_demands[i];
			node->response_times[i]= node->service_demands[i] / (1- node->utilization_factors[i]);
			node->total_utilization_factor+=node->utilization_factors[i];
		}
	}
}

/** \brief Initializes the ::node_data struct.
 * \param[in] data The structure to be initialized.
 * \param[in] node_id The node id.
 * \param[in] father The topology element that has the current node as children (in the dataflow).
 * \param[in] top The ::topology struct containing the general info.
 */
void init_node_data(node_data *data,int node_id,node_data* father,topology* top){
	memset(data,0,sizeof(node_data));
	data->node_id=node_id;
	data->father=father;
	data->top=top;
	data->classes=NUM_CLASSES;
	data->input_rates=calloc(NUM_CLASSES,sizeof(double));
	data->output_rates=calloc(NUM_CLASSES,sizeof(double));
	//transitio messages have two visits only between nodes
	if(getType(top,node_id)!=NODE){
		data->node_visits_per_class[CLASS_TRANSITION]=NODE_SINGLE_VISIT;
	} else {
		data->node_visits_per_class[CLASS_TRANSITION]=NODE_DOUBLE_VISIT;
	}
	if(getType(top,node_id)==LAN){
		data->lan_direction=NODE_SPLIT_IN_OUT;
		data->input_service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->output_service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->input_utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->output_utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->input_response_times=calloc(NUM_CLASSES,sizeof(double));
		data->output_response_times=calloc(NUM_CLASSES,sizeof(double));
	}else{
		data->lan_direction=NODE_NOT_SPLIT;
		data->service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->response_times=calloc(NUM_CLASSES,sizeof(double));
	}
	data->node_visits_per_class=calloc(NUM_CLASSES,sizeof(int));
	data->node_visits_per_class[CLASS_TELEMETRY]=NODE_SINGLE_VISIT;
	data->node_visits_per_class[CLASS_COMMAND]=NODE_SINGLE_VISIT;
	data->node_visits_per_class[CLASS_BATCH]=NODE_SINGLE_VISIT;
	//setup of name and type variables
	data->name=malloc(sizeof(char)*128);
	switch(getType(top,node_id)){
		case SENSOR:
			snprintf(data->name,sizeof(char)*128,"Sensor");
			data->type=malloc(sizeof(char)*128);
			snprintf(data->type,sizeof(char)*128,"Type: %d",((lp_infos*)getInfo(top,node_id))->sensor_type);
			break;
		case ACTUATOR:
			snprintf(data->name,sizeof(char)*128,"Actuator");
			data->type=malloc(sizeof(char)*128);
			snprintf(data->type,sizeof(char)*128,"Type: %d",((lp_infos*)getInfo(top,node_id))->actuator_type);
			break;
		case LAN:
			snprintf(data->name,sizeof(char)*128,"Lan");
			break;
		case WAN:
			snprintf(data->name,sizeof(char)*128,"Wan");
			break;
		case NODE:
			switch(((lp_infos*)getInfo(top,node_id))->node_type){
				case CENTRAL:
					snprintf(data->name,sizeof(char)*128,"Central node");
					break;
				case REGIONAL:
					snprintf(data->name,sizeof(char)*128,"Regional node");
					break;
				case LOCAL:
					snprintf(data->name,sizeof(char)*128,"Local node");
					break;
			}
			break;
	}
}

/** \brief Frees the memory used by a ::node_data struct.
 * \param[in] data The struct to be freed.
 * Since the struct is included in the upper node children list it will not freed directly, but when its upper node frees its data struct; in the meantine, to avoid memory problems it will be reinitialized to 0.
 */
void free_node_data(node_data* data){
	int i;
	if(data->input_rates!=NULL){
		free(data->input_rates);
	}
	if(data->output_rates!=NULL){
		free(data->output_rates);
	}
	if(data->service_demands!=NULL){
		free(data->service_demands);
	}
	if(data->utilization_factors!=NULL){
		free(data->utilization_factors);
	}
	if(data->response_times!=NULL){
		free(data->response_times);
	}
	if(data->input_service_demands!=NULL){
		free(data->service_demands);
	}
	if(data->input_utilization_factors!=NULL){
		free(data->utilization_factors);
	}
	if(data->input_response_times!=NULL){
		free(data->response_times);
	}
	if(data->output_service_demands!=NULL){
		free(data->service_demands);
	}
	if(data->output_utilization_factors!=NULL){
		free(data->utilization_factors);
	}
	if(data->output_response_times!=NULL){
		free(data->response_times);
	}
	if(data->node_visits_per_class!=NULL){
		free(data->node_visits_per_class);
	}
	if(data->name!=NULL){
		free(data->name);
	}
	if(data->type!=NULL){
		free(data->type);
	}
	if(data->childrens!=NULL){
		for(i=0;i<data->num_childrens;i++){
			free(data->childrens[i]);
		}
	}
	memset(data,0,sizeof(node_data));
}
/** \brief Visits the graph to perform several operations, according to ::graph_visit_type.
 * \param[in] data The ::node_data struct of the current node, when starting the visit it should be the central node.
 * \param[in] visit_type The type of visit to perform.
 * \param[in] out The output file (needed for ::GRAPH_PRINT_DATA visit type).
 */
void graph_visit(node_data* data,graph_visit_type visit_type,FILE* out){
	assert(data!=NULL && visit_type<NUM_GRAPH_VISITS);
	int *lowers=NULL,num_lowers=0,i;
	lp_infos* info=(lp_infos*)getInfo(data->top,data->node_id);
	//we get the array of the lower nodes to be visited
	lowers=find_nodes_to_visit(data->top,data->node_id,&num_lowers);
	//this is the first visit of the tree, so we need to allocate a node_data struct for each children
	if(visit_type==GRAPH_COMPUTE_RATES){
		data->childrens=calloc(num_lowers,sizeof(node_data*));
		for(i=0;i<num_lowers;i++){
			data->num_childrens++;
			data->childrens[i]=malloc(sizeof(node_data));
			init_node_data(data->childrens[i],lowers[i],data,data->top);
		}
	}
		//in the second visit we need to compute the rates for command messages before reaching the leaves
	if(visit_type==GRAPH_COMPUTE_COMMANDS && info->node_type!=WAN){
		compute_data(data,visit_type);
	}
	//to begin computation and to print results we need to reach a node with no lower nodes
	for(i=0;i<num_lowers && lowers!=NULL;i++){
		graph_visit(data->childrens[i],visit_type,out);
	}
	//during the first visit we need to compute rate for most of the message types starting for the leaves of the tree
	if(visit_type==GRAPH_COMPUTE_RATES && info->node_type!=WAN){
		compute_data(data,visit_type);
	}
	// in the third visit we print the data and free the occupied memory
	if(visit_type==GRAPH_PRINT_DATA){
		print_results(data,out);
		free_node_data(data);
	}
}

/// \brief Computes the parameters for the topology file location passed as the first argument.
int main(int argc, char** argv){
	assert(argc==2);
	//get topology from file
	char* topology_path=argv[1];
	FILE *out=fopen("results.tex","w");
	fprintf(out,"\\section{Computed Formulas Example}\n The following data is computed from the topology stored in file %s.\\\\\n",topology_path);
	printf("topology file:%s\n",argv[1]);
	topology * genTop = getTopology(topology_path);
	node_data* central=malloc(sizeof(node_data));
	init_node_data(central,find_central(genTop->topArr,genTop->total_nodes),NULL,genTop);
	//1st visit, to compute rates
	graph_visit(central,GRAPH_COMPUTE_RATES,out);
	//2nd visit, to compute commands
	graph_visit(central,GRAPH_COMPUTE_COMMANDS,out);
	//3rd visit, to print results
	graph_visit(central,GRAPH_PRINT_DATA,out);
	free(central);
	return 0;
}
