/** \file formulas.c
 * \brief A "simple" program to compute the queuing model parameters, given a topology file.
 * The program will write the computed parameters on a LaTeX file called results.tex, which is a complete LaTeX document.
 * required LaTeX packages: `booktabs`, `float`
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include"../utils/partop/header.h"
#include "../utils/priority_scheduler/priority_scheduler_datatypes.h"
#include"quick_sort.h"

/// The visit type of the topology graph, since we need to visit it more than once.
typedef enum _graph_visit_type {
	GRAPH_COMPUTE_RATES=0, ///< This visit will compute all the paramters for telemetry, transition and batch send messages
	GRAPH_COMPUTE_COMMANDS, ///< This visit will compute the parameters for command messages.
	GRAPH_PRINT_DATA, ///< This visit will print the computed parameters on the output file and free memory.
	GRAPH_CLEANUP,
	NUM_GRAPH_VISITS ///< total number of visits __KEEP THIS AT LAST POSITION__.
} graph_visit_type;

///Used to avoid confusion while indexing the results array given by the ::compute_data function.
typedef enum _data_classes {
	CLASS_TELEMETRY=0, ///< telemetry messages.
	CLASS_TRANSITION, ///< transition messages.
	CLASS_COMMAND, ///< command messages.
	CLASS_BATCH, ///< batch send messages.
	CLASS_REPLY, ///< reply to transition messages
	NUM_CLASSES
} data_classes;

/// The number of visits that a message class can perform on a queue.
typedef enum _node_visits_per_message{
	NODE_NO_VISIT=0,
	NODE_SINGLE_VISIT,
	NODE_DOUBLE_VISIT
} node_visits_per_message;

///Paramters used to indicate that the current node is a LAN which has been split in an input queue and an output queue.
typedef enum _node_splitting_classes {
	NODE_NOT_SPLIT=-1, ///< Not split, treat it as a single queue.
	NODE_SPLIT_IN_OUT, ///< Node split in two queues.
	NODE_SPLIT_IN, ///< Node split in two queues but consider only the input queue.
	NODE_SPLIT_OUT ///< Node split in two queues but consider only output queue.
} node_splitting_classes;

///Enum used to indicate if the node has a storage system
typedef enum _node_storage_classes{
	NODE_NO_STORAGE=0, ///< Nodes with no storage
	NODE_SIMPLE_STORAGE ///< Nodes with a simple storage (a single queue)
}node_storage_classes;

/// Struct used to hold the parameters computed for each element of the topology.
typedef struct _node_data{
	//we use the input_rates array for the lan in and the output_rates array for the lan out when the node is a lan
	int num_cores;
	double *input_rates,*service_demands,*utilization_factors, *num_mean_events_in_queue,*output_rates, total_utilization_factor,*service_times; ///< Parameters to be computed
	double *response_times_A; ///< Response time for class j computed with D_j/(1-U) with j in ::NUM_CLASSES.
	double *response_times_B; ///< Response time for class j computed with D_i + SUM_(N_j*D_j) with i,j in ::NUM_CLASSES.
	Element_topology* top; ///< The ::topology*.
	data_classes classes; ///< The number of classes of messages (usually this is equal to ::NUM_CLASSES).
	int* node_visits_per_class; ///< The visits that each messages does on the current node, ordered by ::data_classes
	int node_id; ///< The id of the current node in the topology.
	int num_childrens; ///< The number of below connected nodes.
	char* type; ///< The name of the node (this usually matches to the node's role, e.g. sensor,actuator,local, central and regional).
	char* node_type; ///< If the node is an actuator / sensor then it's type will be here.
	struct _node_data** childrens; ///< The array of ids of the below connected nodes.
	struct _node_data* father; ///< The above connected node.
	//the parameters below make sense only for lans
	node_splitting_classes node_split_status; ///this parameter makes sense only for lan/split elements, if its value is not ::NODE_NOT_SPLIT then we need to use the below double* arrays instead of the ones above, rates excluded.
	double *input_service_times,*input_service_demands,*input_utilization_factors,*input_response_times_A, input_total_utilization_factor, *input_num_mean_events_in_queue, *input_response_times_B; ///< Input parameters.
	double output_total_utilization_factor,*output_service_times,*output_service_demands,*output_utilization_factors,*output_response_times_A, *output_num_mean_events_in_queue, *output_response_times_B; ///< output parameters.
	int *input_node_visits_per_class,*output_node_visits_per_class; ///< node visits are also split.
	//parameters below make sense if the mode has a storage system
	node_storage_classes node_storage_state;
	double* storage_service_times, *storage_service_demands, *storage_response_times_A, *storage_utilization_factors, storage_total_utilization_factor,*storage_input_rates, *storage_response_times_B, *storage_num_mean_events_in_queue;
	int *storage_visits_per_class;
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
void print_results_in_table(char* elem,int node_id,char* table_type,char* table_header,double* results,int num_results,FILE* out){
	int i;
	//we print the table header
	print_table_header(num_results,table_header,out);
	//we print the row of the table (see https://en.wikipedia.org/wiki/Printf_format_string for info on the format)
	fprintf(out,"$%f$",results[0]);
	for(i=1;i<num_results-1;i++){
		fprintf(out,"& $%f$",results[i]);
	}
	fprintf(out,"& $%f$ \\\\\n",results[num_results-1]);
	fprintf(out,"\\bottomrule\n\\end{tabular}\n\\caption{%s %d %s}\n\\label{tab:%s-%d}\n\\end{table}\n\n",elem,node_id,table_type,elem,node_id);
}

void print_results_in_table_int(char* elem,int node_id,char* table_type,char* table_header,int* results,int num_results,FILE* out){
	int i;
	//we print the table header
	print_table_header(num_results,table_header,out);
	//we print the row of the table (see https://en.wikipedia.org/wiki/Printf_format_string for info on the format)
	fprintf(out,"$%d$",results[0]);
	for(i=1;i<num_results-1;i++){
		fprintf(out,"& $%d$",results[i]);
	}
	fprintf(out,"& $%d$ \\\\\n",results[num_results-1]);
	fprintf(out,"\\bottomrule\n\\end{tabular}\n\\caption{%s %d %s}\n\\label{tab:%s-%d}\n\\end{table}\n\n",elem,node_id,table_type,elem,node_id);
}

/** \brief Prints the parameters of the specified node in several tables.
 * \param[in] node The node of which we want to print the parameters.
 * \param[in] out The file wher we want to print.
 */
