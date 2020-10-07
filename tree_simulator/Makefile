CC = rootsim-cc
CFLAGS = -Wall -Wextra -g
TARGET = queues

SRCS = application.c simulation_functions.c ../utils/priority_queue/priority_queue.c ../utils/priority_queue/heap/heap.c ../utils/partop/parser.c ../utils/partop/topology.c ../utils/priority_scheduler/priority_scheduler.c ../utils/priority_scheduler/quick_sort.c
OBJS = $(SRCS:.c=.o)
DEPS= ../utils/priority_queue/priority_queue.h ../utils/priority_queue/heap/heap.h ../utils/priority_queue/heap/_heap.h ../utils/application_datatypes.h ../utils/priority_scheduler/priority_scheduler.h ../utils/priority_scheduler/quick_sort.h ../utils/partop/header.h

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.o.rs $(TARGET)
