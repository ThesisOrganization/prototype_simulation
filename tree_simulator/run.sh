#!/bin/bash

number_lp=""
file_json="simulation_results.json"
stat_json="simulation_stats.json"
run_type="serial"
working_threads=1
options=""
dbg_arg=""
dbg_param=""
sim_name=$(sed -n 1p ../Simulator/simulator-name)

for arg
do
	if [[ $arg == "json" || $arg == "clean" ]]; then
			options=$arg
	fi
	if [[ $arg == "parallel" ]]; then
			run_type="parallel"
			working_threads=$(nproc)
	fi
	if [[ $arg == "valgrind" || $arg == "gdb" ]]; then
		dbg_arg=$arg
	fi
	if [[ ${arg:0:5} == "--lp=" ]]; then
		number_lp=${arg#"--lp="}
	fi
	if [[ ${arg:0:5} == "--wt=" ]]; then
		working_threads=${arg#"--wt="}
	fi
done

if [ "$dbg_arg" == "valgrind" ]; then
		dbg_param="valgrind --leak-check=full"
elif [ "$dbg_arg" == "gdb" ]; then
		dbg_param="gdb --args"
fi

if [ "$options" == "json" ]; then
		python3 generate_json.py $file_json
elif [ "$options" == "clean" ]; then
		rm lp_data/*.json
		rm -r outputs
		rm -rf USE-model-sources
		make clean
		rm *.txt
		rm *.json
else
	echo "Compiling and running model with $sim_name"
	if [[ -e "LP.txt" && -r "LP.txt" ]]; then
		number_lp=$(sed -n 2p LP.txt)
	else
		number_lp=$(sed -n 1p topology.txt)
	fi
	if [[ $working_threads -gt $number_lp ]]; then
		echo "number of working threads grater than number of lp, matching number of lp"
		working_threads=$number_lp
	fi
	if [[ $sim_name == "ROOT-Sim" ]]; then
		make
		rm -r outputs
		rm lp_data/*.json
		if [[ $run_type == "parallel" ]]; then
				$dbg_param ./simulation --wt $working_threads --lp $number_lp
				stat_source="outputs/execution_stats"
			else
				$dbg_param ./simulation --sequential --lp $number_lp
				stat_source="outputs/sequential_stats"
			fi
			stat_used_mem=$(sed -n 's/PEAK MEMORY USAGE.......... : //p' $stat_source)
			stat_duration=$(sed -n 's/TOTAL SIMULATION TIME ..... : //p' $stat_source)
	elif [[ $sim_name == "USE" ]]; then
		rm -r outputs
		rm lp_data/*.json
		echo "copying USE-sources in simulator-sources"
		rm -rf USE-model-sources
		cp -r ../USE-source USE-model-sources
		echo "tuning USE makefile to compile our model"
		stat_source="USE_output.txt"
		#getting sources from model makefile and adjusting their path and variable for the USE makefile in the USE-model-sources
		srcs=$(sed -E -e 's:SRCS:EDGE_SOURCES:' -n -e 's:[A-Za-z0-9_\./]+\.c:../&:gp' Makefile)
		#adding sources (first 3 expressions), adding list of .o files (4th expression), change the all target to 'edge' (5th expression), add the edge target (6th expression) and finally add the _edge target (7th expression)
		sed -i -e '431 i\'"$srcs"'' -i -e '539 i\EDGE_OBJ= $(EDGE_SOURCES:.c=.o)' -i -e '/all: .*/ c\all: edge' -i -e '560 i\edge: TARGET=edge\nedge: _edge executable' -i -e '661 i\_edge: $(EDGE_OBJ)\n\t@ld -r -g $(EDGE_OBJ) -o model/__application.o' USE-model-sources/Makefile

		# upping MAX_DATA_SIZE to 512
		sed -i -e 's/MAX_DATA_SIZE\t\t128/MAX_DATA_SIZE\t\t512/' USE-model-sources/include/events.h

		cd USE-model-sources

		#we use the free command to get only the available memory in RAM
		MAX_GIGAS=$(free -h | sed -E -e '1d' -e '3d' -e 's:[GMK]i::g' -e 's/Mem:[ ]+//' -e 's/[0-9\.]+[ ]+//g')
		ck=10
		pub=0.33333
		epb=3
		nbc=1
		lookahead=0.0
		dbg=0
		sperimental=1
		lin_pin=1
		reversible=0
		#1=stampe dettagliate
		report=1
		queue_len=1204
		max_lp=0

		make THR_POOL_SIZE=${queue_len} MAX_ALLOCABLE_GIGAS=${MAX_GIGAS} NBC=${nbc} MAX_SKIPPED_LP=${max_lp} REVERSIBLE=0 LOOKAHEAD=${lookahead} PERC_USED_BUCKET=${pub} ELEM_PER_BUCKET=${epb} REPORT=${report} DEBUG=${dbg} SPERIMENTAL=${sperimental} CHECKPOINT_PERIOD=${ck} LINEAR_PINNING=${lin_pin}

		cp edge ../simulation
		cd ..
		# we save the output so we can grab the stats whithout redirecting output
		$dbg_param ./simulation $working_threads $number_lp | tee $stat_source

		stat_used_mem=$(sed -E -n -e 's/Total allocated space\.+: //p' $stat_source)
		stat_duration=$(sed -E -n -e 's/Simulation ended \(seconds\):[ ]+//p' $stat_source)

	elif [[ $sim_name == "NeuRome" ]]; then
		rm -r outputs
		rm lp_data/*.json
		make
		if [[ -n $options ]]; then
		if [ "$options" == "parallel" ]; then
			rm -r outputs
			rm lp_data/*.json
			$dbg_param ./model_parallel
			fi
			else
				rm lp_data/*.json
				rm -r outputs
				$dbg_param ./model_serial
				fi
	fi
	#creating the json file with stats
		echo "{ \"run_type\": \"$run_type\", \"num_threads\": \"$working_threads\", \"used_mem\": \"$stat_used_mem\", \"duration\": \"$stat_duration seconds\", \"platform\":\"$sim_name\" }" > $stat_json
	# merging lpl jsons
	python3 generate_json.py $file_json
fi
