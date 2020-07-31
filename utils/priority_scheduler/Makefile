CC = gcc
CFLAGS = -Wall -g
SRCS = quick_sort.c priority_scheduler.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(SRCS:.c=.h)
MAIN = driver

all:    $(MAIN)


$(MAIN): $(OBJS) $(MAIN).c
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN).c $(OBJS)

%.o: %.c %.h
	gcc -ggdb -c $(CFLAGS) $*.c -o $*.o

clean:
	$(RM) *.o $(MAIN)