void print_results(node_data* node,FILE* out){
	char* table_header=malloc(sizeof(char)*512);
	char* table_type=malloc(sizeof(char)*128);
	if(getType(node->top)!=WAN && getType(node->top)!=SENSOR){
		fprintf(out,"\\subsection{%s %d}\n",node->type,node->node_id);
		fprintf(out,"Type: %s\\\\\n",node->node_type);
	}
	if(getType(node->top)!=WAN && (getType(node->top)!=LAN && node->node_split_status==NODE_NOT_SPLIT) && getType(node->top)!=SENSOR){
		//we print the rates table
		snprintf(table_type,sizeof(char)*128,"rates");
		//snprintf(table_type,sizeof(char)*128,"input rates");
		snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & $\\lambda_e$ & $\\lambda_c$ & $\\lambda_b$\\\\\n\\midrule\n");
		print_results_in_table(node->type,node->node_id,table_type,table_header,node->input_rates,node->classes,out);
		//	snprintf(table_type,sizeof(char)*128,"output rates");
		//	print_results_in_table(node->name,node->node_id,table_type,table_header,node->output_rates,node->classes,out);
		//we print the visits for the node classes
		snprintf(table_type,sizeof(char)*128,"visits per message class");
		snprintf(table_header,sizeof(char)*512,"$V_t$ & $V_e$ & $V_c$ & $V_b$\\\\\n\\midrule\n");
		print_results_in_table_int(node->type,node->node_id,table_type,table_header,node->node_visits_per_class,node->classes,out);
		//we print the service times
		snprintf(table_type,sizeof(char)*128,"service times");
		snprintf(table_header,sizeof(char)*512,"$S_t$ & $S_e$ & $S_c$ & $S_b$\\\\\n\\midrule\n");
		print_results_in_table(node->type,node->node_id,table_type,table_header,node->service_times,node->classes,out);
		//we print the service demand table
		snprintf(table_type,sizeof(char)*128,"service demands");
		snprintf(table_header,sizeof(char)*512,"$D_t$ & $D_e$ & $D_c$ & $D_b$\\\\\n\\midrule\n");
		print_results_in_table(node->type,node->node_id,table_type,table_header,node->service_demands,node->classes,out);
		//we print the utilization factors
		snprintf(table_type,sizeof(char)*128,"utilization factors");
		snprintf(table_header,sizeof(char)*512,"$U_t$ & $U_e$ & $U_c$ & $U_b$\\\\\n\\midrule\n");
		print_results_in_table(node->type,node->node_id,table_type,table_header,node->utilization_factors,node->classes,out);
		fprintf(out,"Total utilization factor: %f\\\\\n",node->total_utilization_factor);
		//we print the response times
		snprintf(table_type,sizeof(char)*128,"response times");
		snprintf(table_header,sizeof(char)*512,"$R_t$ & $R_e$ & $R_c$ & $R_b$\\\\\n\\midrule\n");
		print_results_in_table(node->type,node->node_id,table_type,table_header,node->response_times_A,node->classes,out);
	} else{

		if(getType(node->top)==WAN ){
			//	fprintf(out,"Delay: $%f$",((lp_infos*)getInfo(node->top,node->node_id))->delay);
		} else{
			if(getType(node->top)==SENSOR){
				//	snprintf(table_type,sizeof(char)*128,"output rates");
				//	snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & $\\lambda_e$ & $\\lambda_c$ & $\\lambda_b$\\\\\n\\midrule\n");
				//	print_results_in_table(node->name,node->node_id,table_type,table_header,node->output_rates,node->classes,out);
			} else {
				if(getType(node->top)){
					fprintf(out,"\\subsubsection{LAN in}");
				}
				snprintf(table_type,sizeof(char)*128,"Lan in rates");
				snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & $\\lambda_e$ & $\\lambda_c$ & $\\lambda_b$\\\\\n\\midrule\n");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->input_rates,node->classes,out);
				//we print the visits for the node classes
				snprintf(table_type,sizeof(char)*128,"Lan in visits per message class");
				snprintf(table_header,sizeof(char)*512,"$V_t$ & $V_e$ & $V_c$ & $V_b$\\\\\n\\midrule\n");
				print_results_in_table_int(node->type,node->node_id,table_type,table_header,node->input_node_visits_per_class,node->classes,out);
				//we print the service times
				snprintf(table_type,sizeof(char)*128,"LAN in service times");
				snprintf(table_header,sizeof(char)*512,"$S_t$ & $S_e$ & $S_c$ & $S_b$\\\\\n\\midrule\n");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->input_service_times,node->classes,out);
				//we print the service demand tables
				snprintf(table_type,sizeof(char)*128,"Lan in service demands");
				snprintf(table_header,sizeof(char)*512,"$D_t$ & $D_e$ & $D_c$ & $D_b$\\\\\n\\midrule\n");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->input_service_demands,node->classes,out);

				//we print the utilization factors
				snprintf(table_type,sizeof(char)*128,"Lan in utilization factors");
				snprintf(table_header,sizeof(char)*512,"$U_t$ & $U_e$ & $U_c$ & $U_b$\\\\\n\\midrule\n");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->input_utilization_factors,node->classes,out);
				fprintf(out,"Lan in Total utilization factor: $%f$\\\\\n",node->input_total_utilization_factor);
				//we print the response times
				snprintf(table_type,sizeof(char)*128,"Lan in response times");
				snprintf(table_header,sizeof(char)*512,"$R_t$ & $R_e$ & $R_c$ & $R_b$\\\\\n\\midrule\n");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->input_response_times_A,node->classes,out);
				if(getType(node->top)){
					fprintf(out,"\\subsubsection{LAN out}");
				}
				//LAN out
				snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & $\\lambda_e$ & $\\lambda_c$ & $\\lambda_b$\\\\\n\\midrule\n");
				snprintf(table_type,sizeof(char)*128,"Lan out rates");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->output_rates,node->classes,out);
				snprintf(table_type,sizeof(char)*128,"LAN out visits per message class");
				snprintf(table_header,sizeof(char)*512,"$V_t$ & $V_e$ & $V_c$ & $V_b$\\\\\n\\midrule\n");
				print_results_in_table_int(node->type,node->node_id,table_type,table_header,node->output_node_visits_per_class,node->classes,out);
				//we print the service times
				snprintf(table_type,sizeof(char)*128,"Lan out service times");
				snprintf(table_header,sizeof(char)*512,"$S_t$ & $S_e$ & $S_c$ & $S_b$\\\\\n\\midrule\n");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->output_service_times,node->classes,out);
				snprintf(table_header,sizeof(char)*512,"$D_t$ & $D_e$ & $D_c$ & $D_b$\\\\\n\\midrule\n");
				snprintf(table_type,sizeof(char)*128,"Lan out service demands");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->output_service_demands,node->classes,out);
				snprintf(table_header,sizeof(char)*512,"$U_t$ & $U_e$ & $U_c$ & $U_b$\\\\\n\\midrule\n");
				snprintf(table_type,sizeof(char)*128,"Lan out utilization factors");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->output_utilization_factors,node->classes,out);
				fprintf(out,"Lan out Total utilization factor: $%f$\\\\\n",node->output_total_utilization_factor);
				snprintf(table_type,sizeof(char)*128,"Lan out response times");
				snprintf(table_header,sizeof(char)*512,"$R_t$ & $R_e$ & $R_c$ & $R_b$\\\\\n\\midrule\n");
				print_results_in_table(node->type,node->node_id,table_type,table_header,node->output_response_times_A,node->classes,out);
			}
		}
	}
	if(getType(node->top)==NODE && getNodeType(node->top)==CENTRAL){
		fprintf(out,"\\subsubsection{Central node storage system}\n");
		snprintf(table_type,sizeof(char)*128,"rates");
		//snprintf(table_type,sizeof(char)*128,"input rates");
		snprintf(table_header,sizeof(char)*512,"$\\lambda_t$ & $\\lambda_e$ & $\\lambda_c$ & $\\lambda_b$\\\\\n\\midrule\n");
		print_results_in_table("Central node storage system",node->node_id,table_type,table_header,node->storage_input_rates,node->classes,out);
		//	snprintf(table_type,sizeof(char)*128,"output rates");
		//	print_results_in_table(node->name,node->node_id,table_type,table_header,node->output_rates,node->classes,out);
		//we print the visits for the node classes
		snprintf(table_type,sizeof(char)*128,"visits per message class");
		snprintf(table_header,sizeof(char)*512,"$V_t$ & $V_e$ & $V_c$ & $V_b$\\\\\n\\midrule\n");
		print_results_in_table_int("Central node storage system ",node->node_id,table_type,table_header,node->storage_visits_per_class,node->classes,out);
		//we print the service times
		snprintf(table_type,sizeof(char)*128,"service times");
		snprintf(table_header,sizeof(char)*512,"$S_t$ & $S_e$ & $S_c$ & $S_b$\\\\\n\\midrule\n");
		print_results_in_table("Central node storage system ",node->node_id,table_type,table_header,node->storage_service_times,node->classes,out);
		//we print the service demand table
		snprintf(table_type,sizeof(char)*128,"service demands");
		snprintf(table_header,sizeof(char)*512,"$D_t$ & $D_e$ & $D_c$ & $D_b$\\\\\n\\midrule\n");
		print_results_in_table("Central node storage system ",node->node_id,table_type,table_header,node->storage_service_demands,node->classes,out);
		//we print the utilization factors
		snprintf(table_type,sizeof(char)*128,"utilization factors");
		snprintf(table_header,sizeof(char)*512,"$U_t$ & $U_e$ & $U_c$ & $U_b$\\\\\n\\midrule\n");
		print_results_in_table("Central node storage system ",node->node_id,table_type,table_header,node->storage_utilization_factors,node->classes,out);
		fprintf(out,"Total utilization factor: %f\\\\\n",node->storage_total_utilization_factor);
		//we print the response times
		snprintf(table_type,sizeof(char)*128,"response times");
		snprintf(table_header,sizeof(char)*512,"$R_t$ & $R_e$ & $R_c$ & $R_b$\\\\\n\\midrule\n");
		print_results_in_table("Central node storage system ",node->node_id,table_type,table_header,node->storage_response_times_A,node->classes,out);
	}
	fflush(out);
	free(table_header);
	free(table_type);
}

