#!/bin/bash
quiet="no"
PREFIX=$PWD
ROOTSIM_ARGS=""
NeuRome_args=""
sim_name="all"
debugging="no"
for arg
do
	if [[ $arg == "-q" || $arg == "--quiet" ]]; then
		quiet="yes"
	fi
	if [[ $arg == "debug" || $arg == "debug-extra" ]]; then
		ROOTSIM_ARGS+=" --enable-debug"
		NeuRome_args+="-Dbuildtype=debug"
		debugging="yes"
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
	echo "This script will download and setup ROOT-Sim, USE and NeuRome simulators for the current machine"
	echo "A single simulator can be chosen by passing its name (ROOT-Sim, USE or NeuRome/ROOT-Sim3) as an argument, by default this script will setup all of them."
	echo "The simulators will be found in the \"./Simulators\" folder, according to the simulator name and type of installation."
	echo -e "When setting up ROOT-Sim some arguments can be used to customize the installation:\n
	 \"debug\": enable debug symbols\n
	 \"debug-extra\": extra checks during debug (kills performance)\n
	 \"mpi\": enable mpi\n
	 \"no-rebinding\": disable LP rebinding\n
	 \"profile\": enable performance reports\n"
	echo -e "When setting up NeuRome the following arguments can be used:\n
	\"debug\": enable debug symbols"
	echo "To disable this message run the script with -q or --quiet"
	echo -e "\nUnrecognized arguments will be ignored."
	read -n1 -r -p "Press any key to continue or CTRL+C to exit" key
fi

echo "Starting setup and installation in the Simulators folder."
mkdir -p Simulators
if [[ $sim_name == "all" || $sim_name == "ROOT-Sim" ]]; then
	echo "Setting up ROOT-Sim"
	echo "We assume to have all the prerequisites installed (git,autotools, autoconf, automake, make, gcc and libtoolize)."

	git clone https://github.com/HPDCS/ROOT-Sim Simulators/ROOT-Sim-source
	mkdir -p Simulators/ROOT-Sim-bin
	cd Simulators/ROOT-Sim-source
	sh autogen.sh
	./configure $ROOTSIM_ARGS --prefix=$PREFIX/Simulators/ROOT-Sim-bin
	make install
	cd ../..
	echo "writing the compatiblity header in \"Simulators/ROOT-Sim-bin/include/compatibility.h\""
	header="#ifndef COMPATIBILITY_H\n
	#define COMPATIBILITY_H\n
	///blank for ROOT-Sim\n
	#endif"
	echo -e $header > Simulators/ROOT-Sim-bin/include/compatibility.h
	echo "done"
fi
if [[ $sim_name == "all" || $sim_name == "USE" ]]; then
	echo "Setting up USE"
	echo "Needed dependencies: libcap2, libcap-dev, g++, gcc, gdb, make, libnuma-dev, git, time, linux-tools-`uname -r`, linux-cloud-tools-`uname -r`"

	git clone https://github.com/HPDCS/USE.git Simulators/USE
	cd Simulators/USE
	git checkout iPiDES_new
	cd ../..
	echo "writing the compatiblity header in \"Simulators/USE/include/compatibility.h\""
	header="#ifndef COMPATIBILITY_H\n
	#define COMPATIBILITY_H\n
	///blank for USE\n
	#endif"
	echo -e $header > Simulators/USE/include/compatibility.h
	echo "done"
fi
if [[ $sim_name == "all" || $sim_name == "NeuRome" ]]; then
	echo "setting up NeuRome"
	echo "We assume to have all the prerequisites installed (git,gcc>=8,meson)."

	git clone https://github.com/ROOT-Sim/core.git Simulators/NeuRome-source
	mkdir -p Simulators/NeuRome-bin
	cd Simulators/NeuRome-source
	git checkout release-3.0.0
	if [[ $debugging == "yes" ]]; then
		sed -i 's/-O3/-O0 -g/' NeuRome-source/compiler.c
	else
		sed -i 's/-O0 -g/-O3/' NeuRome-source/compiler.c
	fi
	meson build -Dprefix=$PREFIX/Simulators/NeuRome-bin $NeuRome_args
	cd build
	ninja test
	ninja install
	cd ../../..
	echo "writing the compatiblity header in \"Simulators/NeuRome-bin/include/compatibility.h\""
	header="#ifndef COMPATIBILITY_H\n
	#define COMPATIBILITY_H\n
	#define n_prc_tot n_lps\n
	#define OnGVT CanEnd\n
	#endif"
	echo -e $header > Simulators/NeuRome-bin/include/compatibility.h
	echo "done"
fi
echo "setup complete"
