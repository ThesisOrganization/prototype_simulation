CC = gcc
CFLAGS = -Wall -Wextra
TARGET = analytical_model_computation
SRCS= quick_sort.c formulas.c ../utils/partop/topology.c ../utils/partop/parser.c ../utils/idmap/idmap.c ../utils/idmap/idmap_quick_sort.c
OBJS = $(SRCS:.c=.o)
DEPS= ../utils/partop/header.h quick_sort.h ../utils/idmap/idmap.h
DBG_OPTS_TO_ENABLE= -g -O0
DBG_OPTS=

.PHONY: clean copy

all: $(TARGET) copy

$(TARGET): $(OBJS)
ifdef DBG
	DBG_OPTS=$(DBG_OPTS_TO_ENABLE)
endif
	@$(CC) $(CFLAGS) $(DBG_OPTS) -o $(TARGET) $(OBJS)

copy:
ifdef LOCATION
	@cp $(TARGET) $(LOCATION)/$(TARGET)
endif


%.o : %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@$(RM) $(SRCS:.c=.o*) $(TARGET) $(LOCATION)/$(TARGET)