/** \brief Prints in JSON format the parameters for all the messages classes
 * \param[in] node The node which we want to print.
 * \param[in] type The type of the node (as a element in the queue network)
 * \param[in] split_class If the node is not split this parameter should be ::NODE_NOT_SPLIT if we want to print information on one of the two queues (not both at the same invocation) we can use ::NODE_SPLIT_IN for the input queue or ::NODE_SPLIT_OUT for the output queue.
 * \param[in] storage_class If the node has a storage, the corresponding class will print paramters only for that storage class.
 * \param[in] out The file on which we want to print.
 *
 * Calling the function with split_class=::NODE_SPLIT_IN_OUT is not supported.
 * The function will print the node parameters if split_class=::NODE_NOT_SPLIT and storage_class=::NODE_NO_STORAGE.
 * If the node has a storage or is split then the corresponding information can be printed using storege_class nad split_class printing one corresponding for each invocation.
 * (eg. split node with simple storage need to call this function 3 times, one with split_class=::NODE_SPLIT_IN and storage_class=::NODE_NO_STORAGE, one with split_class=::NODE_SPLIT_OUT and storage_class=::NODE_NO_STORAGE and one with split_class=::NODE_NOT_SPLIT and storage_class=::NODE_SIMPLE_STORAGE).
 * A wrong usage of these params means that the function will print nothing.
 */
void print_parameters(node_data* node,node_splitting_classes split_class,node_storage_classes storage_class,FILE* out){
	data_classes class;
	double *lambda_ins,*service_demands,*utilization_factors,*response_times_A,*response_times_B,*num_mean_events_in_queue,*service_times;
	int found_params=0,*number_visits;
	//we choose which group of parameters to print
	if(split_class==NODE_SPLIT_IN){
		found_params=1;
		lambda_ins=node->input_rates;
		service_demands=node->input_service_demands;
		utilization_factors=node->input_utilization_factors;
		response_times_A=node->input_response_times_A;
		response_times_B=node->input_response_times_B;
		service_times=node->input_service_times;
		number_visits=node->input_node_visits_per_class;
		num_mean_events_in_queue=node->input_num_mean_events_in_queue;
	}
	if(split_class==NODE_SPLIT_OUT){
		found_params=1;
		lambda_ins=node->output_rates;
		service_demands=node->output_service_demands;
		utilization_factors=node->output_utilization_factors;
		response_times_A=node->output_response_times_A;
		response_times_B=node->output_response_times_B;
		service_times=node->output_service_times;
		number_visits=node->output_node_visits_per_class;
		num_mean_events_in_queue=node->output_num_mean_events_in_queue;
	}
	if(storage_class==NODE_SIMPLE_STORAGE){
		found_params=1;
		lambda_ins=node->storage_input_rates;
		service_demands=node->storage_service_demands;
		utilization_factors=node->storage_utilization_factors;
		response_times_A=node->storage_response_times_A;
		response_times_B=node->storage_response_times_B;
		num_mean_events_in_queue=node->storage_num_mean_events_in_queue;
		service_times=node->storage_service_times;
		number_visits=node->storage_visits_per_class;
	}
	if(storage_class==NODE_NO_STORAGE && split_class==NODE_NOT_SPLIT){
		found_params=1;
		lambda_ins=node->input_rates;
		service_demands=node->service_demands;
		utilization_factors=node->utilization_factors;
		response_times_A=node->response_times_A;
		response_times_B=node->response_times_B;
		service_times=node->service_times;
		number_visits=node->node_visits_per_class;
		num_mean_events_in_queue=node->num_mean_events_in_queue;
	}
	//we print parameters for each class
	for(class=CLASS_TELEMETRY;found_params==1 && class<CLASS_REPLY;class++){
		//print the class JSON object
		switch(class){
			case CLASS_TELEMETRY:
				fprintf(out,"\"telemetry\" : {");
				if(getType(node->top)==NODE){
					fprintf(out,"\"aggregation_rate\" : %d,",getAggregationRate(node->top)[TELEMETRY]);
				}
				break;
			case CLASS_TRANSITION:
				fprintf(out,"\"transition\" : {");
				if(getType(node->top)==NODE){
					fprintf(out,"\"aggregation_rate\" : %d,",getAggregationRate(node->top)[TRANSITION]);
				}
				break;
			case CLASS_COMMAND:
				fprintf(out,"\"command\" : {");
				if(getType(node->top)==NODE){
					fprintf(out,"\"aggregation_rate\" : %d,",getAggregationRate(node->top)[COMMAND]);
				}
				break;
			case CLASS_BATCH:
				fprintf(out,"\"batch\" : {");
				if(getType(node->top)==NODE){
					fprintf(out,"\"aggregation_rate\" : %d,",getAggregationRate(node->top)[BATCH_DATA]);
				}
				break;
			default:
				break;
		}
		//print parameters for that class
		fprintf(out,"\"lambda_in\" : %f,",lambda_ins[class]);
		fprintf(out,"\"service_demand\" : %f,",service_demands[class]);
		fprintf(out,"\"utilization_factor\" : %f,",utilization_factors[class]);
		fprintf(out,"\"response_time_a\" : %f,",response_times_A[class]);
		fprintf(out,"\"response_time\" : %f,",response_times_B[class]);
		fprintf(out,"\"number_mean_queue\" : %f,",num_mean_events_in_queue[class]);
		fprintf(out,"\"service_time\" : %f,",service_times[class]);
		fprintf(out,"\"number_of_visits\" : %d",number_visits[class]);
		fprintf(out,"}");
		if(class+1<CLASS_REPLY){
			fprintf(out,",");
		}
	}
}

