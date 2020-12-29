#!/bin/bash
source .venv/edgecomputing/bin/activate

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
lp_aggregation="--lp_aggregation_criteria=regional"
make_redirect=""
checkpoint_interval=""
working_threads=""

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
	elif [[ $arg == "-a" || $arg == "--analytical-model" ]]; then
		targets[${#targets[@]}]="analytical"
	elif [[ $arg == "-o" ]]; then
		targets[${#targets[@]}]="optimize"
	elif [[ $arg == "-s" || $arg == "--simulation" ]]; then
		targets[${#targets[@]}]="simulation"
		sim_options+="--run-complete "
	elif [[ $arg == "-r" || $arg == "--results" ]]; then
		targets[${#targets[@]}]="results"
	elif [[ $arg == "all" || $arg == "--all" ]]; then
		targets[${#targets[@]}]="all"
		options="all"
		sim_options+="--run-complete "
	elif [[ ${arg:0:29} == "--redir-compilation-messages=" ]]; then
		make_redirect=">>${arg#"--redir-compilation-messages="} 2>&1"
		sim_options+="$arg "
	elif [[ ${arg:0:6} == "--cat=" ]]; then
		catalog_path=${arg#'--cat='}
	elif [[ ${arg:0:17} == "--lp-aggregation=" ]]; then
		lp_aggregation="--lp_aggregation_criteria=${arg#"--lp-aggregation="}"
	elif [[ ${arg:0:6} == "--out=" ]]; then
		output_location=${arg#'--out='}
		sim_options+=$arg
	elif [[ $arg == "clean" ]]; then
		targets[${#targets[@]}]="clean"
		sim_options+="$arg "
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
		echo -e "ERROR: all and other execution options specified, use ONLY all or -g, -a, -s, -r."
		error="yes"
fi

if [[ ${#targets[@]} == 0 ]]; then
		echo -e "ERROR: no command specified, use at least one of these: -g -a -s -r --all.\n"
		error="yes"
fi
if [[ $quiet == "no" || $error == "yes" ]]; then
	echo -e "This script will test the tree_simulator model with the configuration provided in \"tests_topology\"."
	echo -e "\nThe script can generate the topology, run the analytical model, run the simulation model and present the result in a pdf located in \"pdf_result\". The pdf file name will be: h-m-s-simulator_used-run_type-results."
	echo -e "\nHowever to run any of these operations an argument needs to be given, see below in \"Execution options\"."
	echo -e "\nArgument list:\n
	\nGeneral options\n
\"-q --quiet\": suppress this message\n
\"--redir_compilation_messages=[path to file]\": redirect compilation messages to a file. The file will be used in append.\n
\"clean\": remove all the products of a previous run, including the output location and all object files.
\n Execution options\n
\"-g\": generate only the topology files\n
\"-o\":optimize simulation performance\n
\"-a\": only run the analytical model\n
\"-s\": run only the simulation model\n
\"-r\": create only the pdf with the results\n
\"all --all\":execute all the above steps (don't use this argument with the ones above)\n
\nData options:\n
		\"aggregated --aggregated -aggr\": in the pdf results, aggregate nodes that have similar characteristics (by default a 20% tolerance is used for aggreagtion)\n
	\"--lp_aggregation=[central,regional,local,lan]\": customizes how the elements are to be aggregated into LPs.
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

#converting into absolute path
if [[ $output_location =~ ^[^/].*$ ]]; then
	output_location="$(pwd)/$output_location"
fi
mkdir -p $output_location
for target in ${targets[@]}; do
	if [[ $target == "clean" ]]; then
	echo "Cleaning..."
	rm -rf $output_location
	make -C utils/partop clean >/dev/null
	make -C model_computation clean >/dev/null
	cd tree_simulator
	bash run.sh -q clean
	cd ..
	echo "Done."
	fi
	if [[ $target == "all" || $target == "generator" ]]; then
		echo "Starting generator..."
		python3 utils/generator/generator.py $catalog_path $topology_path $output_location
		err=$?
		if [[ $err != 0  ]]; then
			echo "error during topology generation, aborting."
			exit $err
		fi
		echo "Done."
		eval make -C utils/partop driverBinaries $make_redirect
		err=$?
		if [[ $err != 0  ]]; then
			echo "Error during compilation of topology binary files generator, aborting."
			exit $err
		fi
		echo "Setting up folders for binary files..."
		rm -rf $output_location"/bin"
		mkdir -p $output_location"/bin/gentop" $output_location"/bin/lptop"
		echo "Done."
		echo "Creating binary files..."
		./utils/partop/driverBinaries $output_location"/topology.txt" $output_location"/LP.txt" $output_location $lp_aggregation
		err=$?
		if [[ $err != 0  ]]; then
			echo "Error during generation of topology binary files, aborting."
			exit $err
		fi
		echo "Done."
	fi
	if [[ $target == "all" || $target == "analytical" ]]; then
		echo "Starting analytical model computation..."
		eval LOCATION="$output_location" make -C model_computation $make_redirect
		err=$?
		if [[ $err != 0  ]]; then
			echo "Error during compilation of analytical model, aborting."
			exit $err
		fi
		cd $output_location
		./analytical_model_computation
		err=$?
		cd $initial_location
		if [[ $err != 0  ]]; then
			echo "Error during analytical model computation, aborting."
			exit $err
		fi
		echo "Done."
	fi
	if [[ $target == "all" || $target == "optimize" ]]; then
		echo "Starting optimzation..."
		usr_timeout=""
		for opt in $sim_options; do
			if [[ ${opt:0:10} == "--timeout=" ]]; then
				usr_timeout=$opt
			fi
			if [[ ${opt:0:21} == "--simulation-timeout=" ]];then
				usr_timeout=$opt
			fi
		done
		mkdir -p $output_location/perf_traces/parallel
		mkdir -p $output_location/perf_traces/serial
		mkdir -p $output_location/perf_traces/bin
		mkdir -p $output_location/perf_traces/bin/gentop
		mkdir -p $output_location/perf_traces/bin/lptop
		ln -P $output_location/LP.txt -t $output_location/perf_traces
		ln -P $output_location/topology.txt -t $output_location/perf_traces
		ln -P $output_location/bin/lp*.bin -t $output_location/perf_traces/bin
		ln -P $output_location/bin/lptop/* -t $output_location/perf_traces/bin/lptop
		ln -P $output_location/bin/gentop/* -t $output_location/perf_traces/bin/gentop
		sim_options=${sim_options/--out=$output_location/}
		sim_options=${sim_options/--wt=[0-9]*/}
		sim_options=${sim_options/parallel/}
		sim_options=${sim_options/serial/}
		sim_options=${sim_options/--checkpoint-period=[0-9]*/}
		sim_options=${sim_options/--run-complete/}
		sim_options=${sim_options/--simulation-timeout=[0-9]*/}
		sim_options=${sim_options/--timeout=[0-9]*/}
		if [[ $sim_name == "USE" ]]; then
			timeout="--timeout=5"
		else
			timeout="--simulation-timeout=1000"
		fi

		cd $initial_location/tree_simulator
		echo "Getting serial trace..."
		bash run.sh -q $sim_name $timeout -c -e --redir-compilation-messages=/dev/null $sim_options serial --out=$output_location/perf_traces
		err=$?
		if [[ $err != 0 ]]; then
			echo Error during serial model tracing aborting
			rm -r $output_location/perf_traces
			exit $err
		fi
		if [[ $sim_name == "USE" ]]; then
			mv $output_location/perf_traces/USE_output.txt $output_location/perf_traces/serial/USE_output.txt
		else
			mkdir -p $output_location/perf_traces/serial/outputs
			mv $output_location/perf_traces/outputs/sequential_stats $output_location/perf_traces/serial/outputs/sequential_stats
		fi
		echo "Getting parallel trace..."
		bash run.sh -q $sim_name $timeout -c -e $sim_options parallel --wt=$(nproc) --redir-compilation-messages=/dev/null --out=$output_location/perf_traces
		err=$?
		if [[ $err != 0 ]]; then
			echo Error during parallel model tracing aborting
			rm -r $output_location/perf_traces
			exit $err
		fi
		if [[ $sim_name == "USE" ]]; then
			mv $output_location/perf_traces/USE_output.txt $output_location/perf_traces/parallel/USE_output.txt
		else
			mkdir -p $output_location/perf_traces/parallel/outputs
			mv $output_location/perf_traces/outputs/execution_stats $output_location/perf_traces/parallel/outputs/execution_stats
		fi
		rm -r $output_location/perf_traces/bin $output_location/perf_traces/LP.txt $output_location/perf_traces/topology.txt
		make clean > /dev/null #we remove all the traces of previous compilations
		cd $initial_location
		cd utils/tuning
		echo "Executing optimization..."
		python3 sim_optimize.py $sim_name $output_location $output_location $output_location $output_location/perf_traces/serial $output_location/perf_traces/parallel
		cd $initial_location

		opt_params=$(cat $output_location/sim_config.json)
		threads=${opt_params#*\"Chosen_threads\":}
		threads=${threads/"}"/}
		ckp_period=${opt_params#*\"Checkpoint_period_opt\":}
		ckp_period=${ckp_period/,*/}
		if [[ $threads > 1 ]]; then
			t_arg="parallel --wt=$threads"
		else
			t_arg="serial"
		fi
		sim_options+=" --run-complete --out=$output_location $t_arg --checkpoint-period=$ckp_period $usr_timeout"
		echo $sim_options
		echo "Done."
	fi
	if [[ $target == "all" || $target == "simulation" ]]; then
		echo "Starting simulation..."
		cd tree_simulator
		bash run.sh $sim_options
		err=$?
		cd ..
		if [[ $err != 0  ]]; then
			echo "Error during model simulation, aborting."
			echo $err
			exit $err
		fi
		echo "Done."
	fi
	if [[ $target == "all" || $target == "results" ]]; then
		echo "Parsing jsons and merging them..."
		if [[ $pdf_options == "-a" ]]; then
			echo "...aggregated results."
			python3 jsonMerger/jsonParse.py -a $output_location $output_location $output_location $output_location
		else
			echo "...standard."
			python3 jsonMerger/jsonParse.py $output_location $output_location $output_location $output_location
		fi
		err=$?
		if [[ $err != 0  ]]; then
			echo "Error during data parsing, aborting."
			exit $err
		fi
		echo "Done."
		cd $output_location
		echo "Creating pdf..."
		eval pdflatex complete_results.tex $make_redirect
		err=$?
		rm -f complete_results.log complete_results.aux
		if [[ $err != 0  ]]; then
			echo "Error during document creation, aborting."
			exit $err
		fi
		echo "Done."
		echo "Moving and renaming pdf..."
		res_name=$(date +%H_%M_%S)-$sim_name-$run_type-complete_results.pdf
		if [[ $output_location =~ ^.*tree_simulator_bin$ ]]; then
			output_location="$initial_location/pdf_results"
		fi
		mv complete_results.pdf $output_location/$res_name
		if [[ $err != 0  ]]; then
			echo "Error during document rename, aborting."
			exit $err
		fi
		echo "Done, results are in file $res_name located in the \"$output_location\" folder."
		cd $initial_location
	fi
done
