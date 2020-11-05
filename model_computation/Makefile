CC = gcc
CFLAGS = -Wall -Wextra -ggdb
TARGET = formulas
SRCS= quick_sort.c formulas.c ../utils/partop/topology.c ../utils/partop/parser.c ../utils/idmap/idmap.c ../utils/idmap/idmap_quick_sort.c
OBJS = $(SRCS:.c=.o)
DEPS= ../utils/partop/header.h quick_sort.h ../utils/idmap/idmap.h
.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -fsanitize=address

%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.o.rs $(TARGET)
