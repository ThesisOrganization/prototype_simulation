#!/bin/bash

platform=$(sed -n '1 p' Simulator/simulator-name)
sim_options="-q "
pdf_options=""
aggr_coeff=""
number_test=""
quiet="no"

for arg
do
	if [[ $arg == "--aggregated" || $arg == "-a" ]]; then
			pdf_options=$arg
	fi
	if [[ ${arg:0:11} == "--sim_coef=" ]]; then
		aggr_coeff="-sim_coef "+${arg#"--sim_coef="}
	fi
	if [[ ${arg:0:5} == "--sc=" ]]; then
		aggr_coeff="-sim_coef "+${arg#"--sc="}
	fi
	if [[ $arg == "-q" || $arg == "--quiet" ]]; then
		quiet="yes"
	fi
	sim_options+=$arg" "
done

if [[ $quiet == "no" ]]; then
	echo "This script will test the tree_simulator model with the configurations provided in \"tests_topology\"."
	echo -e "Argument list:\n \"-q --quiet\": suppress this message\n \"aggregated\": to aggregate nodes that have similar characteristics (by default ver have a 20% tolerance)\n \"-sc= --sim_coef=\": to define a custom tolerance value (as a number between 0 and 1) for the aggregation\n all the arguments that can be passed to \"tree_simulator/run.sh\" are supported, here we will provide only a brief list of the most important ones:\n  \"--wt=\":number of threads\n  \"--lp=\":number of lps\n  \"gdb\": run the simulation under gdb\n  \"valgrind\": run the simulation under valgrind\n  \"parallel\": run the simulation with more than one thread\n  \"serial\": run the simulation with only one thread (default choice)"
	read -n1 -r -p "Press any key to continue or CTRL+C to exit" key
fi

for filename in tests_topology/*.txt; do
	echo "Starting test.."
	echo "Starting generator.."
	cd utils/generator
	python3 generator.py "$filename"
	echo "Done."
	echo "Starting analytical model computation.."
	cd ../../model_computation
	make
	./formulas
	echo "Done."
	echo "Starting simulation.."
	cd ../tree_simulator
	bash run.sh $sim_options
	echo "Done."
	echo "Parsing jsons and merging them.."
	cd ../jsonMerger
	if [[ $pdf_options == "aggregated" ]]; then
	  echo "...aggregated results."
	  python3 jsonParse.py -a $aggr_coeff
	else
	  echo "...standard."
	  python3 jsonParse.py
	fi
	echo "Done."
	echo "Creating pdf.."
	pdflatex complete_results.tex
	echo "Done."
	echo "Moving and renaming pdf.."
	mv complete_results.pdf ../pdf_results/$(date +%H_%M_%S)-$platform-complete_results.pdf
	echo "Done."
	cd ..
done
