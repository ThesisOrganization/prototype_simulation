import sys


def get_dict(text):
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
    
    list_ids = list(dict_devices.keys())
    list_ids.sort()
    return dict_devices, list_ids

def print_classes(all_classes):
    classes_list = all_classes.strip().split("......................\n")
    return_id_string = classes_list[0]
    classes_list = classes_list[1:]
    for class_string in classes_list:
        class_list = class_string.strip().split("\n")
        class_number_string = class_list[0]
        class_list = class_list[1:]
        print(class_number_string)
        for metric_string in class_list:
            if ": " in metric_string:
                value_list = metric_string.strip().split(": ")
                value = value_list[1]
                if value == "-nan":
                    value = 0.0
                else:
                    value = float(value)
                print(value_list[0])
                print(value)

    return return_id_string

def print_devices(list_ids, dict_devices):

    for id_device in list_ids:
        #print("############################################ NEW DEVICE ###########################################à")
        #print(dict_devices[id_device])
        lans_list = dict_devices[id_device].strip().split("<<<<<<<<<<<<<<<<<<<<\n")
        #print("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO")
        #print(dict_devices[id_device])
        #print("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO")
        #print(dict_devices[id_device])
        if len(lans_list) > 1:
            device_id_string = lans_list[0]
            device_id = device_id_string.strip().split(",")[0].split(": ")[1]
            print(device_id)
            lan_string_name = print_classes(lans_list[1]).strip().strip(":")
            print(lan_string_name)
            lan_string_name = print_classes(lans_list[2]).strip().strip(":")
            print(lan_string_name)
        else:
            device_id_string = print_classes(lans_list[0])
            device_id = device_id_string.strip().split(",")[0].split(": ")[1]
            print(device_id)
    

if len(sys.argv) < 2:
    file_name = "./output.txt"
else:
    file_name = sys.argv[1]


f = open(file_name, "r")

text = f.read()

dict_devices, list_ids = get_dict(text)

f.close()

out = "report.tex"
f = open(out, "w")

f.write("prova")

print_devices(list_ids, dict_devices)

f.close()


print(list_ids)
