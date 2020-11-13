#!/bin/bash

#echo "simulation_time"
#simulation_time=400000
number_lp=""
file_json="simulation_results.json"
working_threads=$(nproc)
options=""
dbg_arg=""
dbg_param=""
sim_name=$(sed -n 1p ../Simulator/simulator-name)
echo $sim_name

	if [[ -e "LP.txt" && -r "LP.txt" ]]; then
		number_lp=$(sed -n 2p LP.txt)
	else
		number_lp=$(sed -n 1p topology.txt)
	fi

for arg
do
	if [[ $arg == "alone" || $arg == "json" || $arg == "clean" || $arg == "parallel" ]]; then
			options=$arg
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
elif [[ $sim_name == "ROOT-Sim" ]]; then
	echo "Compiling and running model with ROOT-Sim"
	make
	if [[ -n $options ]]; then

	if [ "$options" == "alone" ]; then
			./simulation --sequential --lp $number_lp
elif [ "$options" == "parallel" ]; then
			rm -r outputs
			rm lp_data/*.json
			$dbg_param ./simulation --wt $working_threads --lp $number_lp
			python3 generate_json.py $file_json
			fi
	else
			#./queues --sequential --lp $number_lp --simulation-time $simulation_time
			rm lp_data/*.json
			rm -r outputs
			$dbg_param ./simulation --sequential --lp $number_lp
			python3 generate_json.py $file_json
	fi
elif [[ $sim_name == "USE" ]]; then
	echo "Compiling and running model with USE"
	echo "copiyng USE-sources in simulator-sources"
	cp -r ../USE-source USE-model-sources
	echo "tuning USE makefile to compile our model"

	#getting sources from model makefile and adjusting their path and variable for the USE makefile in the USE-model-sources
	srcs=$(sed -E -e 's:SRCS:EDGE_SOURCES:' -n -e 's:[A-Za-z0-9_\./]+\.c:../&:gp' Makefile)
	#adding sources (first 3 expressions), adding list of .o files (4th expression), change the all target to 'edge' (5th expression), add the edge target (6th expression) and finally add the _edge target (7th expression)
	sed -i -e '431 i\'"$srcs"'' -i -e '539 i\EDGE_OBJ= $(EDGE_SOURCES:.c=.o)' -i -e '/all: .*/ c\all: edge' -i -e '560 i\edge: TARGET=edge\nedge: _edge executable' -i -e '661 i\_edge: $(EDGE_OBJ)\n\t@ld -r -g $(EDGE_OBJ) -o model/__application.o' USE-model-sources/Makefile

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

	make MAX_ALLOCABLE_GIGAS=${MAX_GIGAS} NBC=${nbc} MAX_SKIPPED_LP=${max_lp} REVERSIBLE=${reversible} LOOKAHEAD=${lookahead} PERC_USED_BUCKET=${pub} ELEM_PER_BUCKET=${epb} REPORT=${report} DEBUG=${dbg} SPERIMENTAL=${sperimental} CHECKPOINT_PERIOD=${ck} LINEAR_PINNING=${lin_pin}

	cp edge ../simulation
	cd ..
	$dbg_param ./simulation $working_threads $number_lp
fi
