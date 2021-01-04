#!/bin/bash
########################################################################################################################
# USER-CONFIGURABLE VARIABLES                                                                                          #
########################################################################################################################

# The following variable will need to be edited by the user to customize the automated tests behaviour.

# when using paths consider the realpath command to resolve relative paths

#if you want to run a program (for example timeout) in which the script must be executed the tou can set it up here. te content of this varaible vill be executed before the script, it's jour job to insert ; && or & and so on to ensure that averything works.
parent_cmd="../test/utils/timeout/timeout -m 14000000"

# this variables is used to determine the location of the folder which contains the start.sh script which will be executed to start each test
script_location=$(realpath .)

# this variable will be used to redirect compilation messages to a file, possible a values are "" to turn off redirection or the path to the file.
redir_compilation="" #"/dev/null"

# the base directory where all the executed tests can be found (separated by the topology name)
output_base_dir="$(realpath ..)/test/performance_tests"

#customize the test execution, targets must separated by spaces. Available values: all, -g, -a, -s, -r. Refer to the start.sh help message for info. (all must be used by itself, without other targets)
script_target="-g -s"

#timeout argument in seconds. -1 means no timeout
real_timeout="300"

#timeout argument in logical virtual time. -1 means no timeout
timeout_lvt="500000"

# 0 is for pure serial execution, while 1 is for parallel configuration but with one working thread
thread_list=("5") # ("0" "2" "5" "10" "20" "30" "40")

# makes the test fail if there are more threads than LPs. Available options: yes, no
threads_less_than_lps="no"

#the list of seeds to be used. -1 is for no seed specified
seed_list=("-1")

# yes means default configuration for simulation of message processing
sim_processing_options=("no" "yes") #("no" "yes" "10000")

scheduler_options=("FIFO") # ("FIFO" "RR")

preemption_options=("no" "yes") # ("no" "yes")

simulator_list=("ROOT-Sim" "USE") # ("USE" "ROOT-Sim" "NeuRome")

# currently available choices are central, local, regional and lan, refer to the documentation of utils/partop/header:lp_aggregation_criteria for details.
lp_aggregation=("regional" "local" "lan")

#to run test we need a catalog entry in the catalog_list for each element in the topology_list, if you want to use a unique catalog for each test set the following variable to "yes", otherwise set it to "no"
unique_catalog_location="yes"

#The following variable will need to contain the location of the unique catalog.
unique_catalog_path="$(realpath ..)/test/catalog"

#this variable will determine if there is a single folder that stores all the topologies. Possible values: "yes","no"
unique_topologies_location="yes"

#this variable will maintain the location of the topology base folder (don't put a final '/')
unique_topologies_path="$(realpath ..)/test/topologies"

#this is the list of paths to the topology config files, if there is a unique folder this array can contain only the topology file basepath and not the camplete path to each file.
topology_list=("80-400.txt")

# this is the list of catalogs which will be accessed to get the elements information for each topology, it is used only if there is no unique catalog. Thus, the number of entries in this list must be the exact number of entries of the topology_list
catalog_list=("$(realpath ..)/test/catalog")

#if the central node must be ignored in the simulation (yes/no)
ignore_central="no"

# the maximum number of retires to do when a test fails
MAX_RETRY=2

########################################################################################################################
#STARTING TESTS                                                                                                        #
########################################################################################################################

