# RootSim Models
Here you can find all the models that we build for rootsim or USE environments.

## Setup
We provide a setup script which will download ROOT-Sim source in `ROOT-Sim-source` and install its libraries and
compiler in `ROOT-Sim-bin`.

## Usage
To use the models we provide a top-level Makefile which will compile all the models using `rootsim-cc` in their
respective folder.

To compile a model you need to issue a `make [model name]` command. If you need to use debug option you can use the
`make [model name]-debug` command which will compile the model with the following options: `-ggdb -fsanitize=address`.
