#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include"../utils/partop/header.h"
#include"quick_sort.h"


typedef enum{
	GRAPH_COMPUTE_RATES=0, ///< This visit will compute all the paramters for telemetry, transition and batch send messages
	GRAPH_COMPUTE_COMMANDS, ///< This visit will compute the parameters for command messages
	GRAPH_PRINT_DATA,
	NUM_GRAPH_VISITS ///< total number of visits __KEEP THIS AT LAST POSITION__
} graph_visit_type;

typedef struct{
	double local_gen_comm; ///< probability of a local node to receive a command
	double reg_gen_comm; ///< probability of a regional node to receive a command
	double cent_gen_comm; ///< probability of a central node to receive a command
} probs;

/// an enum used to avoid confusion while indexing the results array given by the ::compute_data function
typedef enum{
	CLASS_TELEMETRY=0, ///< telemetry messages
	CLASS_TRANSITION, ///< transition messages
	CLASS_BATCH, ///< batch send messages
	CLASS_COMMAND, ///< command messages
	NUM_CLASSES
} data_classes;

typedef enum{
	NODE_SINGLE_VISIT=1,
	NODE_DOUBLE_VISIT
} node_visits_per_message;

typedef enum {
	LAN_DIR_IGNORE=-1,
	LAN_DIR_IN_OUT,
} lan_direction_classes;

typedef struct node_data{
	//we use the input_rates array for the lan in and the output_rates array for the lan out when the node is a lan
	double *input_rates,*service_demands,*utilization_factors,*response_times,*output_rates;
	double total_utilization_factor;
	topology* top;
	int classes;
	int* node_visits_per_class;
	int node_id;
	int num_childrens;
	char* name;
	char* type;
	struct node_data** childrens;
	struct node_data* father;
	//the parameters below make sense only for lans
	int lan_direction; //this parameter makes sense only for lan elements, uses values from the lan_direction_classes enum, it its value is not LAN_DIR_IGNORE then we need to use the below double* arrays
	double *lan_in_service_demands,*lan_in_utilization_factors,*lan_in_response_times;
	double lan_in_total_utilization_factor,lan_out_total_utilization_factor;
	double *lan_out_service_demands,*lan_out_utilization_factors,*lan_out_response_times;
} node_data;

void print_table_header(int cols,char* table_header,FILE* out){
	int i;
	fprintf(out,"\\begin{table}[H]\n\\centering\n\\begin{tabular}{@{}");
	for(i=0;i<cols;i++){
		fprintf(out,"c");
	}
	fprintf(out,"@{}}\n\\toprule\n%s",table_header);
}

void print_results_in_table(char* elem,char* type,int node_id,char* table_type,char* table_header,double* results,int num_results,FILE* out){
	int i;
	char* elem_type=malloc(sizeof(char)*128);
	//we print the table header
	print_table_header(num_results,table_header,out);
	//we print the row of the table (see https://en.wikipedia.org/wiki/Printf_format_string for info on the format)
	fprintf(out,"%'#.3g",results[0]);
	for(i=1;i<num_results-1;i++){
		fprintf(out,"& %'#.3g",results[1]);
	}
	fprintf(out,"& %'#.3g \\\\\n",results[num_results-1]);
	//we print the table footer
	if(type!=NULL){
		snprintf(elem_type,sizeof(char)*128,"%s type:%s",elem,type);
	} else{
		strncpy(elem_type,elem,sizeof(char)*128);
	}
	fprintf(out,"\\bottomrule\n\\end{tabular}\n\\caption{%s id:%d %s}\n\\label{tab:%s-%d}\n\\end{table}",elem_type,node_id,table_type,elem,node_id);
	free(elem_type);
}

void print_results(node_data* node,FILE* out){
	char* table_header=malloc(sizeof(char)*512);
	char* table_type=malloc(sizeof(char)*128);
	fprintf(out,"\\paragraph{%s %d}\n",node->name,node->node_id);
	if(node->type!=NULL){
		fprintf(out,"Type: %s\n",node->type);
	}
	if(getType(node->top,node->node_id)!=WAN && (getType(node->top,node->node_id)!=LAN && node->lan_direction!=LAN_DIR_IGNORE)){
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
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->lan_in_service_demands,node->classes,out);
			snprintf(table_type,sizeof(char)*128,"Lan out service demands");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->lan_out_service_demands,node->classes,out);
			//we print the utilization factors
			snprintf(table_type,sizeof(char)*128,"Lan in utilization factors");
			snprintf(table_header,sizeof(char)*512,"$U_t$ & U_e & U_c && U_b\\\\\n\\midrule\n");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->lan_in_utilization_factors,node->classes,out);
			fprintf(out,"Lan in Total utilization factor: %'#.3g",node->lan_in_total_utilization_factor);
			snprintf(table_type,sizeof(char)*128,"Lan out utilization factors");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->lan_out_utilization_factors,node->classes,out);
			fprintf(out,"Lan out Total utilization factor: %'#.3g",node->lan_out_total_utilization_factor);
			//we print the response times
			snprintf(table_type,sizeof(char)*128,"Lan in response times");
			snprintf(table_header,sizeof(char)*512,"$R_t$ & R_e & R_c && R_b\\\\\n\\midrule\n");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->lan_in_response_times,node->classes,out);
			snprintf(table_type,sizeof(char)*128,"Lan out response times");
			print_results_in_table(node->name,node->type,node->node_id,table_type,table_header,node->lan_out_response_times,node->classes,out);
		}
	}
	free(table_header);
	free(table_type);
}

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

