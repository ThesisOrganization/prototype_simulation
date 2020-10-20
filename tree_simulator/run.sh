#!/bin/bash

#echo "simulation_time"
simulation_time=400000
number_lp=22
file_output="output.txt"
file_latex="simulation_results.tex"


if [ -n "$2" ]; then
    number_lp=$2
fi

if [ -n "$3" ]; then
    simulation_time=$3
fi

if [ "$1" == "valgrind" ]; then
    valgrind --leak-check=full ./queues --sequential --lp $number_lp --simulation-time $simulation_time
elif [ "$1" == "gdb" ]; then
    gdb --args ./queues --sequential --lp $number_lp --simulation-time $simulation_time
elif [ "$1" == "print" ]; then
    ./queues --sequential --lp $number_lp --simulation-time $simulation_time
elif [ "$1" == "pdf" ]; then
    python3 generate_pdf.py $file_output $file_latex
    pdflatex $file_latex
else
    ./queues --sequential --lp $number_lp --simulation-time $simulation_time > $file_output
    #python3 print_sorting.py $file_output
fi
