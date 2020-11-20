import sys
import json
import os

from collections.abc import Mapping

def print_warning(a,b):
    print("WARNING: ", end="")
    print(a, end="")
    print(" =/= ", end="")
    print(b)

def compare_values(a, b):
    if(isinstance(a, str) and isinstance(b, str)):
        if(a != b):
            print_warning(a,b)
        return
    if(a != b):
        print_warning(a, b)

def compare_dicts(a, b):
    if(not isinstance(a, Mapping) and not isinstance(b,Mapping)):
        compare_values(a, b)
        return
    if(len(a) != len(b)):
        print("ERROR: len(a) != len(b)!")
        return
    for k in a:
        compare_dicts(a[k], b[k])


simulator_file = "simulation_results.json"
fsimulator = open(simulator_file, "r")

compare_file = "./compare_json/simulation_results.json"
fcompare = open(compare_file, "r")

simulator = json.load(fsimulator)
compare = json.load(fcompare)

#compare if the two list match exactly
print(simulator == compare)

#check len of the two list
number_elements_simulator = len(simulator)
number_elements_compare = len(compare)

if(number_elements_simulator != number_elements_compare):
    print("ERROR: number_elements_simulator != number_elements_compare => true!")

#sorting
simulator = sorted(simulator, key=lambda device: device["id"])
compare = sorted(compare, key=lambda device: device["id"])

#comparing each device in the list
for i in range(0, number_elements_simulator):
    compare_dicts(simulator[i], compare[i])




fcompare.close()

fsimulator.close()


