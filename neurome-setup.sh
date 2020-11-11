#! /bin/sh
#This script will download and setup the rootsim simulator for the current machine

PREFIX=$PWD

echo "Starting NeuRome setup and installation."
echo "We assume to have all the prerequisites installed (git,gcc>=8,meson)."

sleep 2
rm -rf Simulator
git clone https://github.com/ROOT-Sim/core.git NeuRome-source
mkdir Simulator
cd NeuRome-source
git checkout release-3.0.0
meson build -Dprefix=$PREFIX/Simulator
cd build
ninja test
ninja install
cd ..
