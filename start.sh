#!/bin/bash
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
./run.sh run $1
echo "Done."
echo "Parsing jsons and merging them.."
cd ../jsonMerger
if [ "$2" == "aggregated" ]; then
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
