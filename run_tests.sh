#!/bin/bash

# some global variables which should not be edited, they are used to keep track of the test sessions
LOG_FILE="tests.log"
SESSION_DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
BEGIN_SESSION="START SESSION AT $SESSION_DATE"
END_SESSION="COMPLETED SESSION $SESSION_DATE"

#now there are the variables which contain our variations for the tests

# 0 is for pure serial execution, while 1 is for parallel configuration but with one working thread
thread_list="0" #("0" "1" "2" "4" "8" "16" "40")

#the list of seeds to be used
seed_list=("1996" "1996" "2006")

# yes means default configuration for simulation of message processing
sim_processing_options=("no") #("no" "yes" "10000")

scheduler_options=("FIFO") #("FIFO" "RR")

preemption_options=("no")

simulator_list=("ROOT-Sim") #("USE" "ROOT-Sim" "NeuRome")

# IMPORTANT: TO run test we need a catalog entry in the catalog_list for each element in the topology_list

#this is the list of paths to the topology config txt files.
topology_list=("../test/tests_topology/config.txt")

# this is the list of catalogs which will be accessed to get the elements information for each topology file
catalog_list=("../test/tests_topology/catalog")

if [[ ${#topology_list[@]} != ${#catalog_list[@]} ]]; then
	echo "ERROR:The topology list and the catalog list mus have a 1:1 association between their elements!"
	exit 255
fi

MAX_RETRY=2

#creation of the log file and the logging of the session start
touch $LOG_FILE
echo -e "$BEGIN_SESSION\n\n" >> $LOG_FILE
echo -e "$BEGIN_SESSION\n\n"

#now we need to create all the possible combination of the simulation parameters between each variable
# also, for each variable we need to create the corresponding argument
for seed in ${seed_list[@]}; do
	seed_arg="--seed=$seed"
	for simulator_arg in ${simulator_list[@]}
	do
		for thread_num in ${thread_list[@]}
		do
			if [[ $thread_num == "0" ]]; then
				execution_arg="serial"
				thread_arg=""
			else
				execution_arg="parallel"
				thread_arg="--wt=$thread_num"
			fi

			for scheduler in ${scheduler_options[@]}
			do
				scheduler_arg="--sched=$scheduler"

				for preemption_choice in ${preemption_options[@]}
				do
					if [[ $preemption_choice == "yes" ]]; then
						preemption_arg="--preempt"
					else
						preemption_arg=""
					fi

					for sim_processing_choice in ${sim_processing_options[@]}
					do
						if [[ $sim_processing_choice == "no" ]]; then
							sim_processing_arg=""
						elif [[ $sim_processing_choice == "yes" ]]; then
							sim_processing_arg="--sim_processing"
						else
							sim_processing_arg="--sim_processing=$sim_processing_choice"
						fi

						for ((topology_id=0; topology_id < ${#topology_list[@]}; topology_id++));
						do
							# we get the matching topology and catalog from the lists
							topology=${topology_list[topology_id]}
							catalog=${catalog_list[topology_id]}
							#we get the output path to be same as the topology file
							path="${topology%/*}"
							# some variables for the logging
							DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
							BEGIN="BEGIN test:.............$DATE"
							END="Test COMPLETE"
							ERR_RETRY="MAX RETRY ($MAX_RETRY) reached! SKIPPING"
							NUM_RETRIES=0


							topology_arg="--cat=$catalog --top=$topology"
							output="$(pwd)/$path/tests/$seed-$simulator_arg-$execution_arg-threads_$thread_num-$scheduler-preempt_$preemption_choice-sim_proc_$sim_processing_choice"

							out_arg="--out=$output"

							test_cmd="bash start.sh -q all $topology_arg $simulator_arg $execution_arg $thread_arg $scheduler_arg  $preemption_arg $sim_processing_arg $seed_arg $out_arg "

							# we create the test directory
							mkdir -p $output

							#we use the below while to check if the test as already been executed correctly (we assume it happens when the END variable is written on the log file and there return value of the command is 0), otherwise we retry the test for a maximum of MAX_RETRY tentatives.
							err=0
							while [[ $(grep -c "$output\n$END" $LOG_FILE) == 0 ]];
							do
							# we log the beginning of the test
								echo -e "$BEGIN\ntest command:\t$test_cmd" >> $LOG_FILE
								echo -e "test results path:\t$output">>$LOG_FILE
								echo "$BEGIN"

								#we execute the test
								$test_cmd
								err=$?
								#test is considered complete if there is no error
								if [[ $err == 0 ]]; then
									#we save the completion time and we log the successful completion
									DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
									echo -e "$END at $DATE\n\n" >> $LOG_FILE
									echo -e "$END at $DATE\n\n"
								fi
								NUM_RETRIES=$((NUM_RETRIES + 1))
								#if we exceeded the maximum allowed tentatives we skip the test and proceed further.
								if [[ $NUM_RETRIES -ge $MAX_RETRY ]]; then
									echo -e "$ERR_RETRY\n\n"
									echo -e "$ERR_RETRY\n\n" >> $LOG_FILE
									break
								fi
							done
						done
					done
				done
			done
		done
	done
done
# we log the successful end of the test session
DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
echo -e "$END_SESSION AT $DATE\n\n" >> $LOG_FILE
echo -e "$END_SESSION AT $DATE\n\n"
