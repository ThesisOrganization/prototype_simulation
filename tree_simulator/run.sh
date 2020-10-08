#!/bin/bash

if [ "$1" == "valgrind" ]; then
    valgrind --leak-check=full ./queues --sequential --lp 30 --simulation-time 10000
else
    ./queues --sequential --lp 30 --simulation-time 10000
fi
