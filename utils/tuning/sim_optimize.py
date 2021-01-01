import argparse
import json
import jmespath
import numpy as np
import os
from scipy.optimize import minimize_scalar

# this function will visit the graph and compute the communication rate between a father and its child if they are on different LPs. (it works for one father at a time).
def visit_graph(node_id,comms_dict,lp_list,model):
	node=jmespath.search(f'[?id==`{node_id}`]',model)[0]
	node_lp=lp_list[node['id']]
	# get the children list
	childrens=node['childrens']
	num_childrens=len(childrens)
	#now we need to check if we are in the same lp of our childrens
	for child_id in childrens:
		visit_graph(child_id,comms_dict,lp_list,model)
		child=jmespath.search(f'[?id==`{child_id}`]',model)[0]
		# father and child are in diffrent lp we need to keep track of this communication
		if node_lp != lp_list[child_id]:
			# we compute the child rate between the father and the child
			child_rate=0
			#we add in batch the telemetry, commands and batch rates divided by their aggregation rate
			classes=["telemetry","command","batch"]
			for elem in classes:
				child_rate+=jmespath.search(f'sum([lan_in,lan_out,parameters][].{elem}.lambda_in)',child)/jmespath.search(f'sum([lan_in,lan_out,parameters][].{elem}.aggregation_rate)',child)
			#then we estimate the transition rate by diving the rate of transition received by the father by the number of children.
			child_rate+=jmespath.search(f'sum([lan_in,lan_out,parameters][].transition.lambda_in)',node)/num_childrens
			if child_rate >0:
				if f'{node_lp}-{lp_list[child_id]}' not in comms_dict:
					comms_dict[f'{node_lp}-{lp_list[child_id]}']=0
				comms_dict[f'{node_lp}-{lp_list[child_id]}']+=child_rate


def compute_alpha(simulator,comms_dict,n_proc,num_lps,total_message_rate):
	#compute alpha, which is the probability of having a communication which is across differen processors (or LPs with USE as a simulation platform)
	alpha=0
	# we need to compute the rate between lps associated to different threads
	if simulator == "ROOT-Sim":
		#with rootsim we need to compute the probability of communication between LPs associated to different threads. The association between threads and LPs is approximated.
		for src_lp in range(0,num_lps):
			src_thread=src_lp%n_proc
			for dest_lp in range(src_lp,num_lps):
				dest_thread=dest_lp%n_proc
				if f'{src_thread}-{dest_thread}' in comms_dict:
					alpha+=comms_dict[f'{src_thread}-{dest_thread}']
				if f'{dest_thread}-{src_thread}' in comms_dict:
					alpha+=comms_dict[f'{dest_thread}-{src_thread}']
	elif simulator == "USE":
		#with USE we need to take into account all the communication between LPs
		for elem in comms_dict.Values():
			alpha+=elem
	return alpha/total_message_rate

def compute_rollback_prob_upper_bound(alpha,delta,num_lps,n_proc):
	return alpha*(1/2-delta)/(1-(1/2-delta)*(1-((num_lps-1)/num_lps)**(n_proc-1)))

def compute_nsafe(Pr_ub,delta,n_proc):
	nsafe=0
	for i in range(1,n_proc+1):
		nsafe+=(((1-Pr_ub)*((1-alpha)*(1/2+delta)+alpha*((1/2+delta)**2)))**(i-1))
	return nsafe

def compute_UB_pl(Pr_ub,delta,n_proc):
	nsafe=compute_nsafe(Pr_ub,delta,n_proc)
	return 1-(nsafe/n_proc)

def get_Tev_parallel(simulator,parallel_run):
	t_extract=0
	t_out=0
	t_r=0
	t_ev=0
	t_s=0
	t_in=0
	if simulator == "ROOT-Sim":
		f_run=open(parallel_run+"/outputs/execution_stats")
		lines=f_run.readlines()
		n_events=int(lines[30].strip().split(" ")[-1])
		t_ev=float(lines[38].strip().split(" ")[-2])
		t_r=float(lines[41].strip().split(" ")[-2])
		t_s=float(lines[40].strip().split(" ")[-2])
	if simulator == "USE":
		f_run=open(parallel_run+"/USE_output.txt")
		line=[""]
		for line in f_run:
			if "Average time spent to fetch....................." in line:
				t_extract+=float(line.strip().split(" ")[-2])
			if "Restore Checkpoint time........................." in line:
				t_r+=float(line.strip().split(" ")[-2])
			if "Average time to process any event...............: " in line:
				t_ev+=float(line.strip().split(" ")[-2])
			if "Save Checkpoint time............................" in line:
				t_s+=float(line.strip().split(" ")[-2])
			if "Average time spent to Enqueue...................:" in line:
				t_out+=float(line.strip().split(" ")[-2])
			if "TOTAL EXECUTED EVENTS ..... :" in line:
				n_events=int(line.strip().split(" ")[-1])

	return t_extract,t_out,t_r,t_ev,t_s,t_in,n_events


