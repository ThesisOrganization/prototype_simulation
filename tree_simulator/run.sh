#!/bin/bash

#echo "simulation_time"
#simulation_time=400000
number_lp=""
file_json="simulation_results.json"
working_threads=$(nproc)
options=""

	if [[ -e "LP.txt" && -r "LP.txt" ]]; then
		number_lp=$(sed -n 2p LP.txt)
	else
		number_lp=$(sed -n 1p topology.txt)
	fi

for arg
do
	if [[ $arg == "valgrind" || $arg == "gdb" || $arg == "alone" || $arg == "json" || $arg == "clean" || $arg == "parallel" ]]; then
			options=$arg
	fi
	if [[ ${arg:0:5} == "--lp=" ]]; then
		number_lp=${arg#"--lp="}
	fi
	if [[ ${arg:0:5} == "--wt=" ]]; then
		working_threads=${arg#"--wt="}
	fi
done

if [[ -n $options ]]; then
if [ "$options" == "valgrind" ]; then
		valgrind --leak-check=full ./queues --sequential --lp $number_lp
elif [ "$options" == "gdb" ]; then
		gdb --args ./queues --wt 2 --lp $number_lp
elif [ "$options" == "alone" ]; then
		./queues --sequential --lp $number_lp
elif [ "$options" == "json" ]; then
		python3 generate_json.py $file_json
elif [ "$options" == "clean" ]; then
		rm lp_data/*.json
		rm -r outputs
elif [ "$options" == "parallel" ]; then
		rm -r outputs
		rm lp_data/*.json
		./queues --wt $working_threads --lp $number_lp
		python3 generate_json.py $file_json
		fi
else
		#./queues --sequential --lp $number_lp --simulation-time $simulation_time
		rm lp_data/*.json
		rm -r outputs
		./queues --sequential --lp $number_lp
		python3 generate_json.py $file_json
fi
