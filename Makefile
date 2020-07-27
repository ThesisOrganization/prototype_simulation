#enables warnings
CCOPTS= -Wall -Wstrict-prototypes -Wextra
#enables gdb debug options
CCOPTS-DBG= -ggdb
#shared library options to link it
LIB_PATH= -L$(shell pwd)/ROOT-Sim-bin/lib
#enables ASAN
TEST-OPT= -fsanitize=address
# uses rootsim-cc as compiler
CC= ROOT-Sim-bin/bin/rootsim-cc

.phony: all three-queues three-queues-debug

all: docs three-queues


docs: FORCE
		doxygen docs/doxygen/Doxyfile
		$(MAKE) -C docs/latex/


three-queues:
		$(CC) $(LIB_PATH) $(CCOPTS) -o three_queues/three_queues_model three_queues/application.c

three-queues-debug:
		$(CC) $(LIB_PATH) $(CCOPTS) $(CCOPTS-DBG) -o three_queues/three_queues_model three_queues/application.c $(TEST-OPT)

# this target is used to alwys trigger the docs target,
FORCE:
