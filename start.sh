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
if [ -n "$1" ]; then
    echo "...with custom LPS."
    ./run.sh run $1
else
  echo "...in standard execution."
  ./run.sh
fi
echo "Done."
echo "Parsing jsons and merging them.."
cd ../jsonMerger
python3 jsonParse.py
echo "Done."
echo "Creating pdf.."
pdflatex complete_results.tex
echo "Done."
