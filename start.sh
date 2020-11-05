#!/bin/bash

sim_options=""
number_lp=""
working_threads=""
pdf_options=""

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
done

echo "Starting generator.."
cd utils/generator
python3 generator.py
echo "Done."
echo "Starting analytical model computation.."
cd ../../model_computation
make
./formulas
echo "Done."
echo "Starting simulation.."
cd ../tree_simulator
make
./run.sh $sim_options $number_lp $working_threads
echo "Done."
echo "Parsing jsons and merging them.."
cd ../jsonMerger
if [[ $pdf_options == "aggregated" ]]; then
  echo "...aggregated results."
  python3 jsonParseAggregation.py
else
  echo "...standard."
  python3 jsonParse.py
fi
echo "Done."
echo "Creating pdf.."
pdflatex complete_results.tex
echo "Done."