/** \brief Prints the data of a node in json format.
 * \param[in] node The node we wish to print data.
 * \param[in] out The file where we want to print.
 */
void print_json(node_data* node,FILE* out){

	fprintf(out,"{");
	//we print the node id
	fprintf(out,"\"id\" : %d,",node->node_id);
	//we print node type
	fprintf(out,"\"type\" : \"%s\",",node->type);
	int father_id=-1;
	node_data *tmp=node;
	if(node->father!=NULL){
		father_id=node->father->node_id;
		while(tmp!=NULL && getType(tmp->father->top)==WAN){
			tmp=tmp->father;
			father_id=tmp->father->node_id;
		}
	}
	fprintf(out,"\"father\" : %d,",father_id);

	fprintf(out,"\"childrens\":[");
	int i,j;
	//we avoid printing sensors and wans as childrens since they do not hold any useful data (was are delays and sensors cannot be separated from lans)
	for(i=0;i<node->num_childrens;i++){
		tmp=node->childrens[i];
		if(tmp!=NULL && getType(tmp->top)==WAN){
			for(j=0;j<tmp->num_childrens;j++){
				if(tmp!=NULL && getType(tmp->childrens[j]->top)!=SENSOR){
					fprintf(out,"%d",tmp->childrens[j]->node_id);
					if(j<tmp->num_childrens-1){
						fprintf(out,",");
					}
				}
			}
			if(i<node->num_childrens-1){
				fprintf(out,",");
			}
		}else {
			if(tmp!=NULL && getType(tmp->top)!=SENSOR){
				fprintf(out,"%d",tmp->node_id);
				if(i<node->num_childrens-1){
					fprintf(out,",");
				}
			}
		}
	}
	fprintf(out,"],");
	//we now print the parameters accordingly with the node type
	switch(getType(node->top)){
		case SENSOR:
			break;
		case ACTUATOR:
		case NODE:
			//we print node_type only for nodes
			if(getType(node->top)==NODE){
				fprintf(out,"\"node_type\" : \"%s\",",node->node_type);
				fprintf(out,"\"num_cores\" : %d,",node->num_cores);
			}
			fprintf(out,"\"parameters\" : {");
			print_parameters(node,NODE_NOT_SPLIT,NODE_NO_STORAGE,out);
			fprintf(out,"}");
			//central node has also storage parameters
			if(getType(node->top)==NODE && getNodeType(node->top)==CENTRAL){
				fprintf(out,",\"storage\" : {");
				print_parameters(node,NODE_NOT_SPLIT,NODE_SIMPLE_STORAGE,out);
				fprintf(out,"}");
			}
			break;
		case LAN:
			fprintf(out,"\"lan_in\" : {");
			print_parameters(node,NODE_SPLIT_IN,NODE_NO_STORAGE,out);
			fprintf(out,"},");
			fprintf(out,"\"lan_out\" : {");
			print_parameters(node,NODE_SPLIT_OUT,NODE_NO_STORAGE,out);
			fprintf(out,"}");
	}
	fprintf(out,"}");
	fflush(out);
}


/** \brief Locate the central node.
 * \param[in] array The ::topArray struct which contains all the nodes of the topology.
 * \param[in] elements The lenght of the ::topArray struct (usually ::topology->num_nodes).
 * \returns The id of the central node (supposed to be unique).
 */
