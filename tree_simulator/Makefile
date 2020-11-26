CFLAGS = -Wall -Wextra
DBG_OPTS_TO_ENABLE= -g -O0
TARGET = simulation
LOCATION=../tree_simulator_bin
SRCS = simulation.c simulation_functions/finish_event/finish_functions.c simulation_functions/arrive_event/arrive_functions.c simulation_functions/init_event/init_functions.c ../utils/priority_queue/priority_queue.c ../utils/priority_queue/heap/heap.c ../utils/partop/parser.c ../utils/partop/topology.c ../utils/priority_scheduler/priority_scheduler.c ../utils/priority_scheduler/queue_quick_sort.c simulation_functions/setup_protocol/setup_protocol.c simulation_functions/setup_protocol/setup_recv_functions.c simulation_functions/setup_protocol/setup_send_functions.c ../utils/idmap/idmap.c ../utils/idmap/idmap_quick_sort.c ../utils/message/message.c simulation_functions/common_functions.c ../utils/partop/binaryParse.c

OBJS_rootsim = $(SRCS:.c=_rootsim.o)
OBJS_use = $(SRCS:.c=.o)

DEPS= ./simulation_functions/finish_event/finish_functions.h ./simulation_functions/arrive_event/arrive_functions.h ./simulation_functions/init_event/init_functions.h ../utils/priority_queue/priority_queue.h ../utils/priority_queue/heap/heap.h ../utils/priority_queue/heap/_heap.h ../utils/application_datatypes.h ../utils/priority_scheduler/priority_scheduler.h ../utils/priority_scheduler/priority_scheduler_datatypes.h ../utils/priority_scheduler/queue_quick_sort.h ../utils/partop/header.h ../utils/partop/partop_datatypes.h simulation_functions/setup_protocol/setup_datastructures.h simulation_functions/setup_protocol/setup_protocol.h simulation_functions/setup_protocol/setup_recv_functions.h simulation_functions/setup_protocol/setup_send_functions.h ../utils/idmap/idmap.h ../utils/idmap/idmap_quick_sort.h ../utils/message/message.h ./simulation.h ./simulation_functions/common_functions.h

.PHONY: clean

MAX_GIGAS=$(shell free -h | sed -E -e '1d' -e '3d' -e 's:[GMK]i::g' -e 's/Mem:[ ]+//' -e 's/[0-9\.]+[ ]+//g')
ck=10
pub=0.33333
epb=3
nbc=1
lookahead=0.0
dbg=0
sperimental=1
lin_pin=1
reversible=0
#1=stampe dettagliate
report=1
queue_len=256
max_lp=0
sim_max_duration=60

ifdef DBG
DBG_OPTS=$(DBG_OPTS_TO_ENABLE)
endif

all: rootsim use neurome

use : $(SRCS) $(DEPS)
	make PHOLD_SOURCES="$(SRCS:%=$(shell pwd)/%)" TARGET=simulation_use THR_POOL_SIZE=$(queue_len) MAX_ALLOCABLE_GIGAS=$(MAX_GIGAS) NBC=$(nbc) MAX_SKIPPED_LP=$(max_lp) REVERSIBLE=0 LOOKAHEAD=$(lookahead) PERC_USED_BUCKET=$(pub) ELEM_PER_BUCKET=$(epb) REPORT=$(report) DEBUG=$(dbg) SPERIMENTAL=$(sperimental) CHECKPOINT_PERIOD=$(ck) LINEAR_PINNING=$(lin_pin) -C ../Simulators/USE/ _phold executable
	mkdir -p $(LOCATION) ; mv ../Simulators/USE/simulation_use $(LOCATION)/simulation_use

rootsim : $(OBJS_rootsim) $(DEPS)
	../Simulators/ROOT-Sim-bin/bin/rootsim-cc $(CFLAGS) $(DBG_OPTS) -I../Simulators/ROOT-Sim-bin/include $(SRCS) -o simulation_rootsim
	mkdir -p $(LOCATION) ; mv simulation_rootsim $(LOCATION)/simulation_rootsim

%_rootsim.o : %.c
	../Simulators/ROOT-Sim-bin/bin/rootsim-cc -c $(CFLAGS) $(DBG_OPTS) -I../Simulators/ROOT-Sim-bin/include $< -o $@

neurome : $(SRCS) $(DEPS)
	../Simulators/NeuRome-bin/bin/rootsim-cc $(CFLAGS)  $(DBG_OPTS) -I../Simulators/NeuRome-bin/include $(SRCS)
	mkdir -p $(LOCATION) ; mv model_parallel $(LOCATION)/simulation_neurome_parallel
	mkdir -p $(LOCATION) ; mv model_serial $(LOCATION)/simulation_neurome_serial

clean:
	rm -rf $(OBJS_use:.o=.o*) $(OBJS_rootsim:.o=.o*) $(LOCATION)/simulation*
	make PHOLD_SOURCES="$(SRCS:%.c=$(shell pwd)/%.c)" TARGET=simulation_use -C ../Simulators/USE clean
