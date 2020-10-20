import sys

dict_types_devices = {0:"Node", 1:"Sensor", 2:"Actuator", 3:"WAN", 4:"LAN"}


def ret_string_table_row(items):
    return_string = "$" + str(items[0]) + "$"
    for i in range(1, 4):
        return_string += " & $"
        return_string += str(items[i])
        return_string += "$"
    return_string += "\\\\"

    return return_string




def ret_string_table(items, labels, caption):
    header = "\\begin{table}[H]\n\\centering\n\\begin{tabular}{@{}cccc@{}}\n\\toprule\n"
    header += ret_string_table_row(labels)
    header += "\n\\midrule\n"
    header += ret_string_table_row(items)
    header += "\n\\bottomrule\n\\end{tabular}\n\\caption{"
    header += caption
    header += "}\n\\end{table}\n\n"
    return header


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
    for class_string in classes_list:
        class_list = class_string.strip().split("\n")
        class_number_string = class_list[0]
        class_list = class_list[1:]
        #print(class_number_string)
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
                    S.append(value)
                if value_list[0] == "Average Response time":
                    R.append(value)
                if value_list[0] == "Average number of visits":
                    N.append(value)
                if value_list[0] == "Utilization factor":
                    U.append(value)
                if value_list[0] == "Arrival rate":
                    A.append(value)
                if value_list[0] == "Throughput":
                    X.append(value)
                #print(value)
    #ret_string_table(N, ["N_t", "N_e", "N_c", "N_b"])
    #all_data_string = ret_string_table(S, ["S_t", "S_e", "S_c", "S_b"]) + ret_string_table(R, ["R_t", "R_e", "R_c", "R_b"]) + ret_string_table(U, ["U_t", "U_e", "U_c", "U_b"]) + ret_string_table(A, ["S_t", "S_e", "S_c", "S_b"]) + ret_string_table(X, ["S_t", "S_e", "S_c", "S_b"])
    
    all_data_string = ret_string_table(A, ["\\lambda_t", "\\lambda_e", "\\lambda_c", "\\lambda_b"], "Arrival rates") + ret_string_table(S, ["D_t", "D_e", "D_c", "D_b"], "Service demands") + ret_string_table(U, ["U_t", "U_e", "U_c", "U_b"], "Utilization factors") + ret_string_table(R, ["R_t", "R_e", "R_c", "R_b"], "Response times")


    return return_id_string, all_data_string

def print_devices(list_ids, dict_devices, file_h):

    initial_header = "\\documentclass{article}\n\\usepackage{booktabs}\n\\usepackage{float}\n\\begin{document}\n\\section{Computed Formulas Example}\n"

    file_h.write(initial_header)

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
            if len(device_id_string.split("......................\n")) > 1:
                #disk
                
                device_id_string = lans_list[0].split("\n")[0] #manca il ritorno a capo
                lans_list.insert(0, device_id_string)

                device_id = device_id_string.strip().split(",")[0].split(": ")[1]
                device_type = int(device_id_string.strip().split(",")[1].split(": ")[1])
                #print(device_id)
                subsection_string = "\\subsection{" + dict_types_devices[device_type] + " number: " + device_id + "}\n"

                lan_string_name, all_data_string = print_classes(lans_list[1])
                lan_string_name = lan_string_name.strip().strip(":")
                lan_string_name = "Server"
                #print(lan_string_name)
                subsubsection_string = "\subsubsection{" + lan_string_name + "}"

                file_h.write(subsection_string)
                file_h.write(subsubsection_string)
                file_h.write(all_data_string)

                lan_string_name, all_data_string = print_classes(lans_list[2])
                lan_string_name = lan_string_name.strip().strip(":")
                lan_string_name = "Disk"
                #print(lan_string_name)
                subsubsection_string = "\subsubsection{" + lan_string_name + "}"

                file_h.write(subsubsection_string)
                file_h.write(all_data_string)
            else:
                device_id = device_id_string.strip().split(",")[0].split(": ")[1]
                device_type = int(device_id_string.strip().split(",")[1].split(": ")[1])
                #print(device_id)
                subsection_string = "\\subsection{" + dict_types_devices[device_type] + " number: " + device_id + "}\n"

                lan_string_name, all_data_string = print_classes(lans_list[1])
                lan_string_name = lan_string_name.strip().strip(":")
                #print(lan_string_name)
                subsubsection_string = "\subsubsection{" + lan_string_name + "}"

                file_h.write(subsection_string)
                file_h.write(subsubsection_string)
                file_h.write(all_data_string)

                lan_string_name, all_data_string = print_classes(lans_list[2])
                lan_string_name = lan_string_name.strip().strip(":")
                #print(lan_string_name)
                subsubsection_string = "\subsubsection{" + lan_string_name + "}"

                file_h.write(subsubsection_string)
                file_h.write(all_data_string)
        else:
            device_id_string, all_data_string = print_classes(lans_list[0])
            device_id = device_id_string.strip().split(",")[0].split(": ")[1]
            device_type = int(device_id_string.strip().split(",")[1].split(": ")[1])
            subsection_string = "\\subsection{" + dict_types_devices[device_type] + " number: " + device_id + "}\n"
            #print(device_id)
            file_h.write(subsection_string)
            file_h.write(all_data_string)

    file_h.write("\\end{document}")
    

if len(sys.argv) < 2:
    file_name = "./output.txt"
else:
    file_name = sys.argv[1]


f = open(file_name, "r")

text = f.read()

dict_devices, list_ids = get_dict(text)

f.close()

if len(sys.argv) < 3:
    out = "simulation_results.tex"
else:
    out = sys.argv[2]

f = open(out, "w")

#f.write("prova")

list_file_name = "../model_computation/order.txt"
list_file = open(list_file_name, "r")
list_string_ids = list_file.read().strip().strip(",").split(",")
list_ids = []
for string_id in list_string_ids:
    list_ids.append(int(string_id))

#print(list_ids)
#list_ids = [8,18,3,10,19,4,1,12,20,5,14,21,6,2,0]

print_devices(list_ids, dict_devices, f)

f.close()


#print(list_ids)
