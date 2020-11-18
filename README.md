# RootSim Models
Here you can find all the models that we build for ROOT-Sim or USE environments.

## Setup

Only one simulator at a time can be configured to run simulations, since they share the same folder `Simulator` to make our code agnostic to the simulator used.

To setup a simulator use the `setup.sh` script, details on the arguments accepted are shown in the help message when the script is run.

#### ROOT-Sim
The ROOT-Sim setup can take several arguments to customize the rootsim installation ([refer to the ROOT-Sim Getting Started page for details](https://hpdcs.github.io/ROOT-Sim//getting-started.html)):

- `debug`: enable debug symbols
- `debug-extra`: extra checks during debug (kills performance)
- `mpi`: enable mpi
- `no-rebinding`: disable LP rebinding
- `profile`: enable performance reports

#### NeuRome (a.k.a. ROOT-Sim 3)
NeuRome is also semi-supported by the `setup.sh` script and the `run.sh` script, but the compiled model is still unusable.

#### USE
Compatible models will have a script (`run.sh`), that will copy USE and the model sources in a dedicated folder (`USE-model-sources`) and compile them together. The executable will be in the same place and with the same as the one created by the other simulators. The temporary folder will be removed after compilation is done. To customize compilation parameters `run.sh` can be edited in the section where the `sim_name` variable is checked to have value 'USE'.

## Usage (ROOT-Sim and NeuRome)
To use the models we provide a Makefile in each model folder, which will compile all the models using `rootsim-cc` in their respective folder.

Where present, the `run.sh` script will handle compilation and execution with the configured simulator, refer to the help message shown when the script is run.

To start the process of generating a topology, computing the model results, running the simulation and merging the results into one pdf, you need to use the `start.sh` script, which will handle everything for the tree_simulator model.
Refer to the help message shown when the script is run for details on arguments.

## Dependecies

#### Utils
- `bash`
- `python >= 3.6`
- `numpy`
- `sed`
- `make`
- `git`
- `pdflatex`
- `trap`
- `wait`
- `tail`
- `kill`

##### LATEX packages
- `booktabs`
- `float`
- `margin`

#### ROOT-Sim
- `git`
- `autotools`
- `autoconf`
- `automake`
- `libtoolize`
- `make`
- `gcc`

#### USE
- `libcap2`
- `libcap-dev`
- `g++`
- `gcc`
- `gdb`
- `make`
- `libnuma-dev`
- `git`
- `time`
- `linux-tools-*` (for your kernel version, usually identified by `uname -r`)
- `linux-cloud-tools-*` (for your kernel version, usually identified by `uname -r`)

#### NeuRome (a.k.a. ROOT-Sim 3)
- `gcc >= 8`
- `meson`
