echo "This script will configure USE"
echo "Needed dependencies: libcap2, libcap-dev, g++, gcc, gdb, make, libnuma-dev, git, time, linux-tools-`uname -r`, linux-cloud-tools-`uname -r`"

git clone https://github.com/HPDCS/USE.git USE-source
cd USE-source
git checkout iPiDES_new
cd ..
rm -rf Simulator
cp -r USE-source Simulator
echo "USE" > Simulator/simulator-name
