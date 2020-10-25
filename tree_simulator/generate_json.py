import sys
import json
import os

if len(sys.argv) < 2:
    out = "simulation_results.json"
else:
    out = sys.argv[1]

fout = open(out, "w")

path = "lp_data/"

list_files_name = os.listdir(path)
list_files_name.remove(".gitignore")
final_list = []

for file_name in list_files_name:
    fin = open(path + file_name, "r")
    #print(file_name)
    final_list.extend(json.load(fin))
    fin.close()


json.dump(final_list, fout)

fout.close()


