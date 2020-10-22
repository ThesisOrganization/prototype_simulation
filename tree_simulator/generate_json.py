import sys
import json

dict_types_devices = {0:"node", 1:"sensor", 2:"actuator", 3:"wan", 4:"lan"}
dict_class_number = {0:"telemetry", 1:"transition",2:"command",3:"batch"}
dict_types_nodes = {0:"central", 1:"regional", 2:"local"}



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
    S = []
    R = []
    N = []
    U = []
    A = []
    X = []
    parameters = {}
    for class_string in classes_list:
        class_list = class_string.strip().split("\n")
        if int(class_list[0][-1]) < 4:
            class_number_string = dict_class_number[int(class_list[0][-1])]
            class_list = class_list[1:]
            #print(class_number_string)
            parameters[class_number_string] = {}
            #dict_class_to_add = {}
            for metric_string in class_list:
                if ": " in metric_string:
                    value_list = metric_string.strip().split(": ")
                    value = value_list[1]
                    if value == "-nan":
                        value = 0.0
                    else:
                        value = float(value)
                    #print(value_list[0])
                    if value_list[0] == "Average Service time":
                        parameters[class_number_string]["service_demand"] = value
                        S.append(value)
                    if value_list[0] == "Average Response time":
                        parameters[class_number_string]["response_time"] = value
                        R.append(value)
                    if value_list[0] == "Average number of visits":
                        N.append(value)
                    if value_list[0] == "Utilization factor":
                        parameters[class_number_string]["utilization_factor"] = value
                        U.append(value)
                    if value_list[0] == "Arrival rate":
                        parameters[class_number_string]["lambda_in"] = value
                        A.append(value)
                    if value_list[0] == "Throughput":
                        X.append(value)
    
    return return_id_string, parameters

def print_devices(list_ids, dict_devices):

    list_all_devices = []

    for id_device in list_ids:
        dict_device_to_add = {}
        dict_device_to_add["id"] = id_device
        lans_list = dict_devices[id_device].strip().split("<<<<<<<<<<<<<<<<<<<<\n")
        if len(lans_list) > 1:
            device_id_string = lans_list[0]
            if len(device_id_string.split("......................\n")) > 1:
                #disk
                
                device_id_string = lans_list[0].split("\n")[0] #manca il ritorno a capo
                lans_list.insert(0, device_id_string)

                device_id = device_id_string.strip().split(",")[0].split(": ")[1]
                device_type = int(device_id_string.strip().split(",")[1].split(": ")[1])
                dict_device_to_add["type"] = dict_types_devices[device_type]
                #print(device_id)
                node_type_value = int(device_id_string.strip().split(",")[2].split(": ")[1])
                dict_device_to_add["node_type"] = dict_types_nodes[node_type_value]

                lan_string_name, dict_device_to_add["parameters"] = print_classes(lans_list[1])


                lan_string_name, dict_device_to_add["storage"] = print_classes(lans_list[2])

            else:
                device_id = device_id_string.strip().split(",")[0].split(": ")[1]
                device_type = int(device_id_string.strip().split(",")[1].split(": ")[1])
                dict_device_to_add["type"] = dict_types_devices[device_type]

                lan_string_name, dict_device_to_add["lan_in"] = print_classes(lans_list[1])

                lan_string_name, dict_device_to_add["lan_out"] = print_classes(lans_list[2])
        else:
            device_id_string, dict_device_to_add["parameters"] = print_classes(lans_list[0])

            device_id = device_id_string.strip().split(",")[0].split(": ")[1]
            device_type = int(device_id_string.strip().split(",")[1].split(": ")[1])
            dict_device_to_add["type"] = dict_types_devices[device_type]
            
            node_type_value = int(device_id_string.strip().split(",")[2].split(": ")[1])
            if node_type_value != -1:
                dict_device_to_add["node_type"] = dict_types_nodes[node_type_value]
            
        list_all_devices.append(dict_device_to_add)
    return list_all_devices
    

if len(sys.argv) < 2:
    file_name = "./output.txt"
else:
    file_name = sys.argv[1]


f = open(file_name, "r")

text = f.read()

dict_devices, list_ids = get_dict(text)

f.close()

if len(sys.argv) < 3:
    out = "simulation_results.json"
else:
    out = sys.argv[2]

f = open(out, "w")

#f.write("prova")

#list_file_name = "../model_computation/order.txt"
#list_file = open(list_file_name, "r")
#list_string_ids = list_file.read().strip().strip(",").split(",")
#list_ids = []
#for string_id in list_string_ids:
#    list_ids.append(int(string_id))

#print(list_ids)
#list_ids = [8,18,3,10,19,4,1,12,20,5,14,21,6,2,0]

list_final = print_devices(list_ids, dict_devices)

#print(list_final)
#print(json.dumps(list_final))
json.dump(list_final, f)

f.close()


#print(list_ids)