def get_Tev_serial(simulator,serial_run):
	tev_serial=-1
	if simulator  == "ROOT-Sim":
		f_run=open(serial_run+"/outputs/sequential_stats")
		lines=f_run.readlines()
		tev_serial=float(lines[10].strip().split(" ")[-2])
	if simulator == "USE":
		f_run=open(serial_run+"/USE_output.txt")
		tev_serial=0
		for line in f_run:
			if "Average time to process any event..............." in line:
				tev_serial+=float(line.strip().split(" ")[-2])
			if "Save Checkpoint time............................" in line:
				tev_serial+=float(line.strip().split(" ")[-2])
			if "Average time spent to fetch....................." in line:
				tev_serial+=float(line.strip().split(" ")[-2])
			if "Average time spent to Enqueue..................." in line:
				tev_serial+=float(line.strip().split(" ")[-2])
	return tev_serial

def compute_R(checkpoint_period,Pr_ub,t_extract,t_out,t_r,t_ev,t_s,t_in,serial_cost,n_proc,UB_pl):
	checkpoint_period_int=int(checkpoint_period)
	parallel_cost=t_extract+UB_pl*t_out+Pr_ub*t_r+(Pr_ub*(checkpoint_period_int-1)/2*t_ev)+t_ev+t_out+(t_s/checkpoint_period_int)+(1+UB_pl)*t_in
	R=parallel_cost/(n_proc*(1-UB_pl)*serial_cost)
	return R



parser = argparse.ArgumentParser(description="apply the timewarp tradeoff algorithm to choose between a serial and parallel simulation")
parser.add_argument('simulator',type=str,choices=["ROOT-Sim", "USE"],help="simulator that will run the simulation")
parser.add_argument('model_res',metavar="model_res",type=str,help="Path to the analytcal model json file")
parser.add_argument('lp_config',metavar="lp_config",type=str,help="the path to the LP.txt file")
parser.add_argument('output',type=str,help="the path where the output file with the configuration mus be created")
parser.add_argument('serial_run',type=str,help="The location of the statistics file for a serial run of the model")
parser.add_argument('parallel_run',type=str,help="The location of the statistics file for a parallel run of the model")
args = parser.parse_args()

Rmin=-1
delta=0
#get the number of LPs (and so the maximum number of threads) from LP.txt
f_lp=open(args.lp_config+"/LP.txt","r")
num_elements=int(f_lp.readline())
num_lps=int(f_lp.readline())

#then we need to get the LP id for each element, we do so by building a numpy array where the key is the element id and the value is the LP id.
lp_list=np.zeros(num_elements,dtype=int)
for line in f_lp:
	line=line.strip().split(";")
	if len(line) == 3:
		lp_f_model=open(args.model_res+"/model_res.json","r")
		lp_id=line[0]
		elems=line[2].strip().split(",")
		for elem in elems:
			elem=int(elem)
			lp_list[elem]=lp_id
f_lp.close()
#we need to get some informations from the results of the analytical model
f_model=open(args.model_res+"/model_res.json","r")
model=json.load(f_model)
total_message_rate=jmespath.search("sum([].*.*.lambda_in[][])",model)
#and the rate of messages that are sent between LPs
# to do so we visit the tree starting from the central node and we record the communication between different lps
#this node will contain the rate of the communication between two lps with the key "[lp1]-[lp2]", this key is not ordered, so also "[lp2]-[lp1]" could be found
comms_dict={}
visit_graph(0,comms_dict,lp_list,model)
f_model.close()

parallel_costs=get_Tev_parallel(args.simulator,args.parallel_run)
t_extract=parallel_costs[0]
t_out=parallel_costs[1]
t_r=parallel_costs[2]
t_ev=parallel_costs[3]
t_s=parallel_costs[4]
t_in=parallel_costs[5]
n_parallel_events=parallel_costs[6]
serial_cost=get_Tev_serial(args.simulator,args.serial_run)

delta=0
R_min=-1
checkpoint_period=-1
chosen_procs=-1
for n_proc in range(2,num_lps+1):
	R=-1
	#we compute alpha
	alpha=compute_alpha(args.simulator,comms_dict,n_proc,num_lps,total_message_rate)
	# then we need to compute R and minimize in respect to the checkpoint period
	Pr_ub=compute_rollback_prob_upper_bound(alpha,delta,num_lps,n_proc)
	UB_pl=compute_UB_pl(Pr_ub,delta,n_proc)
	# we minimize R in respect to the checkpoint period
	min_res=minimize_scalar(compute_R,bounds=[1,40],method='bounded',args=(Pr_ub,t_extract,t_out,t_r,t_ev,t_s,t_in,serial_cost,n_proc,UB_pl))
	ckp=int(min_res.x)
	R=compute_R(ckp,Pr_ub,t_extract,t_out,t_r,t_ev,t_s,t_in,serial_cost,n_proc,UB_pl)
	if R < R_min and R>=0 or R_min < 0:
		R_min=R
		chosen_procs=n_proc
		if ckp > 40:
			ckp=40
		checkpoint_period=ckp

print(f"R {R_min} checkpoint period {checkpoint_period} threads {chosen_procs}")
if R_min >=1:
	chosen_procs=1
f_out=open(args.output+"/sim_config.json","w")
f_out.write("{"+f" \"R_min\":{R_min},\"Checkpoint_period_opt\":{checkpoint_period}, \"Chosen_threads\":{chosen_procs}"+"}")
f_out.close()