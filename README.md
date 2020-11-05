# RootSim Models
Here you can find all the models that we build for rootsim or USE environments.

## Setup
We provide a setup script which will download ROOT-Sim source in `ROOT-Sim-source` and install its libraries and
compiler in `ROOT-Sim-bin`.

The can take several arguments to customize the rootsim installation ([refer to the ROOT-Sim Getting Started page for details](https://hpdcs.github.io/ROOT-Sim//getting-started.html)):

- `debug`: enable debug symbols
- `debug-extra`: extra checks during debug (kills performance)
- `mpi`: enable mpi
- `no-rebinding`: disable LP rebinding
- `profile`: enable performance reports

## Usage
To use the models we provide a Makefile in each model folder, which will compile all the models using `rootsim-cc` in their
respective folder.

To start the process of generating a topology, computing the model results, running the simulation and merging the results into one pdf, you need to start the bash script, where the first parameter is not optional:

`./start.sh <number of LPs for the simulation> <wheter the results should be aggregated or not>`

Second parameter should be either empty or "aggregated"
