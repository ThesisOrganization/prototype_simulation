CC = rootsim-cc
CFLAGS = -Wall -Wextra -g -ggdb -fsanitize=address
TARGET = queues

SRCS = simulation.c ./simulation_functions/finish_event/finish_functions.c ./simulation_functions/arrive_event/arrive_functions.c ./simulation_functions/init_event/init_functions.c ../utils/priority_queue/priority_queue.c ../utils/priority_queue/heap/heap.c ../utils/partop/parser.c ../utils/partop/topology.c ../utils/priority_scheduler/priority_scheduler.c ../utils/priority_scheduler/queue_quick_sort.c simulation_functions/setup_protocol/setup_protocol.c simulation_functions/setup_protocol/setup_recv_functions.c simulation_functions/setup_protocol/setup_send_functions.c

OBJS = $(SRCS:.c=.o)

DEPS= ./simulation_functions/finish_event/finish_functions.h ./simulation_functions/arrive_event/arrive_functions.h ./simulation_functions/init_event/init_functions.h ../utils/priority_queue/priority_queue.h ../utils/priority_queue/heap/heap.h ../utils/priority_queue/heap/_heap.h ../utils/application_datatypes.h ../utils/priority_scheduler/priority_scheduler.h ../utils/priority_scheduler/queue_quick_sort.h ../utils/partop/header.h simulation_functions/setup_protocol/setup_datastructures.h simulation_functions/setup_protocol/setup_protocol.h simulation_functions/setup_protocol/setup_recv_functions.h simulation_functions/setup_protocol/setup_send_functions.h

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.o.rs $(TARGET)
