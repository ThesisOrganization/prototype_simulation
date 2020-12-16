#!/bin/bash

run_type="serial"
sim_name=""
sim_options="-q "
pdf_options=""
aggr_coeff=""
number_test=""
quiet="no"
topology_path="tests_topology/config.txt"
catalog_path="tests_topology/catalog"
output_location="tree_simulator_bin"
targets=()
error="no"
initial_location=$(pwd)
options=""

for arg
do
	if [[ $arg == "aggregated" || $arg == "--aggregated" || $arg == "-aggr" ]]; then
		pdf_options="-a"
	elif [[ ${arg:0:11} == "--sim_coef=" ]]; then
		aggr_coeff="-sim_coef "+${arg#"--sim_coef="}
	elif [[ ${arg:0:5} == "--sc=" ]]; then
		aggr_coeff="-sim_coef "+${arg#"--sc="}
	elif [[ $arg == "-q" || $arg == "--quiet" ]]; then
		quiet="yes"
	elif [[ ${arg:0:6} == "--top=" ]]; then
		topology_path=${arg#"--top="}
	elif [[ $arg == "-g" || $arg == "--generator" ]]; then
		targets[${#targets[@]}]="generator"
	elif [[ $arg == "-a" || $arg == "--analytical_model" ]]; then
		targets[${#targets[@]}]="analytical model"
	elif [[ $arg == "-s" || $arg == "--simulation" ]]; then
		targets[${#targets[@]}]="simulation"
		sim_options+="--run-complete "
	elif [[ $arg == "-r" || $arg == "--results" ]]; then
		targets[${#targets[@]}]="results"
	elif [[ $arg == "all" || $arg == "--all" ]]; then
		targets[${#targets[@]}]="all"
		options="all"
		sim_options+="--run-complete "
	elif [[ ${arg:0:6} == "--cat=" ]]; then
		catalog_path=${arg#'--cat='}
	elif [[ ${arg:0:6} == "--out=" ]]; then
		output_location=${arg#'--out='}
		sim_options+=$arg
	elif [[ $arg == "clean" ]]; then
		targets[${#targets[@]}]="clean"
		sim_options+=$arg
	else
		sim_options+=$arg" "
		arg=${arg//'-'/''}
		arg=${arg,,}
		if [[ $arg == "rootsim" ]]; then
			sim_name="ROOT-Sim"
		elif [[ $arg == "rootsim3" || $arg == "neurome" ]]; then
			sim_name="NeuRome"
		elif [[ $arg == "use" ]]; then
			sim_name="USE"
		fi
	fi
	if [[ $arg == "parallel" ]]; then
		run_type="parallel"
	fi
done

if [[ ${#targets[@]} > 1 && $options == "all" ]]; then
		echo -e "ERROR: all and other execution options specified, use ONLY all or -g, -a, -s, -r"
		error="yes"
fi

if [[ ${#targets[@]} == 0 ]]; then
		echo -e "ERROR: no command specified, use at least one of these: -g -a -s -r --all\n"
		error="yes"
fi
if [[ $quiet == "no" || $error == "yes" ]]; then
	echo -e "This script will test the tree_simulator model with the configuration provided in \"tests_topology\"."
	echo -e "\nThe script can generate the topology, run the analytical model, run the simulation model and present the result in a pdf located in \"pdf_result\". The pdf file name will be: h-m-s-simulator_used-run_type-results"
	echo -e "\nHowever to run any of these operations an argument needs to be given, see below in \"Execution options\"."
	echo -e "\nArgument list:\n
	\nGeneral options\n
\"-q --quiet\": suppress this message\n
\"clean\": remove all the products of a previous run, including the output location and all object files.
\n Execution options\n
\"-g\": generate only the topology files\n
\"-a\": only run the analytical model\n
\"-s\": run only the simulation model\n
\"-r\": create only the pdf with the results\n
\"all --all\":execute all the above steps (don't use this argument with the ones above)\n
\nData options:\n
		\"aggregated --aggregated -aggr\": in the pdf results, aggregate nodes that have similar characteristics (by default a 20% tolerance is used for aggreagtion)\n
	\"--cat=\": path to the catalog folder that stores element types, defaults to \"tests_topology/catalog\"\n
	\"--top=\": path to the topology file from the \"rootsim-models\" folder; if this argument is not specified the the default path will be \"tests_topology/config.txt\"\n
	\"-sc= --sim_coef=\": to define a custom tolerance value (as a number between 0 and 1) for the results aggregation\n
	\"--out=\": location where all the produced files (data and executables) will be located, defaults to \"tree_simulator_bin\"\n
\nSimulation options:
All the arguments that can be passed to \"tree_simulator/run.sh\" are supported, here we will provide only a brief list of the most important ones:\n
	\"--wt=\":number of threads\n
	\"--lp=\":number of lps\n
	\"gdb\": run the simulation under gdb\n
	\"valgrind\": run the simulation under valgrind\n
	\"parallel\": run the simulation with more than one thread\n
	\"serial\": run the simulation with only one thread (default choice)\n
	\"NeuRome\": use NeuRome as a simulation platform.\n
	\"ROOT-Sim\": use ROOT-Sim as a simulation platform.\n
	\"USE\": use USE as a simulation platform.\n"
	echo -e "\nUnrecognized arguments will be ignored."
fi
if [[ $error == "yes" ]];then
	exit 255
else
	if [[ $quiet == "no" ]]; then
		read -n1 -r -p "Press any key to continue or CTRL+C to exit" key
	fi
fi

for target in ${targets[@]}; do
	if [[ $target == "clean" ]]; then
	echo "cleaning"
	rm -rf $output_location
	make -C utils/partop clean
	make -C model_computation clean
	cd tree_simulator
	bash run.sh $sim_options
	cd ..
	echo "Done"
	fi
	if [[ $target == "all" || $target == "generator" ]]; then
		echo "Starting generator.."
		python3 utils/generator/generator.py $catalog_path $topology_path $output_location
		err=$?
		if [[ $err != 0  ]]; then
			echo "error during topology generation, aborting"
			exit $err
		fi
		echo "Done."
		echo "Generating topology binary files"
		make -C utils/partop driverBinaries
		err=$?
		if [[ $err != 0  ]]; then
			echo "error during compilation of topology binary files generator, aborting"
			exit $err
		fi
		echo "setting up folders for binary files"
		rm -rf $output_location"/bin"
		mkdir -p $output_location"/bin/gentop" $output_location"/bin/lptop"
		echo "Done"
		echo "Creating binary files"
		./utils/partop/driverBinaries $output_location"/topology.txt" $output_location"/LP.txt" $output_location
		echo "Done"
		err=$?
		if [[ $err != 0  ]]; then
			echo "error during generation of topology binary files, aborting"
			exit $err
		fi
	fi
	if [[ $target == "all" || $target == "analytical model" ]]; then
		echo "Starting analytical model computation.."
		make LOCATION=$output_location -C model_computation
		err=$?
		if [[ $err != 0  ]]; then
			echo "error during compilation of analytical model, aborting"
			exit $err
		fi
		cd $output_location
		./analytical_model_computation
		err=$?
		cd $initial_location
		if [[ $err != 0  ]]; then
			echo "error during analytical model computation, aborting"
			exit $err
		fi
		echo "Done."
	fi
	if [[ $target == "all" || $target == "simulation" ]]; then
		echo "Starting simulation.."
		cd tree_simulator
		bash run.sh $sim_options
		err=$?
		cd ..
		if [[ $err != 0  ]]; then
			echo "error during model simulation, aborting"
			exit $err
		fi
		echo "Done."
	fi
	if [[ $target == "all" || $target == "results" ]]; then
		echo "Parsing jsons and merging them.."
		if [[ $pdf_options == "-a" ]]; then
			echo "...aggregated results."
			python3 jsonMerger/jsonParse.py -a $output_location $output_location $output_location $output_location
		else
			echo "...standard."
			python3 jsonMerger/jsonParse.py $output_location $output_location $output_location $output_location
		fi
		err=$?
		if [[ $err != 0  ]]; then
			echo "error during data parsing, aborting"
			exit $err
		fi
		echo "Done."
		cd $output_location
		echo "Creating pdf.."
		pdflatex complete_results.tex
		err=$?
		rm -f complete_results.log complete_results.aux
		cd $initial_location
		if [[ $err != 0  ]]; then
			echo "error during document creation, aborting"
			exit $err
		fi
		echo "Done."
		echo "Moving and renaming pdf.."
		res_name=$(date +%H_%M_%S)-$sim_name-$run_type-complete_results.pdf
		if [[ $output_location == "tree_simulator_bin" ]]; then
			output_location="pdf_result"
		fi
		mv complete_results.pdf $output_location/$res_name
		if [[ $err != 0  ]]; then
			echo "error during document rename, aborting"
			exit $err
		fi
		echo "Done, results are in file $res_name located in the \"pdf_results\" folder"
		cd ..
	fi
done
