#! /bin/sh
#This script will download and setup the rootsim simulator for the current machine

PREFIX=$PWD
ARGS=""

for arg
do
		if [[ $arg == "debug" || $arg == "debug-extra" ]]; then
		ARGS+=" --enable-debug"
	fi
	if [[ $arg == "debug-extra" ]]; then
		ARGS+=" --enable-extra-checks"
	fi
	if [[ $arg == "mpi" ]]; then
		ARGS+=" --enable-mpi"
	fi
	if [[ $arg == "no-rebinding" ]]; then
		ARGS+=" --disable-rebinding"
	fi
	if [[ $arg == "profile" ]]; then
		ARGS+=" --enable-profile"
	fi
done

echo "Starting Root-Sim setup and installation."
echo "We assume to have all the prerequisites installed (git,autotools, autoconf, automake, make, gcc and libtoolize)."
echo "The executable will be found in the ROOT-Sim-bin folder"
echo -e "Argument list:\n \"debug\": enable debug symbols\n \"debug-extra\": extra checks during debug (kills performance)\n \"mpi\": enable mpi\n \"no-rebinding\": disable LP rebinding\n \"profile\": enable performance reports\n"

sleep 2
rm -fr Simulator
git clone https://github.com/HPDCS/ROOT-Sim ROOT-Sim-source
mkdir Simulator
cd ROOT-Sim-source
sh autogen.sh
./configure $ARGS --prefix=$PREFIX/Simulator
make install
cd ..
echo "ROOT-Sim" > Simulator/simulator-name