int find_central(Element_topology ** array,int elements){
	int i,central=-1;
	for(i=0;i<elements && central==-1;i++){
		int type = getType(array[i]);
		if(type==NODE){
			int node_type =getNodeType(array[i]);
			if(node_type == CENTRAL){
				central=i;
			}
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
int* find_nodes_to_visit(Element_topology* elTop,int* num_lowers){
	int i,j,k,duplicate;
	int num_lans=getNumberLANS(elTop);
	int num_nodes=getNumberLower(elTop);
	int *nodes=getLowers(elTop);
	int *lans=getLANS(elTop);
	// the array below is used to visit the lower nodes will be terminated by -1
	int *lowers=NULL;
	//when both the number of node and lans is not 0 we need to detect duplicates before exploring the subtrees to avoid doing the same computation multiple times
	lowers=malloc(sizeof(int)*(num_lans+num_nodes+1));
	memset(lowers,-1,sizeof(int)*(num_lans+num_nodes+1));
	if(num_lans>0 && num_nodes>0){

		//we sort the lower nodes and the lans array
		quicksort(nodes,getNumberLower(elTop));
		quicksort(lans,getNumberLANS(elTop));
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
		*num_lowers=k;
	} else{
		if(num_lans==0){
			memcpy(lowers,nodes,sizeof(int)*num_nodes);
			*num_lowers=num_nodes;
		} else {
			if(num_nodes>0){
				memcpy(lowers,nodes,sizeof(int)*num_nodes);
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
void compute_data(node_data* node,graph_visit_type visit_type,double * probOfActuators, int numberOfActTypes){
	double total_command_dest_weight_num=0,branch_command_dest_weight_num=0,*childrens_total_rate=NULL,rate_transition_sensors_below=0,rate_telemetry_sensors_below=0,rate_transition_actuators_below=0,*rates;
	node_data* father_node=NULL;
	int i,j,father_type;
	data_classes class;
	assert(visit_type==GRAPH_COMPUTE_COMMANDS || visit_type==GRAPH_COMPUTE_RATES);
	int elem_type=getType(node->top);
	int node_type = getNodeType(node->top);
	//we get the node information struct
	// if we are not the a sensor, a wan or the central node  we need to compute the number of actuators that our upper node (WAN ecluded) can reach with a command message, weighted by their probability to be reached
	if((elem_type!=NODE || node_type!=CENTRAL) && elem_type!=SENSOR && elem_type!=WAN ){
		switch(elem_type){
			case NODE:
				if(node_type==LOCAL){
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
		while(getType(father_node->top)!=NODE || getNodeType(father_node->top)!=father_type){
			father_node=father_node->father;
		}
		//compute the weighted number of actuators below our father
		for(i=0;i<numberOfActTypes;i++){
			total_command_dest_weight_num+=getActuatorTypesBelowList(father_node->top)[i]*probOfActuators[i];
		}
		//if we are not an actuator we need to compute the weighted number of actuators below us
		if(elem_type!=ACTUATOR){
			for(i=0;i<numberOfActTypes;i++){
				branch_command_dest_weight_num+=getActuatorTypesBelowList(node->top)[i]*probOfActuators[i];
			}
		}
		// if the node is a split lan and we are an actuator, we need to get the rates from the lan in.
		if(getType(father_node->top)==LAN && father_node->node_split_status==NODE_SPLIT_IN_OUT && getNodeType(node->top)==ACTUATOR){
			rates=father_node->input_rates;
		} else {
			rates=father_node->output_rates;
		}
	}
	// we have different rates formula for each element type
	switch(getType(node->top)){
		case SENSOR:
			if(visit_type==GRAPH_COMPUTE_RATES){
				//sensors can send only telemetry and transition messages
				node->output_rates[CLASS_TELEMETRY]=getSensorRates(node->top)[TELEMETRY];
				node->output_rates[CLASS_TRANSITION]=getSensorRates(node->top)[TRANSITION];
			}
			break;
		case ACTUATOR:
			//actuators send transitions and receive commands
			if(visit_type==GRAPH_COMPUTE_RATES){
				node->output_rates[CLASS_TRANSITION]=getRateTransition(node->top);
			}
			if(visit_type==GRAPH_COMPUTE_COMMANDS){
				node->input_rates[CLASS_COMMAND]= rates[CLASS_COMMAND] * ( probOfActuators[getActuatorType(node->top)] / total_command_dest_weight_num);
			}
			node->service_times[CLASS_COMMAND]=getServiceTimeCommand(node->top);
			break;
		case LAN:
			//Lan rely messages from local nodes to sensors /actuators and vice versa, but are split in two queues
			if(visit_type==GRAPH_COMPUTE_RATES){
				for(i=0;i<node->num_childrens;i++){
					if(getType(node->childrens[i]->top)==ACTUATOR){
						rate_transition_actuators_below+=getRateTransition(node->childrens[i]->top);
					}
					else if(getType(node->childrens[i]->top)==SENSOR){
						rate_telemetry_sensors_below+=getSensorRates(node->childrens[i]->top)[TELEMETRY];
						rate_transition_sensors_below+=getSensorRates(node->childrens[i]->top)[TRANSITION];
					}
				}
				node->output_rates[CLASS_TELEMETRY]=rate_telemetry_sensors_below;
				node->output_rates[CLASS_TRANSITION]=rate_transition_sensors_below+rate_transition_actuators_below;
			}
			if(visit_type==GRAPH_COMPUTE_COMMANDS){
				node->input_rates[CLASS_COMMAND]=rates[CLASS_COMMAND] * branch_command_dest_weight_num / total_command_dest_weight_num;
			}
			if(node->node_split_status==NODE_SPLIT_IN_OUT){
				node->input_service_times[CLASS_COMMAND]=getLANsINserviceTimesByType(node->top)[COMMAND];
				node->output_service_times[CLASS_TELEMETRY]=getLANsOUTserviceTimesByType(node->top)[TELEMETRY];
				node->output_service_times[CLASS_TRANSITION]=getLANsOUTserviceTimesByType(node->top)[TRANSITION];
			}
			break;
		case NODE:
			//node rely telemetry and trasition message to their upper node and commands to their below node
			if(visit_type==GRAPH_COMPUTE_RATES){
				childrens_total_rate=calloc(NUM_CLASSES,sizeof(double));
				//we need to sum the output rates of all our childrens
				for(j=CLASS_TELEMETRY;j<NUM_CLASSES;j++){
					for(i=0;i<node->num_childrens;i++){
						childrens_total_rate[j]+=node->childrens[i]->output_rates[j];
					}
				}
				node->input_rates[CLASS_TELEMETRY]=childrens_total_rate[CLASS_TELEMETRY];
				node->output_rates[CLASS_TELEMETRY]=childrens_total_rate[CLASS_TELEMETRY]/getAggregationRate(node->top)[TELEMETRY];
				node->input_rates[CLASS_TRANSITION]=childrens_total_rate[CLASS_TRANSITION];
                // only the transitions which are not followed by a command are sent to the upper level
				node->output_rates[CLASS_TRANSITION]=node->input_rates[CLASS_TRANSITION]*(1-getProbCommandResponse(node->top));
				//generated messages are not counted in the queue
				node->input_rates[CLASS_BATCH]=childrens_total_rate[CLASS_BATCH];
				node->output_rates[CLASS_BATCH]=(node->input_rates[CLASS_BATCH]+getProbCommandResponse(node->top)*node->input_rates[CLASS_TRANSITION])/getAggregationRate(node->top)[BATCH_DATA];
				free(childrens_total_rate);
			}
			if(visit_type==GRAPH_COMPUTE_COMMANDS){
				if(getNodeType(node->top)!=CENTRAL){
					//generated messages are not considered in the queue
					node->input_rates[CLASS_COMMAND]=rates[CLASS_COMMAND] * branch_command_dest_weight_num / total_command_dest_weight_num; // generated messages not counted in the queue
					node->output_rates[CLASS_COMMAND]=node->input_rates[CLASS_COMMAND]+getProbCommandResponse(node->top)*node->input_rates[CLASS_TRANSITION];
				} else{
					node->output_rates[CLASS_COMMAND]=getProbCommandResponse(node->top)*node->input_rates[CLASS_TRANSITION];
				}
				//we compute parameters for the storage included in the node
				if(node->node_storage_state==NODE_SIMPLE_STORAGE){
					node->storage_service_times[CLASS_TELEMETRY]=getDiskServices(node->top)[TELEMETRY];
					node->storage_service_times[CLASS_TRANSITION]=getDiskServices(node->top)[TRANSITION];
					node->storage_service_times[CLASS_BATCH]=getDiskServices(node->top)[BATCH_DATA];
					//we consider the node output rates as input rates to the storage system command messages are not logged, since they are aggregated in batch messages
					node->storage_input_rates[CLASS_TELEMETRY]=node->output_rates[CLASS_TELEMETRY];
					node->storage_input_rates[CLASS_TRANSITION]=node->output_rates[CLASS_TRANSITION];
					//we log on disk both the messages received and the batch messages originated from command messages and we aggregate both
					node->storage_input_rates[CLASS_BATCH]=node->output_rates[CLASS_BATCH];
				}
				node->service_times[CLASS_TELEMETRY]=getServiceTimesNodes(node->top)[TELEMETRY];
				if(getNodeType(node->top)!=CENTRAL){
					node->service_times[CLASS_TRANSITION]=getServiceTimesNodes(node->top)[TRANSITION];
					node->service_times[CLASS_REPLY]=getServiceTimesNodes(node->top)[REPLY];
				}
				else{
					node->service_times[CLASS_TRANSITION]=getServiceTimesNodes(node->top)[TRANSITION];
				}
				node->service_times[CLASS_COMMAND]=getServiceTimesNodes(node->top)[COMMAND];
				node->service_times[CLASS_BATCH]=getServiceTimesNodes(node->top)[BATCH_DATA];
			}
			break;
			//the default case handles all the node which are modeled as delays
			///For nodes modeled as delays and split queues we suppose that the output rates a directed towards the upper nodes, and the input rates are directed towards the lower nodes (as with the splitted lan case)
		default:
			childrens_total_rate=calloc(NUM_CLASSES,sizeof(double));
			//we need to sum the output rates of all our childrens
			for(j=CLASS_TELEMETRY;j<NUM_CLASSES;j++){
				for(i=0;i<node->num_childrens;i++){
					childrens_total_rate[j]+=node->childrens[i]->output_rates[j];
				}
				node->output_rates[j]=childrens_total_rate[j];
				if(father_node!=NULL){
					node->input_rates[j]=father_node->output_rates[j];
				}
			}
			free(childrens_total_rate);
	}
	//computing service time, utilization factor for each message class, doing so we consider only the input rates of each node since computation for the generation of the messages is included in the service time.
	double input_weighted_service_demand=0.0,output_weighted_service_demand=0.0, weighted_service_demand=0.0,storage_weighted_service_demand=0.0;
	for(class=CLASS_TELEMETRY;class<NUM_CLASSES && visit_type==GRAPH_COMPUTE_COMMANDS;class++){
		//LAN are split in two queues, so we use different array to compute the queue parameters
		if(node->node_split_status==NODE_SPLIT_IN_OUT){
			//lan in
			if(node->input_rates[class]>0){
				node->input_service_demands[class]= node->input_node_visits_per_class[class] * node->input_service_times[class];
				node->input_utilization_factors[class]= node->input_rates[class] * node->input_service_demands[class] / node->num_cores;
				node->input_total_utilization_factor+=node->input_utilization_factors[class];
			}
			//lan out
			if(node->output_rates[class]>0){
				node->output_service_demands[class]= node->output_node_visits_per_class[class] * node->output_service_times[class];
				node->output_utilization_factors[class]= node->output_rates[class] * node->output_service_demands[class] / node->num_cores;
				node->output_total_utilization_factor+=node->output_utilization_factors[class];
			}
		} else {
			if(node->input_rates[class]>0){
				//all the other elements use only one queue, so their parameters can be generalized
				//however regional and local nodes need to consider the replies in their transition service demand
				if(class==CLASS_TRANSITION && getType(node->top)==NODE && getNodeType(node->top)!=CENTRAL){
					node->service_demands[CLASS_TRANSITION]= node->node_visits_per_class[class] * (node->service_times[CLASS_TRANSITION] + ((1-getProbCommandResponse(node->top))*node->service_times[CLASS_REPLY]));
				} else {
					node->service_demands[class]= node->node_visits_per_class[class] * node->service_times[class];
				}
				node->utilization_factors[class]= node->input_rates[class] * node->service_demands[class] / node->num_cores;
				node->total_utilization_factor+=node->utilization_factors[class];
			}

		}
		if(node->node_storage_state==NODE_SIMPLE_STORAGE && node->storage_input_rates[class]>0){
			//we assume that data make only one visit one the simple storage component
			node->storage_service_demands[class]= node->storage_visits_per_class[class] * node->storage_service_times[class];
			node->storage_utilization_factors[class]=node->storage_input_rates[class]* node->storage_service_demands[class];
			node->storage_total_utilization_factor+=node->storage_utilization_factors[class];
		}
	}
	// now we compute the response time A, since we need the total utilization factor
	if(visit_type==GRAPH_COMPUTE_COMMANDS){
		for(class=CLASS_TELEMETRY;class<NUM_CLASSES;class++){
			if(node->node_split_status==NODE_SPLIT_IN_OUT){
				if(node->input_rates[class]>0){
					node->input_response_times_A[class]= node->input_service_demands[class] / (1- node->input_total_utilization_factor);
					node->input_num_mean_events_in_queue[class]=node->input_utilization_factors[class] / (1- node->input_total_utilization_factor);
					input_weighted_service_demand+=node->input_num_mean_events_in_queue[class] * node->input_service_demands[class];
				}
				if(node->output_rates[class]>0){
					node->output_response_times_A[class]= node->output_service_demands[class] / (1- node->output_total_utilization_factor);
					node->output_num_mean_events_in_queue[class]=node->output_utilization_factors[class] / (1- node->output_total_utilization_factor);
					output_weighted_service_demand+=node->output_num_mean_events_in_queue[class] * node->output_service_demands[class];
				}
			} else {
				if(node->input_rates[class]>0){
					node->response_times_A[class]= node->service_demands[class] / (1 - node->total_utilization_factor);
					node->num_mean_events_in_queue[class]=node->utilization_factors[class] / (1- node->total_utilization_factor);
					weighted_service_demand+=node->num_mean_events_in_queue[class] * node->service_demands[class];
				}
			}
			if(node->node_storage_state==NODE_SIMPLE_STORAGE && node->storage_input_rates[class]>0){
				node->storage_response_times_A[class]=node->storage_service_demands[class] / (1-node->storage_total_utilization_factor);
				node->storage_num_mean_events_in_queue[class]=node->storage_utilization_factors[class] / (1- node->storage_total_utilization_factor);
				storage_weighted_service_demand+=node->storage_num_mean_events_in_queue[class] * node->storage_service_demands[class];
			}
		}
		for(class=CLASS_TELEMETRY;class<NUM_CLASSES;class++){
			if(node->node_split_status==NODE_SPLIT_IN_OUT){
				if(node->input_rates[class]>0){
					node->input_response_times_B[class]=input_weighted_service_demand + node->input_service_demands[class];
				}
				if(node->output_rates[class]>0){
					node->output_response_times_B[class]=output_weighted_service_demand + node->output_service_demands[class];
				}
			}else{
				if(node->input_rates[class]>0){
					node->response_times_B[class]=weighted_service_demand + node->service_demands[class];
				}
			}
			if(node->node_storage_state==NODE_SIMPLE_STORAGE && node->storage_input_rates[class]>0){
				node->storage_response_times_B[class]=storage_weighted_service_demand + node->storage_service_demands[class];
			}
		}
	}
}

/** \brief Initializes the ::node_data struct.
 * \param[in] data The structure to be initialized.
 * \param[in] node_id The node id.
 * \param[in] father The topology element that has the current node as children (in the dataflow).
 * \param[in] top The ::topology struct containing the general info.
 * The childrens array is not allocated here since we do not now yet how many childs we have
 */
void init_node_data(node_data *data,int node_id,node_data* father,Element_topology * elTop){
	memset(data,0,sizeof(node_data));
	data->node_id=node_id;
	data->father=father;
	data->top=elTop;
	data->classes=NUM_CLASSES;
	data->input_rates=calloc(NUM_CLASSES,sizeof(double));
	data->num_cores = 1;
	data->output_rates=calloc(NUM_CLASSES,sizeof(double));
	//transition messages have two visits only between nodes
	//we model a lan as a split queue
	int type = getType(elTop);
	if(type==LAN){
		data->node_split_status=NODE_SPLIT_IN_OUT;
		data->input_service_times=calloc(NUM_CLASSES,sizeof(double));
		data->output_service_times=calloc(NUM_CLASSES,sizeof(double));
		data->input_service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->output_service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->input_utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->output_utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->input_response_times_A=calloc(NUM_CLASSES,sizeof(double));
		data->output_response_times_A=calloc(NUM_CLASSES,sizeof(double));
		data->input_node_visits_per_class=calloc(NUM_CLASSES,sizeof(int));
		data->output_node_visits_per_class=calloc(NUM_CLASSES,sizeof(int));
		data->input_response_times_B=calloc(NUM_CLASSES,sizeof(double));
		data->output_response_times_B=calloc(NUM_CLASSES,sizeof(double));
		data->input_num_mean_events_in_queue=calloc(NUM_CLASSES,sizeof(double));
		data->output_num_mean_events_in_queue=calloc(NUM_CLASSES,sizeof(double));
	}else{
		data->service_times=calloc(NUM_CLASSES,sizeof(double));
		data->node_split_status=NODE_NOT_SPLIT;
		data->node_visits_per_class=calloc(NUM_CLASSES,sizeof(int));
		data->service_demands=calloc(NUM_CLASSES,sizeof(double));
		data->utilization_factors=calloc(NUM_CLASSES,sizeof(double));
		data->response_times_A=calloc(NUM_CLASSES,sizeof(double));
		data->response_times_B=calloc(NUM_CLASSES,sizeof(double));
		data->num_mean_events_in_queue=calloc(NUM_CLASSES,sizeof(double));
	}
	// in this implementation only the central node has storage
	if(type==NODE){
		int node_type = getNodeType(elTop);
		data->num_cores = getNumberOfCores(elTop);
	 	if(node_type == CENTRAL){
			data->node_storage_state=NODE_SIMPLE_STORAGE;
			data->storage_input_rates=calloc(NUM_CLASSES,sizeof(double));
			data->storage_response_times_A=calloc(NUM_CLASSES,sizeof(double));
			data->storage_service_demands=calloc(NUM_CLASSES,sizeof(double));
			data->storage_service_times=calloc(NUM_CLASSES,sizeof(double));
			data->storage_utilization_factors=calloc(NUM_CLASSES,sizeof(double));
			data->storage_response_times_B=calloc(NUM_CLASSES,sizeof(double));
			data->storage_num_mean_events_in_queue=calloc(NUM_CLASSES,sizeof(double));
			data->storage_visits_per_class=calloc(NUM_CLASSES,sizeof(int));
			data->storage_visits_per_class[CLASS_TELEMETRY]=NODE_SINGLE_VISIT;
			data->storage_visits_per_class[CLASS_TRANSITION]=NODE_SINGLE_VISIT;
			data->storage_visits_per_class[CLASS_BATCH]=NODE_SINGLE_VISIT;
		} else {
		data->node_storage_state=NODE_NO_STORAGE;
	}
	}
	//setup of variables according to the element type
	data->type=malloc(sizeof(char)*128);
	data->node_type=malloc(sizeof(char)*128);
	data->node_type[0]='\0';
	switch(type){
		case SENSOR:
			data->node_visits_per_class[CLASS_TRANSITION]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_TELEMETRY]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_COMMAND]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_BATCH]=NODE_NO_VISIT;
			snprintf(data->type,sizeof(char)*128,"sensor");
			int sens_type = getSensorType(elTop);
			snprintf(data->node_type,sizeof(char)*128,"%d",sens_type);
			break;
		case ACTUATOR:
			data->node_visits_per_class[CLASS_TRANSITION]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_TELEMETRY]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_COMMAND]=NODE_SINGLE_VISIT;
			data->node_visits_per_class[CLASS_BATCH]=NODE_NO_VISIT;
			snprintf(data->type,sizeof(char)*128,"actuator");
			int act_type = getActuatorType(elTop);
			snprintf(data->node_type,sizeof(char)*128,"%d",act_type);
			break;
		case LAN:
			data->input_node_visits_per_class[CLASS_TRANSITION]=NODE_NO_VISIT;
			data->input_node_visits_per_class[CLASS_TELEMETRY]=NODE_NO_VISIT;
			data->input_node_visits_per_class[CLASS_COMMAND]=NODE_SINGLE_VISIT;
			data->input_node_visits_per_class[CLASS_BATCH]=NODE_NO_VISIT;
			data->output_node_visits_per_class[CLASS_TRANSITION]=NODE_SINGLE_VISIT;
			data->output_node_visits_per_class[CLASS_TELEMETRY]=NODE_SINGLE_VISIT;
			data->output_node_visits_per_class[CLASS_COMMAND]=NODE_NO_VISIT;
			data->output_node_visits_per_class[CLASS_BATCH]=NODE_NO_VISIT;
			snprintf(data->type,sizeof(char)*128,"lan");
			break;
		case WAN:
			data->node_visits_per_class[CLASS_TRANSITION]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_TELEMETRY]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_COMMAND]=NODE_NO_VISIT;
			data->node_visits_per_class[CLASS_BATCH]=NODE_NO_VISIT;
			snprintf(data->type,sizeof(char)*128,"wan");
			break;
		case NODE:
			//central node do not receive replies
			if(getNodeType(elTop)!=CENTRAL){
				data->node_visits_per_class[CLASS_COMMAND]=NODE_SINGLE_VISIT;
			}
			data->node_visits_per_class[CLASS_TRANSITION]=NODE_SINGLE_VISIT;
			data->node_visits_per_class[CLASS_TELEMETRY]=NODE_SINGLE_VISIT;
			data->node_visits_per_class[CLASS_BATCH]=NODE_SINGLE_VISIT;
			snprintf(data->type,sizeof(char)*128,"node");
			switch(getNodeType(elTop)){
				case CENTRAL:
					snprintf(data->node_type,sizeof(char)*128,"central");
					break;
				case REGIONAL:
					snprintf(data->node_type,sizeof(char)*128,"regional");
					break;
				case LOCAL:
					snprintf(data->node_type,sizeof(char)*128,"local");
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
	if(data->response_times_A!=NULL){
		free(data->response_times_A);
	}
	if(data->input_service_demands!=NULL){
		free(data->input_service_demands);
	}
	if(data->input_utilization_factors!=NULL){
		free(data->input_utilization_factors);
	}
	if(data->input_response_times_A!=NULL){
		free(data->input_response_times_A);
	}
	if(data->output_service_demands!=NULL){
		free(data->output_service_demands);
	}
	if(data->output_utilization_factors!=NULL){
		free(data->output_utilization_factors);
	}
	if(data->output_response_times_A!=NULL){
		free(data->output_response_times_A);
	}
	if(data->node_visits_per_class!=NULL){
		free(data->node_visits_per_class);
	}
	if(data->input_node_visits_per_class!=NULL){
		free(data->input_node_visits_per_class);
	}
	if(data->output_node_visits_per_class!=NULL){
		free(data->output_node_visits_per_class);
	}
	if(data->type!=NULL){
		free(data->type);
	}
	if(data->node_type!=NULL){
		free(data->node_type);
	}

	if(data->service_times!=NULL){
		free(data->service_times);
	}

	if(data->input_service_times!=NULL){
		free(data->input_service_times);
	}

	if(data->output_service_times!=NULL){
		free(data->output_service_times);
	}
	if(data->storage_input_rates!=NULL){
		free(data->storage_input_rates);
	}
	if(data->storage_response_times_A!=NULL){
		free(data->storage_response_times_A);
	}
	if(data->storage_service_demands!=NULL){
		free(data->storage_service_demands);
	}
	if(data->storage_service_times!=NULL){
		free(data->storage_service_times);
	}
	if(data->storage_utilization_factors!=NULL){
		free(data->storage_utilization_factors);
	}

	if(data->storage_visits_per_class!=NULL){
		free(data->storage_visits_per_class);
	}

	if(data->childrens!=NULL){
		for(i=0;i<data->num_childrens;i++){
			if(data->childrens[i]!=NULL){
				free(data->childrens[i]);
			}
		}
		free(data->childrens);
	}

	if(data->num_mean_events_in_queue!=NULL){
		free(data->num_mean_events_in_queue);
	}
	if(data->input_num_mean_events_in_queue!=NULL){
		free(data->input_num_mean_events_in_queue);
	}
	if(data->output_num_mean_events_in_queue!=NULL){
		free(data->output_num_mean_events_in_queue);
	}
	if(data->storage_num_mean_events_in_queue!=NULL){
		free(data->storage_num_mean_events_in_queue);
	}
	if(data->response_times_B!=NULL){
		free(data->response_times_B);
	}
	if(data->input_response_times_B!=NULL){
		free(data->input_response_times_B);
	}
	if(data->output_response_times_B!=NULL){
		free(data->output_response_times_B);
	}
	if(data->storage_response_times_B){
		free(data->storage_response_times_B);
	}
	memset(data,0,sizeof(node_data));
}
/** \brief Visits the graph to perform several operations, according to ::graph_visit_type.
 * \param[in] data The ::node_data struct of the current node, when starting the visit it should be the central node.
 * \param[in] visit_type The type of visit to perform.
 * \param[in] out_tex The output file in LaTeX format (needed for ::GRAPH_PRINT_DATA visit type).
 * \param[in] out_json The output file in JSON format (needed for ::GRAPH_PRINT_DATA visit type).
 * \param[in] order The output file that conatains the order of node id as csv (needed for ::GRAPH_PRINT_DATA visit type).
 * \param[in] start_id The id of the node from which we start the visit.
 */
void graph_visit(node_data* data,Element_topology ** elTop,graph_visit_type visit_type,FILE* out_tex,FILE* out_json,FILE* order,int start_id,double * probOfActuatorsArray,int numberOfActTypes, int numberOfSensTypes){
	assert(data!=NULL && visit_type<NUM_GRAPH_VISITS);
	int *lowers=NULL,num_lowers=0,i;
	//this is the first visit of th	e tree, so we need to allocate a node_data struct for each children
	if(visit_type==GRAPH_COMPUTE_RATES){
		//we get the array of the lower nodes to be visited
		lowers=find_nodes_to_visit(data->top,&num_lowers);
		data->childrens=calloc(num_lowers,sizeof(node_data*));
		data->num_childrens=num_lowers;
		//printf("childrens: ");
		for(i=0;i<data->num_childrens;i++){
			//printf("%d ",lowers[i]);
			data->childrens[i]=malloc(sizeof(node_data));
			init_node_data(data->childrens[i],lowers[i],data,elTop[lowers[i]]);
		}
		free(lowers);
	}
	//in the second visit we need to compute the rates for command messages before reaching the leaves
	if(visit_type==GRAPH_COMPUTE_COMMANDS){
		compute_data(data,visit_type,probOfActuatorsArray,numberOfActTypes);
	}
	//to begin computation and to print results we need to reach a node with no lower nodes
	for(i=0;i<data->num_childrens && data->childrens!=NULL;i++){
		graph_visit(data->childrens[i],elTop,visit_type,out_tex,out_json,order,start_id,probOfActuatorsArray, numberOfActTypes, numberOfSensTypes);
	}
	//during the first visit we need to compute rate for most of the message types starting for the leaves of the tree
	if(visit_type==GRAPH_COMPUTE_RATES){
		compute_data(data,visit_type,probOfActuatorsArray,numberOfActTypes);
	}
	// in the third visit we print the data and free the occupied memory
	if(visit_type==GRAPH_PRINT_DATA){
		if(getType(data->top)!=WAN && getType(data->top)!=SENSOR){
			if(order!=NULL){
				fprintf(order,"%d",data->node_id);
			}
			if(out_tex!=NULL){
				print_results(data,out_tex);
			}
			//with this type of visit we will add the central node as last
			if(out_json!=NULL){
				print_json(data,out_json);
			}
			if(data->node_id!=start_id){
				if(out_json!=NULL){
					fprintf(out_json,",");
				}
				if(order!=NULL){
					fprintf(order,",");
				}
			}
		}
	}
	if(visit_type==GRAPH_CLEANUP){
		free_node_data(data);
	}
}

/// \brief Computes the parameters for the topology file location passed as the first argument.
int main(int argc, char** argv){
	//assert(argc==2);
	//get topology from file
	printf("Argument Usage: analytical_model_computation [path_to_topology.txt] [path_to_LP.txt] [output_path] by default current folder will be used if any of these arguments is missing\n");
	char* topology_path;
	char* lp_path;
	char* output_path;
	if(argc == 3){
		printf("Using provided paths\n");
		topology_path=argv[1];
		lp_path = argv[2];
		output_path=argv[3];
	}
	else{
		printf("Using current folder\n");
		topology_path= "topology.txt";
		lp_path= "LP.txt";
		output_path=".";
	}
	//int tex_filename_len=snprintf(NULL,0,"%s/results.tex",output_path)+1;
	int json_filename_len=snprintf(NULL,0,"%s/model_res.json",output_path)+1;
	//int order_filename_len=snprintf(NULL,0,"%s/order.txt",output_path)+1;
	//char *tex_filename=malloc((sizeof(char)*tex_filename_len));
	char *json_filename=malloc((sizeof(char)*json_filename_len));
	//char *order_filename=malloc((sizeof(char)*order_filename_len));
	//snprintf(tex_filename,tex_filename_len,"%s/results.tex",output_path);
	snprintf(json_filename,json_filename_len,"%s/model_res.json",output_path);
	//snprintf(order_filename,order_filename_len,"%s/order.txt",output_path);
	FILE *out_json=NULL, *out_tex=NULL, *order=NULL;
	//out_tex=fopen("results.tex","w");
	out_json=fopen(json_filename,"w");
	// order=fopen("order.txt","w");
	int central_id;
	//fprintf(out_tex,"\\documentclass{article}\n\\usepackage{booktabs}\n\\usepackage{float}\\begin{document}\n");
	//fprintf(out_tex,"\\section{Computed Formulas Example}\n");
	fprintf(out_json,"[");

	total_topology * totTop = getTopology(topology_path,lp_path,LP_AGGR_NONE);

	general_topology * genTop = getGenTopology(totTop);
	Element_topology ** elTop = totTop->lpt;
	//topology * genTop = getTopology(topology_path);

	node_data* central=malloc(sizeof(node_data));
	int total = getTotalNodes(genTop)+ getSensorNodes(genTop)+ getActuatorNodes(genTop)+ getNumberOfTotalLANs(genTop)+ getNumberOfTotalWANs(genTop);
	central_id=find_central(elTop,total);

	Element_topology * elTop_central = getLPTopology(totTop,central_id);
	init_node_data(central,central_id,NULL,elTop_central);
	//1st visit, to compute rates
	graph_visit(central,elTop,GRAPH_COMPUTE_RATES,out_tex,out_json,order,central->node_id,getProbOfActuators(genTop),getNumberOfActTypes(genTop),getNumberOfSensTypes(genTop));
	//2nd visit, to compute commands
	graph_visit(central,elTop,GRAPH_COMPUTE_COMMANDS,out_tex,out_json,order,central->node_id,getProbOfActuators(genTop),getNumberOfActTypes(genTop),getNumberOfSensTypes(genTop));
	//3rd visit, to print results
	graph_visit(central,elTop,GRAPH_PRINT_DATA,out_tex,out_json,order,central->node_id,getProbOfActuators(genTop),getNumberOfActTypes(genTop),getNumberOfSensTypes(genTop));
	graph_visit(central,elTop,GRAPH_CLEANUP,out_tex,out_json,order,central->node_id,getProbOfActuators(genTop),getNumberOfActTypes(genTop),getNumberOfSensTypes(genTop));
	free(central);
	//fprintf(out_tex,"\\end{document}");
	fprintf(out_json,"]");
	fflush(out_json);
	//fflush(out_tex);
	//fflush(order);
	fclose(out_json);
	//fclose(order);
	//fclose(out_tex);
	//free(tex_filename);
	free(json_filename);
	//free(order_filename);
	destroyTotalTopology(totTop);
	return 0;
}
