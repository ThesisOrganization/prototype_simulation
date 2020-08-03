CC = rootsim-cc
CFLAGS = -Wall -Wextra
TARGET = queues

SRCS = application.c ../utils/priority_queue/priority_queue.c ../utils/priority_queue/heap/heap.c ../utils/partop/parser.c ../utils/partop/topology.c ../utils/priority_scheduler/priority_scheduler.c ../utils/priority_scheduler/quick_sort.c
OBJS = $(SRCS:.c=.o)

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -g -O0

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@ -g -O0

clean:
	$(RM) *.o $(TARGET)
