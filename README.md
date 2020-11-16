# RootSim Models
Here you can find all the models that we build for rootsim or USE environments.

## Setup

Only one simulator at a time can be configured to run simulations, since they share the same folder `Simulator` to make our code agnostic to the simulator used.

#### ROOT-Sim
We provide a setup script (`rootsim-setup`) which will download ROOT-Sim source in `ROOT-Sim-source` and install its libraries and compiler in `Simulator`.

The can take several arguments to customize the rootsim installation ([refer to the ROOT-Sim Getting Started page for details](https://hpdcs.github.io/ROOT-Sim//getting-started.html)):

- `debug`: enable debug symbols
- `debug-extra`: extra checks during debug (kills performance)
- `mpi`: enable mpi
- `no-rebinding`: disable LP rebinding
- `profile`: enable performance reports

#### NeuRome (a.k.a. ROOT-Sim 3)
We provide a setup script (`neurome-setup`) which will download NeuRome source in `NeuRome-source` and install its libraries and compiler in `Simulator`.

#### USE
Compatible models will have a script (`run-use.sh`) and another makefile (`Makefile-USE-only`), the script will copy USE and the model sources in a dedicated folder (`simulator-sources`) and compile them togheter. The executable will be in the same place and with the same as the one created by the other simulators. The temporary folder will be removed after compilation is done.
__TODO__: explain compilation parameters

## Usage
To use the models we provide a Makefile in each model folder, which will compile all the models using `rootsim-cc` in their respective folder.

To start the process of generating a topology, computing the model results, running the simulation and merging the results into one pdf, you need to start the bash script, where the first parameter is not optional:

`./start.sh <number of LPs for the simulation> <wheter the results should be aggregated or not>`

Second parameter should be either empty or "aggregated"

### Dependecies

#### Utils
- `python >= 3.6`
- `numpy`
- `sed`
- `make`
- `git`
- `pdflatex`
- `script`

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