/**
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

void compute_data(node_data* node,int visit_type){
	double total_command_dest_weight_num=0,branch_command_dest_weight_num=0,*childrens_total_rate,service_time=0,rate_transition_sensors_below=0,rate_telemetry_sensors_below=0,rate_transition_actuators_below=0,*rates;
	node_data* father_node=NULL;
	int i,j,start_class,father_type;
	int elem_type=getType(father_node->top,father_node->node_id);
	//we get the node information struct
	lp_infos *our_info=(lp_infos*) getInfo(node->top,node->node_id),*info=NULL;
	// if we are not the a sensor or the central node we need to compute the number of actuators that our upper node can reach with a command message, weighted by their probability to be reached
	if(elem_type!=SENSOR && (elem_type==NODE && our_info->node_type!=CENTRAL)){
		switch(elem_type){
			case NODE:
				if(our_info->node_type==LOCAL){
					father_type=REGIONAL;
				} else{
					father_type=CENTRAL;
				}
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
	switch(getType(node->top,node->node_id)){
		case SENSOR:
			if(visit_type==GRAPH_COMPUTE_RATES){
				//sensors can send only telemetry and transition messages
				node->output_rates[CLASS_TELEMETRY]=node->top->sensorRatesByType[our_info->sensor_type][TELEMETRY];
				node->output_rates[CLASS_TRANSITION]=node->top->sensorRatesByType[our_info->sensor_type][TRANSITION];
			}
			break;
		case ACTUATOR:
			if(visit_type==GRAPH_COMPUTE_RATES){
				//setup of Elem and type variables
				node->output_rates[CLASS_TRANSITION]=our_info->rateTransition;
			} else {
				node->input_rates[CLASS_COMMAND]= father_node->output_rates[CLASS_COMMAND] * ( node->top->probOfActuators[our_info->actuator_type] / total_command_dest_weight_num);
			}
			break;
		case LAN:
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
				//TODO FIXME
			}
		case NODE:
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
		if(node->lan_direction==LAN_DIR_IN_OUT){
			//lan in
			node->lan_in_service_demands[i]= node->node_visits_per_class[i] * node->top->LANsINserviceTimes[our_info->lan_type][i];
			node->utilization_factors[i]= node->input_rates[i] * node->lan_in_service_demands[i];
			node->lan_in_response_times[i]= node->lan_in_service_demands[i] / (1- node->lan_in_utilization_factors[i]);
			node->lan_in_total_utilization_factor+=node->lan_in_utilization_factors[i];
			//lan out
			node->lan_out_service_demands[i]= node->node_visits_per_class[i] * node->top->LANsOUTserviceTimes[our_info->lan_type][i];
			node->lan_out_utilization_factors[i]= node->output_rates[i] * node->lan_out_service_demands[i];
			node->lan_out_response_times[i]= node->lan_out_service_demands[i] / (1- node->lan_out_utilization_factors[i]);
			node->lan_out_total_utilization_factor+=node->lan_out_utilization_factors[i];
		} else {
			node->service_demands[i]= node->node_visits_per_class[i] * service_time;
			node->utilization_factors[i]= rates[i] * node->service_demands[i];
			node->response_times[i]= node->service_demands[i] / (1- node->utilization_factors[i]);
			node->total_utilization_factor+=node->utilization_factors[i];
		}
	}
}

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
		data->lan_direction=LAN_DIR_IN_OUT;
		data->lan_in_service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->lan_out_service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->lan_in_utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->lan_out_utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->lan_in_response_times=calloc(NUM_CLASSES,sizeof(double));
		data->lan_out_response_times=calloc(NUM_CLASSES,sizeof(double));
	}else{
		data->lan_direction=LAN_DIR_IGNORE;
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
			snprintf(data->type,sizeof(char)*128,"%d",((lp_infos*)getInfo(top,node_id))->sensor_type);
			break;
		case ACTUATOR:
			snprintf(data->name,sizeof(char)*128,"Actuator");
			data->type=malloc(sizeof(char)*128);
			snprintf(data->type,sizeof(char)*128,"%d",((lp_infos*)getInfo(top,node_id))->actuator_type);
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

void graph_visit(node_data* data,int visit_type,FILE* out){
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
	if(visit_type==GRAPH_COMPUTE_COMMANDS && info->node_type!=WAN){
		//in the second visit we need to compute the rates for command messages before reaching the leaves
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
	if(visit_type==GRAPH_PRINT_DATA){
		print_results(data,out);
	}
}

int main(int argc, char** argv){
	//get topology from file
	probs p;
	char* topology_path="test.txt";
	FILE *out=fopen("results.tex","w");
	topology * genTop = getTopology(topology_path);
	fprintf(out,"\\section{Computed Formulas Example}\n The following data is computed from the topology stored in file %s.\\\\",topology_path);
	node_data* central=malloc(sizeof(node_data));
	init_node_data(central,find_central(genTop->topArr,genTop->total_nodes),NULL,genTop);
	//1st visit, to compute rates
	graph_visit(central,GRAPH_COMPUTE_RATES,out);
	//2nd visit, to compute commands
	graph_visit(central,GRAPH_COMPUTE_COMMANDS,out);
	//3rd visit, to print results
	graph_visit(central,GRAPH_PRINT_DATA,out);
	return 0;
}
