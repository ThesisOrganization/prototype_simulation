#!/bin/bash
########################################################################################################################
# USER-CONFIGURABLE VARIABLES                                                                                          #
########################################################################################################################

# The following variable will need to be edited by the user to customize the automated tests behaviour.

# when using paths consider the realpath command to resolve relative paths

#if you want to run a program (for example timeout) in which the script must be executed the tou can set it up here. te content of this varaible vill be executed before the script, it's jour job to insert ; && or & and so on to ensure that averything works.
parent_cmd=""

# this variables is used to determine the location of the folder which contains the start.sh script which will be executed to start each test
script_location=$(realpath .)

# the base directory where all the executed tests can be found (separated by the topology name)
output_base_dir="$(realpath ..)/test/model_tests"

#customize the test execution, targets must separated by spaces. Available values: all, -g, -a, -s, -r. Refer to the start.sh help message for info. (all must be used by itself, without other targets)
script_target="all"

#timeout argument for USE (in seconds). -1 means no timeout
timeout_use="-1"

#timeout argument for ROOT-Sim and NeuRome (in logical virtual time). -1 means no timeout
timeout_rootsim="-1"

# 0 is for pure serial execution, while 1 is for parallel configuration but with one working thread
thread_list=("0") # ("0" "2" "5" "10" "20" "30" "40")

# makes the test fail if there are more threads than LPs. Available options: yes, no
threads_less_than_lps="no"

#the list of seeds to be used. -1 is for no seed specified
seed_list=("1492")

simulation_time_list=("1000" "2000" "3000" "4000" "5000" "6000" "7000" "8000" "9000" "10000" "11000" "12000" "13000" "14000" "15000" "16000" "17000" "18000" "19000" "20000" "21000" "22000" "23000" "24000" "25000" "26000" "27000" "28000" "29000" "30000" "31000" "32000" "33000" "34000" "35000" "36000" "37000" "38000" "39000" "40000" "41000" "42000" "43000" "44000" "45000" "46000" "47000" "48000" "49000" "50000" "51000" "52000" "53000" "54000" "55000" "56000" "57000" "58000" "59000" "60000" "61000" "62000" "63000" "64000" "65000" "66000" "67000" "68000" "69000" "70000" "71000" "72000" "73000" "74000" "75000" "76000" "77000" "78000" "79000" "80000" "81000" "82000" "83000" "84000" "85000" "86000" "87000" "88000" "89000" "90000" "91000" "92000" "93000" "94000" "95000" "96000" "97000" "98000" "99000" "100000" "101000" "102000" "103000" "104000" "105000" "106000" "107000" "108000" "109000" "110000" "111000" "112000" "113000" "114000" "115000" "116000" "117000" "118000" "119000" "120000" "121000" "122000" "123000" "124000" "125000" "126000" "127000" "128000" "129000" "130000" "131000" "132000" "133000" "134000" "135000" "136000" "137000" "138000" "139000" "140000" "141000" "142000" "143000" "144000" "145000" "146000" "147000" "148000" "149000" "150000" "151000" "152000" "153000" "154000" "155000" "156000" "157000" "158000" "159000" "160000" "161000" "162000" "163000" "164000" "165000" "166000" "167000" "168000" "169000" "170000" "171000" "172000" "173000" "174000" "175000" "176000" "177000" "178000" "179000" "180000" "181000" "182000" "183000" "184000" "185000" "186000" "187000" "188000" "189000" "190000" "191000" "192000" "193000" "194000" "195000" "196000" "197000" "198000" "199000" "200000" "201000" "202000" "203000" "204000" "205000" "206000" "207000" "208000" "209000" "210000" "211000" "212000" "213000" "214000" "215000" "216000" "217000" "218000" "219000" "220000" "221000" "222000" "223000" "224000" "225000" "226000" "227000" "228000" "229000" "230000" "231000" "232000" "233000" "234000" "235000" "236000" "237000" "238000" "239000" "240000" "241000" "242000" "243000" "244000" "245000" "246000" "247000" "248000" "249000" "250000" "251000" "252000" "253000" "254000" "255000" "256000" "257000" "258000" "259000" "260000" "261000" "262000" "263000" "264000" "265000" "266000" "267000" "268000" "269000" "270000" "271000" "272000" "273000" "274000" "275000" "276000" "277000" "278000" "279000" "280000" "281000" "282000" "283000" "284000" "285000" "286000" "287000" "288000" "289000" "290000" "291000" "292000" "293000" "294000" "295000" "296000" "297000" "298000" "299000" "300000")

# yes means default configuration for simulation of message processing
sim_processing_options=("no") # ("no" "yes" "10000")

scheduler_options=("FIFO") # ("FIFO" "RR")

preemption_options=("no") # ("no" "yes")

simulator_list=("ROOT-Sim") # ("USE" "ROOT-Sim" "NeuRome")

# currently available choices are central, local, regional and lan, refer to the documentation of utils/partop/header:lp_aggregation_criteria for details.
lp_aggregation=("central") # ("central" "regional" "local" "lan")

#to run test we need a catalog entry in the catalog_list for each element in the topology_list, if you want to use a unique catalog for each test set the following variable to "yes", otherwise set it to "no"
unique_catalog_location="yes"

#The following variable will need to contain the location of the unique catalog.
unique_catalog_path="$(realpath ..)/test/catalog"

#this variable will determine if there is a single folder that stores all the topologies. Possible values: "yes","no"
unique_topologies_location="yes"

