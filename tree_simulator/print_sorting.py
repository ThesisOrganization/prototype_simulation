import sys

if len(sys.argv) < 2:
    file_name = "./output.txt"
else:
    file_name = sys.argv[1]


#file_name = "./output.txt"
f = open(file_name, "r")

text = f.read()

list_devices = text.split("#################################################\n")
list_devices = list_devices[1:]

dict_devices = {}

for elem in list_devices:
    list_informations = elem.split("\n")
    device_string = list_informations[0]
    #print(device_string.split(":"))
    device_number = int(device_string.split(",")[0].split(":")[1].strip())
    #print(device_number)
    dict_devices[device_number] = elem

'''
final_list = list(dict_devices.values())

for text in final_list:
    print("############################################ NEW DEVICE ###########################################")
    print(text)
'''
list_ids = list(dict_devices.keys())
list_ids.sort()


for id_device in list_ids:
    print("############################################ NEW DEVICE ###########################################")
    print(dict_devices[id_device])


print(list_ids)