# we compute the number of test to execute in this session
num_tests=$(( ${#topology_list[@]} * ${#lp_aggregation[@]} * ${#simulator_list[@]} * ${#preemption_options[@]} * ${#scheduler_options[@]} * ${#sim_processing_options[@]} * ${#seed_list[@]} * ${#thread_list[@]} ))
test_num=0

num_tests_failed=0
num_tests_completed=0
num_tests_skipped=0
# some global variables which should not be edited, they are used to keep track of the test sessions
LOG_FILE="tests.log"
SESSION_DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
BEGIN_SESSION="START SESSION AT $SESSION_DATE with $num_tests tests"
END_SESSION="COMPLETED SESSION $SESSION_DATE"

if [[ $redir_compilation != "" ]]; then
	redir_compilation_arg="--redir-compilation-messages=$redir_compilation"
else
	redir_compilation_arg=""
fi

target=""
if [[ $script_target =~ all ]]; then
	target="all"
else
	if [[ $script_target =~ -g ]]; then
		target+="generation"
	fi
	if [[ $script_target =~ -a ]]; then
		target+="_analytic"
	fi
	if [[ $script_target =~ -s ]]; then
		target+="_simulation"
	fi
	if [[ $script_target =~ -r ]]; then
		target+="_results"
	fi
fi

if [[ $script_target =~ [\ gars-]*all[\ -gasr]* ]] && [[ $script_target != "all" ]]; then
	echo "ERROR: wrong target specified: $script_target"
	exit 255
fi

if [[ ${#topology_list[@]} != ${#catalog_list[@]} ]] && [[ $unique_catalog == "no" ]]; then
	echo "ERROR:The topology list and the catalog list mus have a 1:1 association between their elements!"
	exit 255
fi

#creation of the log file and the logging of the session start
touch $LOG_FILE
echo -e "$BEGIN_SESSION\n\n" >> $LOG_FILE
echo -e "$BEGIN_SESSION\n\n"

#now we need to create all the possible combination of the simulation parameters between each variable
# also, for each variable we need to create the corresponding argument
for seed in ${seed_list[@]}; do

	if [[ $seed == "-1" ]]; then
		seed_arg=""
	else
		seed_arg="--seed=$seed"
	fi
	for simulator_arg in ${simulator_list[@]}; do

	simulator_arg=${simulator_arg,,}
	simulator_arg=${simulator_arg//'-'/''}
	if [[ $timeout_lvt != "-1" ]]; then
		timeout=$timeout_lvt
		timeout_arg="--simulation-timeout=$timeout_lvt"
	fi
	if [[	$real_timeout != "-1" ]]; then
		timeout_arg+=" --timeout=$real_timeout"
		if [[ $timeout_lvt == "-1" ]]; then
		timeout=$real_timeout
		fi
	else
		timeout_arg=""
		timeout="none"
	fi

		for lp_aggr_choice in ${lp_aggregation[@]}; do
			lp_aggr_arg="--lp-aggregation=$lp_aggr_choice"

			for thread_num in ${thread_list[@]}; do
				if [[ $thread_num == "0" ]]; then
					execution_arg="serial"
					thread_arg=""
				else
					execution_arg="parallel"
					thread_arg="--wt=$thread_num"
				fi

				for scheduler in ${scheduler_options[@]}; do
					scheduler_arg="--sched=$scheduler"

					for preemption_choice in ${preemption_options[@]}
					do

						if [[ $preemption_choice == "yes" ]]; then
							preemption_arg="--preempt"
						else
							preemption_arg=""
						fi

						for sim_processing_choice in ${sim_processing_options[@]}; do

							if [[ $sim_processing_choice == "no" ]]; then
								sim_processing_arg=""
							elif [[ $sim_processing_choice == "yes" ]]; then
								sim_processing_arg="--sim-processing"
							else
								sim_processing_arg="--sim-processing=$sim_processing_choice"
							fi

							for ((topology_id=0; topology_id < ${#topology_list[@]}; topology_id++)); do

								# we get the matching topology and catalog from the lists
								topology_file=${topology_list[topology_id]}

								if [[ $unique_catalog_location == "no" ]]; then
									catalog=${catalog_list[topology_id]}
								else
									catalog=$unique_catalog_path
								fi

								if [[ $unique_topologies_location == "no" ]]; then
									#we get the topology name
									#step 1, get the basepath
									topology_name="${topology_file##*/}"
									#step 2, we remove the extension of the topology file
									topology_name=${topology_name%.*}
									# we build the topology argument
									topology_arg="--cat=$catalog --top=$topology_file"
								else
									#we get the topology name
									topology_name=${topology_file%.*}
									# we build the topology argument
									topology_arg="--cat=$catalog --top=$unique_topologies_path/$topology_file"
								fi

								output="$output_base_dir/$topology_name/target_$target-seed_$seed-timeout_$timeout-$simulator_arg-$execution_arg-threads_$thread_num-$scheduler-preempt_$preemption_choice-sim_proc_$sim_processing_choice-lp_aggr_$lp_aggr_choice"

								out_arg="--out=$output"

								if [[ $threads_less_than_lps == "yes" ]]; then
									threads_less_than_lps_arg="--threads-less-than-lps"
								else
									threads_less_than_lps_arg=""
								fi

								if [[ $ignore_central == "yes" ]]; then
									ignore_central_arg="--no-central"
								else
									ignore_central_arg=""
								fi

								test_cmd="bash $script_location/start.sh -q $script_target $topology_arg $simulator_arg $execution_arg $thread_arg $scheduler_arg  $preemption_arg $sim_processing_arg $seed_arg $timeout_arg $lp_aggr_arg $threads_less_than_lps_arg $redir_compilation_arg $ignore_central_arg $out_arg"

								# we create the test directory
								mkdir -p $output

								#we use the below while to check if the test as already been executed correctly (we assume it happens when the END variable is written on the log file and there return value of the command is 0), otherwise we retry the test for a maximum of MAX_RETRY tentatives.
								err=0
								touch $output/test_esit.log
								NUM_RETRIES=0
								END="Test $topology_name COMPLETE"
								test_num=$(( $test_num +1 ))

								DATE_BEGIN="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"

								if [[ $(tail -n 1 $output/test_esit.log | grep -c -e "^$END.*\$") != 0 ]]; then
									echo -e "Test $test_num  of $num_tests with topology $topology_name already completed, SKIPPED.\n\n"
									echo -e "Test $test_num  of $num_tests with topology $topology_name already completed, SKIPPED.\n\n" >> $LOG_FILE
									num_tests_skipped=$(( num_tests_skipped + 1 ))
								fi
								test_result="failed"
								while [[ $(tail -n 1 $output/test_esit.log | grep -c -e "^$END.*\$") == 0 ]]; do
									BEGIN="BEGIN test $test_num of $num_tests with topology $topology_name:.............$DATE_BEGIN"
									# we log the beginning of the test
									begin_log="$BEGIN\ntest command:\t$test_cmd\nresults path: $output"
									echo -e "$begin_log" >> $LOG_FILE
									echo -e $begin_log > $output/test_esit.log
									echo "$BEGIN"
									bash start.sh -q clean
									#we execute the test
									script -a -e -q $output/test_esit.log -c "$parent_cmd $test_cmd"
									err=$?
									echo $err
									DATE_END="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
									#test is considered complete if there is no error
									if [[ $err == 0 ]]; then
										#we save the completion time and we log the successful completion
										end_log="$END at $DATE_END"
										echo -e "$end_log\n\n" >> $LOG_FILE
										echo -e "$end_log" >> $output/test_esit.log
										echo -e "$end_log\n\n"
										num_tests_completed=$(( num_tests_completed + 1 ))
										test_result="completed"
									else
										error_log="test $topology_name FAILED at $DATE_END with error code $err"
										echo -e "$error_log\n\n" >> $output/test_esit.log
										echo -e "$error_log\n\n" >> $LOG_FILE
										echo -e "$error_log\n\n"
									fi
									NUM_RETRIES=$((NUM_RETRIES + 1))
									#if we exceeded the maximum allowed tentatives we skip the test and proceed further.
									if [[ $NUM_RETRIES -ge $MAX_RETRY ]] && [[ $test_result == "failed" ]]; then
										ERR_RETRY="MAX RETRY ($MAX_RETRY) reached! SKIPPING"
										echo -e "$ERR_RETRY\n\n"
										echo -e "$ERR_RETRY\n\n" >> $LOG_FILE
										echo -e $ERR_RETRY >> $output/test_esit.log
										num_tests_failed=$(( num_tests_failed + 1 ))
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
done
# we log the successful end of the test session
DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
echo -e "$END_SESSION at $DATE with $num_tests_completed completed tests, $num_tests_failed failed tests and $num_tests_skipped skipped tests out of $num_tests.\n\n" >> $LOG_FILE
echo -e "$END_SESSION at $DATE with $num_tests_completed completed tests, $num_tests_failed failed tests and $num_tests_skipped skipped tests out of $num_tests.\n\n"
