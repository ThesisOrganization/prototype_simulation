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
cd ..
touch Simulator/simulator-name
echo "NeuRome" > Simulator/simulator-name
echo "writing the compatiblity header"
echo "/** \file compatibility.h
 * This file contains definition used to make the model agnostic to the simulator that is running them
 * */
#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H
#define n_prc_tot n_lps
#endif" > Simulator/compatibility.h
echo "setup complete"
