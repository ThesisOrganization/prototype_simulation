CC = gcc
CFLAGS = -Wall -Wextra -ggdb
TARGET = formulas
SRCS= quick_sort.c formulas.c ../utils/partop/topology.c ../utils/partop/parser.c
OBJS = $(SRCS:.c=.o)
DEPS= ../utils/partop/header.h quick_sort.h
.PHONY: clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -fsanitize=address

%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o *.o.rs $(TARGET)
