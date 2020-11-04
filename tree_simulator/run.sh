#!/bin/bash

#echo "simulation_time"
#simulation_time=400000
number_lp=38
file_output="output.txt"
#file_latex="simulation_results.tex"
file_json="simulation_results.json"


if [ -n "$2" ]; then
    number_lp=$2
fi

#if [ -n "$3" ]; then
#    simulation_time=$3
#fi

if [ "$1" == "valgrind" ]; then
    valgrind --leak-check=full ./queues --sequential --lp $number_lp
elif [ "$1" == "gdb" ]; then
    gdb --args ./queues --wt 2 --lp $number_lp
elif [ "$1" == "print" ]; then
    ./queues --sequential --lp $number_lp
elif [ "$1" == "alone" ]; then
    ./queues --sequential --lp $number_lp
elif [ "$1" == "json" ]; then
    python3 generate_json.py $file_json
elif [ "$1" == "clean" ]; then
    rm lp_data/*.json
elif [ "$1" == "parallel" ]; then
    rm lp_data/*.json
    ./queues --wt 2 --lp $number_lp
    python3 generate_json.py $file_json
else
    #./queues --sequential --lp $number_lp --simulation-time $simulation_time
    rm lp_data/*.json
    ./queues --sequential --lp $number_lp
    python3 generate_json.py $file_json
fi
