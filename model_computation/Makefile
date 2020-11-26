CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g
TARGET = analytical_model_computation
SRCS= quick_sort.c formulas.c ../utils/partop/topology.c ../utils/partop/parser.c ../utils/idmap/idmap.c ../utils/idmap/idmap_quick_sort.c
OBJS = $(SRCS:.c=.o)
DEPS= ../utils/partop/header.h quick_sort.h ../utils/idmap/idmap.h
.PHONY: clean copy

all: $(TARGET) copy

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

copy :
ifdef LOCATION
	cp $(TARGET) $(LOCATION)/$(TARGET)
endif

%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(SRCS:.c=.o*) $(TARGET) $(LOCATION)/$(TARGET)
