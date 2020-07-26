CC = rootsim-cc
CFLAGS = -Wall -Wextra
TARGET = queues

SRCS = application.c ../utils/priority_queue/priority_queue.c ../utils/priority_queue/heap/heap.c
OBJS = $(SRCS:.c=.o)

.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o $(TARGET)
