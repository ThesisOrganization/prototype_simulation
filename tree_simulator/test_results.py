import sys
import json
import os

from collections.abc import Mapping

def print_warning(a,b):
    print("WARNING: ", end="")
    print(a, end="")
    print(" =/= ", end="")
    print(b)

PERCENTAGE_CHECK = 10.0

def compare_values(a, b):

    warning_check = False

    if(isinstance(a, str) and isinstance(b, str)):
        if(a != b):
            print_warning(a, b)
            warning_check = True
        return warning_check

    if(isinstance(a, int) and isinstance(b, int)):
        if(a != b):
            print_warning(a, b)
            warning_check = True
        return warning_check

    if(isinstance(a, float) and isinstance(b, float)):
        if(a == 0.0 and b == 0.0):
            return warning_check
        percentage = (abs(a - b) / max(a, b) ) * 100.0
        if(percentage > PERCENTAGE_CHECK):
            print_warning(a, b)
            print("Percentage: ", end="")
            print(percentage)
            warning_check = True
        return warning_check

    print("WARNING: values type not found")
    return warning_check

def compare_dicts(a, b):

    warning_check_final = False

    if(not isinstance(a, Mapping) and not isinstance(b,Mapping)):
        warning_check = compare_values(a, b)
        warning_check_final = warning_check
        return warning_check_final

    if(len(a) != len(b)):
        print("ERROR: len(a) != len(b)!")
        warning_check = True
        return warning_check_final

    for k in a:
        warning_check = compare_dicts(a[k], b[k])
        if warning_check:
            print(k, end="")
            print(", ", end="")
        warning_check_final = warning_check_final or warning_check
    
    return warning_check_final


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
    warning_check = compare_dicts(simulator[i], compare[i])
    if warning_check:
        print("id element: ", end="")
        print(i)
        print()




fcompare.close()

fsimulator.close()


