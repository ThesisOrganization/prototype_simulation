echo "This script will configure USE"
echo "Needed dependencies: libcap2, libcap-dev, g++, gcc, gdb, make, libnuma-dev, git, time, linux-tools-`uname -r`, linux-cloud-tools-`uname -r`"

git clone https://github.com/HPDCS/USE.git USE-source
cd USE-source
git checkout iPiDES_new
touch simulator-name
cd ..
rm -rf Simulator
cp -r USE-source Simulator
touch Simulator/simulator-name
echo "USE" > Simulator/simulator-name

echo "writing the compatiblity header (blank for now)"
echo "/** \file compatibility.h
 * This file contains definition used to make the model agnostic to the simulator that is running them
 * */
#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H
#endif" > Simulator/compatibility.h
echo "setup complete"
