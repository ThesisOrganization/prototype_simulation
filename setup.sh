#!/bin/bash
quiet="no"
PREFIX=$PWD
ROOTSIM_ARGS=""
sim_name="ROOT-Sim"

for arg
do
	if [[ $arg == "-q" || $arg == "--quiet" ]]; then
		quiet="yes"
	fi
	if [[ $arg == "debug" || $arg == "debug-extra" ]]; then
		ROOTSIM_ARGS+=" --enable-debug"
	fi
	if [[ $arg == "debug-extra" ]]; then
		ROOTSIM_ARGS+=" --enable-extra-checks"
	fi
	if [[ $arg == "mpi" ]]; then
		ROOTSIM_ARGS+=" --enable-mpi"
	fi
	if [[ $arg == "no-rebinding" ]]; then
		ROOTSIM_ARGS+=" --disable-rebinding"
	fi
	if [[ $arg == "profile" ]]; then
		ROOTSIM_ARGS+=" --enable-profile"
	fi
	arg=${arg//'-'/''}
	if [[ ${arg,,} == "rootsim" ]]; then
		sim_name="ROOT-Sim"
	fi
	if [[ ${arg,,} == "use" ]]; then
		sim_name="USE"
	fi
	if [[ ${arg,,} == "rootsim3" || ${arg,,} == "neurome" ]]; then
		sim_name="NeuRome"
	fi
done

if [[ $quiet == "no" ]]; then
	echo "This script will download and setup the chosen simulator for the current machine"
	echo "In addition this script will remove objects files from all the model folders, to prevent issues when changing simulators"
	echo "The simulator can be chosen by passing its name (ROOT-Sim, USE or NeuRome/ROOT-Sim3) as an argument, by default this script will setup ROOT-Sim."
	echo "The simulator will be found in the \"./Simulator\" folder."
	echo -e "When setting up ROOT-Sim some arguments can be used to customize the installation:\n \"debug\": enable debug symbols\n \"debug-extra\": extra checks during debug (kills performance)\n \"mpi\": enable mpi\n \"no-rebinding\": disable LP rebinding\n \"profile\": enable performance reports\n"
	echo "To disable this message run the script with -q or --quiet"
	read -n1 -r -p "Press any key to continue or CTRL+C to exit" key
fi

echo "Starting $sim_name setup and installation in the Simulator folder."
rm -fr Simulator
if [[ $sim_name == "ROOT-Sim" ]]; then
	echo "We assume to have all the prerequisites installed (git,autotools, autoconf, automake, make, gcc and libtoolize)."

	git clone https://github.com/HPDCS/ROOT-Sim ROOT-Sim-source
	mkdir Simulator
	cd ROOT-Sim-source
	sh autogen.sh
	./configure $ARGS --prefix=$PREFIX/Simulator
	make install
	cd ..

elif [[ $sim_name == "USE" ]]; then
	echo "Needed dependencies: libcap2, libcap-dev, g++, gcc, gdb, make, libnuma-dev, git, time, linux-tools-`uname -r`, linux-cloud-tools-`uname -r`"

	git clone https://github.com/HPDCS/USE.git USE-source
	cd USE-source
	git checkout iPiDES_new
	cd ..
	cp -r USE-source Simulator

elif [[ $sim_name == "NeuRome" ]]; then

	echo "We assume to have all the prerequisites installed (git,gcc>=8,meson)."

	git clone https://github.com/ROOT-Sim/core.git NeuRome-source
	mkdir Simulator
	cd NeuRome-source
	git checkout release-3.0.0
	meson build -Dprefix=$PREFIX/Simulator
	cd build
	ninja test
	ninja install
	cd ..
	cd ..

fi
echo "writing the compatiblity header in \"Simulator/compatibility.h\""
header="#ifndef COMPATIBILITY_H\n
#define COMPATIBILITY_H\n
///Max len of for the topology.txt and LP.txt file, to avoid having external library functions that allocate memory. (eg. getline)\n
#define MAX_LINE_LEN 128\n"
if [[ $sim_name == "NeuRome" ]]; then
	header+="#define n_prc_tot n_lps\n"
fi
header+="#endif"
echo -e $header > Simulator/compatibility.h
echo "Writing the simulator name in \"Simulator/simulator-name\""
echo $sim_name > Simulator/simulator-name
echo "setup complete"
echo "cleaning models"
for model in {"tree_simulator","three_queues","classes_queues"}; do
	echo "cleaning $model model"
	cd $model
	make clean
	cd ..
done