#this variable will maintain the location of the topology base folder (don't put a final '/')
unique_topologies_path="$(realpath ..)/test/topologies"

#this is the list of paths to the topology config files, if there is a unique folder this array can contain only the topology file basepath and not the camplete path to each file.
topology_list=("2-4-8-16_convergence.txt")

# this is the list of catalogs which will be accessed to get the elements information for each topology, it is used only if there is no unique catalog. Thus, the number of entries in this list must be the exact number of entries of the topology_list
catalog_list=("$(realpath ..)/test/catalog")

# the maximum number of retires to do when a test fails
MAX_RETRY=2

########################################################################################################################
#STARTING TESTS                                                                                                        #
########################################################################################################################
# some global variables which should not be edited, they are used to keep track of the test sessions
LOG_FILE="tests.log"
SESSION_DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
BEGIN_SESSION="START SESSION AT $SESSION_DATE"
END_SESSION="COMPLETED SESSION $SESSION_DATE"

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
	if [[ $simulator_arg == "rootsim" || $simulator_arg == "neurome" ]] && [[ $timeout_rootsim != "-1" ]]; then
		timeout=$timeout_rootsim
		timeout_arg="--timeout=$timeout_rootsim"
	elif [[	$simulator_arg == "use" && $timeout_use != "-1" ]]; then
		timeout_arg="--timeout=$timeout_use"
		timeout=$timeout_use
	else
		timeout_arg=""
		timeout="none"
	fi

		for lp_aggr_choice in ${lp_aggregation[@]}; do
			lp_aggr_arg="--lp_aggregation=$lp_aggr_choice"

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
								sim_processing_arg="--sim_processing"
							else
								sim_processing_arg="--sim_processing=$sim_processing_choice"
							fi

                            for simulation_time in ${simulation_time_list[@]}; do
                                simulation_time_arg="--simulation_time_end=$simulation_time"

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

							    	output="$output_base_dir/$topology_name/target_$target-seed_$seed-timeout_$timeout-$simulator_arg-$execution_arg-threads_$thread_num-$scheduler-preempt_$preemption_choice-sim_proc_$sim_processing_choice-lp_aggr_$lp_aggr_choice-sim_time_end_$simulation_time"

							    	out_arg="--out=$output"

							    	if [[ $threads_less_than_lps == "yes" ]]; then
							    		threads_less_than_lps_arg="--threads-less-than-lps"
							    	else
							    		threads_less_than_lps_arg=""
							    	fi

							    	test_cmd="bash $script_location/start.sh -q $script_target $topology_arg $simulator_arg $execution_arg $thread_arg $scheduler_arg  $preemption_arg $sim_processing_arg $seed_arg $timeout_arg $lp_aggr_arg $threads_less_than_lps_arg $simulation_time_arg $out_arg"

							    	# we create the test directory
							    	mkdir -p $output

							    	#we use the below while to check if the test as already been executed correctly (we assume it happens when the END variable is written on the log file and there return value of the command is 0), otherwise we retry the test for a maximum of MAX_RETRY tentatives.
							    	err=0
							    	touch $output/test_esit.log
							    	NUM_RETRIES=0
							    	END="Test $topology_name COMPLETE"
							    	while [[ $(tail -n 1 $output/test_esit.log | grep -c -e "^$END.*\$") == 0 ]]; do
							    		DATE_BEGIN="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
							    		BEGIN="BEGIN test $topology_name:.............$DATE_BEGIN"
							    		# we log the beginning of the test
							    		begin_log="$BEGIN\ntest command:\t$test_cmd\nresults path: $output"
							    		echo -e $begin_log >> $LOG_FILE
							    		echo -e $begin_log > $output/test_esit.log
							    		echo "$BEGIN"

							    		#we execute the test
							    		$parent_cmd $test_cmd
							    		err=$?
							    		DATE_END="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
							    		#test is considered complete if there is no error
							    		if [[ $err < 128 ]]; then
							    			#we save the completion time and we log the successful completion
							    			end_log="$END at $DATE_END"
							    			echo -e "$end_log\n\n" >> $LOG_FILE
							    			echo -e "$end_log" >> $output/test_esit.log
							    			echo -e "$end_log\n\n"
							    		else
							    			error_log="test $topology_name FAILED at $DATE_END with error code $err"
							    			echo -e "$error_log\n\n" >> $output/test_esit.log
							    			echo -e "$error_log\n\n" >> $LOG_FILE
							    			echo -e "$error_log\n\n"
							    		fi
							    		NUM_RETRIES=$((NUM_RETRIES + 1))
							    		#if we exceeded the maximum allowed tentatives we skip the test and proceed further.
							    		if [[ $NUM_RETRIES -ge $MAX_RETRY ]]; then
							    			ERR_RETRY="MAX RETRY ($MAX_RETRY) reached! SKIPPING"
							    			echo -e "$ERR_RETRY\n\n"
							    			echo -e "$ERR_RETRY\n\n" >> $LOG_FILE
							    			echo -e $ERR_RETRY >> $output/test_esit.log
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
done
# we log the successful end of the test session
DATE="$(date +%d)/$(date +%m)/$(date +%Y) - $(date +%H):$(date +%M):$(date +%S)"
echo -e "$END_SESSION AT $DATE\n\n" >> $LOG_FILE
echo -e "$END_SESSION AT $DATE\n\n"
