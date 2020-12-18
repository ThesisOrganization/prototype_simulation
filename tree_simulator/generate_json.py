import argparse
import json
import os


parser=argparse.ArgumentParser(description="Merge JSONs in that represent tha output for each single LP into one single JSON file.")
parser.add_argument("data_path",help="location of the lp_data folder")
parser.add_argument("out",help="location where the aggreagted JSON file should be created")
args=parser.parse_args()
path=args.data_path+"/"
out=args.out


fout = open(out, "w")
print(path)
list_files_name = os.listdir(path)
#list_files_name.remove(".gitignore")
final_list = []

for file_name in list_files_name:
    print(file_name)
    fin = open(path+file_name, "r")
    #print(file_name)
    final_list.extend(json.load(fin))
    fin.close()


json.dump(final_list, fout)

fout.close()


