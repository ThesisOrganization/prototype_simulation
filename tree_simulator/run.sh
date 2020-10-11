#!/bin/bash

#echo "simulation_time"
simulation_time=5000
number_lp=30
file_output="output.txt"


if [ -n "$2" ]; then
    number_lp=$2
fi

if [ -n "$3" ]; then
    simulation_time=$3
fi

if [ "$1" == "valgrind" ]; then
    valgrind --leak-check=full ./queues --sequential --lp $number_lp --simulation-time $simulation_time
elif [ "$1" == "gdb" ]; then
    gdb --args ./queues --sequantial --lp $number_lp --simulation-time $simulation_time
elif [ "$1" == "print" ]; then
    ./queues --sequential --lp $number_lp --simulation-time $simulation_time
else
    ./queues --sequential --lp $number_lp --simulation-time $simulation_time > $file_output
    python3 print_sorting.py $file_output
fi
