#!/bin/bash

sim_options=""
number_lp=""
working_threads=""
pdf_options=""
aggr_coeff=""
number_test=""

for arg
do
	if [[ $arg == "parallel" ]]; then
			sim_options=$arg
	fi
	if [[ $arg == "aggregated" ]]; then
			pdf_options=$arg
	fi
	if [[ ${arg:0:5} == "--lp=" ]]; then
		number_lp=$arg
	fi
	if [[ ${arg:0:5} == "--wt=" ]]; then
		working_threads=$arg
	fi
	if [[ ${arg:0:11} == "--sim_coef=" ]]; then
		aggr_coeff="-sim_coef "+${arg#"--sim_coef="}
	fi
done

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
	make clean
	make
	./run.sh $sim_options $number_lp $working_threads
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
	mv complete_results.pdf ../pdf_results/$(date +%H_%M_%S)-complete_results.pdf
	echo "Done."
	cd ..
done
