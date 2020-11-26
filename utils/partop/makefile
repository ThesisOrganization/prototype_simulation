CC = gcc
CFLAGS = -Wall
SRCS = parser.c topology.c ../idmap/idmap_quick_sort.c ../idmap/idmap.c binaryGenerate.c
OBJS = $(SRCS:.c=.o)
HEADERS = header.h ../idmap/idmap.h
MAIN = driverBinaries

all:    $(MAIN) driver-test

$(MAIN): $(OBJS) $(MAIN).c
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN).c $(OBJS)

%.o: %.c %.h
	gcc -c $(CFLAGS) $*.c -o $*.o

driver-test :  parser.c topology.c ../idmap/idmap_quick_sort.c ../idmap/idmap.c
	gcc -c $(CFLAGS) driver.c -o driver.o
	gcc $(CFLAGS) -o driver driver.o parser.o topology.o ../idmap/idmap_quick_sort.o ../idmap/idmap.o

clean:
	$(RM) $(SRCS:.c=.o)  $(SRCS:.c=.o.rs)  driver driverBinaries $(MAIN)
