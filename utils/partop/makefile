CC = gcc
CFLAGS = -Wall -g -fsanitize=address
SRCS = parser.c topology.c ../idmap/idmap_quick_sort.c ../idmap/idmap.c
OBJS = $(SRCS:.c=.o)
HEADERS = header.h ../idmap/idmap.h
MAIN = driver

all:    $(MAIN)


$(MAIN): $(OBJS) $(MAIN).c
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN).c $(OBJS)

%.o: %.c %.h
	gcc -c $(CFLAGS) $*.c -o $*.o

clean:
	$(RM) *.o $(MAIN)
