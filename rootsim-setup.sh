#! /bin/sh
#This script will download and setup the rootsim simulator for the current machine

PREFIX=$PWD

echo "Starting Root-Sim setup and installation."
echo "We assume to have all the prerequisites installed (git,autotools, autoconf, automake, make, gcc and libtoolize)."
echo "The executable will be found in the ROOT-Sim-bin folder"
echo ""

git clone https://github.com/HPDCS/ROOT-Sim ROOT-Sim-source
mkdir ROOT-Sim-bin
cd ROOT-Sim-source
sh autogen.sh
./configure --prefix=$PREFIX/ROOT-Sim-bin
make install
cd ..
