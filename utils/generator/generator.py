# coding=utf-8
import json
import numpy as np
import sys
f_out = open("../../tree_simulator/topology.txt", "w")
f_out_LP = open("../../tree_simulator/LP.txt", "w")

f_out_txt = open("../../jsonMerger/jsonAdditionalInfo.txt","w")
numbersString = ""
txt_path = sys.argv[1]
with open("../../"+txt_path) as f:
    lines = f.readlines()
    regionals_list_for_jsonMerge = lines[3][36:-1]
    regionals_list = lines[3][36:-1].split(";")
    number_of_regionals = 0
    number_of_locals = 0
    for element in regionals_list:
        el_list = element.split(",")
        number_of_regionals+=int(el_list[0])
        number_of_locals+=int(el_list[0]) * int(el_list[1])

    cci = lines[5][36:-1].split(",")
    central_type = cci[0]
    delay_lower_router_central = cci[1]
    disk_type = cci[2]

    rci = lines[6][36:-1].split(",")
    regional_type = rci[0]
    delay_lower_router_regional = rci[1]
    delay_upper_router_regional = rci[2]

    lci = lines[7][36:-1].split(",")
    local_type = lci[0]
    delay_lower_router_local = lci[1]
    delay_upper_router_local = lci[2]

    delay_wan = lines[9][36:-1]+"\n"
    act_type = lines[10][36:-1]
    number_of_sensor_type = lines[11][36:-1]
    number_of_lan_types = lines[12][36:-1]
    weight = lines[13][36:-1]

types_of_lans = number_of_lan_types
types_of_sensors = number_of_sensor_type
types_of_actuators = 1

f_out_txt.write(str(number_of_regionals)+"\n")
f_out_txt.write(str(int(number_of_locals))+"\n")
f_out_txt.write(regionals_list_for_jsonMerge+"\n")
sensors_start =1+number_of_regionals+number_of_locals
number_of_lans = 1 #to change, future work
f_out_txt.write(str(number_of_lans)+"\n")
number_of_WANS = 1 + number_of_regionals
f_out_txt.write(str(number_of_WANS)+"\n")

####################################


#USER SHOULD CHANGE THESE
####################################
different_amounts_sensors_type1 = 6 #0-5
different_amounts_sensors_type2 = 2 #transition sensors
different_amounts_actuators = 2 #0-1
####################################

number_of_locals_with_x_sensors_y_actuators_per_regional = np.zeros((number_of_regionals,different_amounts_sensors_type1,different_amounts_sensors_type2,different_amounts_actuators)).astype(int);

#USER SHOULD INPUT THESE, add new ones if necessary
####################################
num_act = 1
num_sens_tel = 5
num_sens_trans = 1

reg_count = 0
for element in regionals_list:
    el_list = element.split(",")
    regionals_temp = int(el_list[0])
    locals_temp = int(el_list[1])
    for i in range(regionals_temp):
        number_of_locals_with_x_sensors_y_actuators_per_regional[reg_count][num_sens_tel][num_sens_trans][num_act] = locals_temp #[id_regionale][#sensori tipo 1][#sensori_tipo2][#attuatori]
        reg_count+=1

####################################
total_sensors = 0
total_actuators = 0
for i in range(number_of_regionals):
    for j in range(different_amounts_sensors_type1) :
        for l in range(different_amounts_sensors_type2):
            for k in range(different_amounts_actuators):
                total_sensors+=j*number_of_locals_with_x_sensors_y_actuators_per_regional[i][j][l][k]
                total_sensors+=l*number_of_locals_with_x_sensors_y_actuators_per_regional[i][j][l][k]
                total_actuators+=k*number_of_locals_with_x_sensors_y_actuators_per_regional[i][j][l][k]

f_out_txt.write(str(total_sensors)+"\n")
f_out_txt.write(str(total_actuators)+"\n")

f_out_txt.write(str(num_sens_tel)+"\n")
f_out_txt.write(str(num_sens_trans)+"\n")
f_out_txt.write(str(num_act)+"\n")

number_of_elements = 1 + number_of_regionals + number_of_locals+total_sensors+total_actuators
number_of_elements_to_write = 1 + number_of_regionals + number_of_locals * 2+total_sensors+total_actuators+number_of_WANS #change x2 because LAN
numbersString = str(number_of_elements_to_write)+"\n"
numbersString+=str(types_of_actuators)+"\n"
numbersString+=str(types_of_sensors)+"\n"
numbersString+=str(types_of_lans)+"\n"
f_out.write(numbersString);
f_out_LP.write(str(number_of_elements_to_write)+"\n")
f_out_LP.write(str(number_of_regionals)+"\n");
#USER SHOULD CHANGE THESE
####################################
#rate_trans_act = "0.0005,"
list_sensors = []
for i in range(int(number_of_sensor_type)):
    with open("../../tests_topology/catalog/sensor/Type"+str(i)+".json") as sensor_infos:
        sensor_inf = json.load(sensor_infos)
        dict_sensors = {}
        for element in sensor_inf:
            dict_sensors[element] = sensor_inf[element]
        list_sensors.append(dict_sensors)


with open("../../tests_topology/catalog/actuator/"+act_type+".json") as actuator_infos:
  actuator_inf = json.load(actuator_infos)
for element in actuator_inf:
    if element == "service_time_commands_act":
        service_time_commands_act = actuator_inf[element]
    elif element == "rate_trans_act":
        rate_trans_act = actuator_inf[element]
    elif element == "measure_type":
        measure_type = actuator_inf[element]
    elif element == "job_type":
        job_type = actuator_inf[element]

f_out_txt.write(str(rate_trans_act)+"\n")
#sens_tele_type1 = "0.0002"
sensor_rates_string = ""
for i in range(int(number_of_sensor_type)):
    if list_sensors[i]['rate_sensors_telemetry'] > 0:
        f_out_txt.write(str(list_sensors[i]['rate_sensors_telemetry'])+"\n")

    else:
        if list_sensors[i]['rate_sensors_transition'] > 0:
            f_out_txt.write(str(list_sensors[i]['rate_sensors_transition'])+"\n")

    if sensor_rates_string == "" :
        sensor_rates_string+=str(list_sensors[i]["rate_sensors_telemetry"])+","+str(list_sensors[i]["rate_sensors_transition"])
    else:
        sensor_rates_string+=";"+str(list_sensors[i]['rate_sensors_telemetry'])+","+str(list_sensors[i]['rate_sensors_transition'])

f_out.write(sensor_rates_string+"\n")

associated_wan_up = ""

service_batch = "0.0"
service_reply = "0.0"
####################################
LAN_IN_services = ""

LAN_OUT_services = ""
delay_lan_list = []

for i in range(int(number_of_lan_types)):
    with open("../../tests_topology/catalog/lan/Type"+str(i)+".json") as lan_infos:
      lan_inf = json.load(lan_infos)
    aggregation_rates_lan = ""
    service_time_lan = ""

    for element in lan_inf:
        if element == "delay":
            delay_lan_list.append(lan_inf[element])
        elif element == "service_times":
            inner_count = 0
            service_time_lan = ""
            for element2 in lan_inf[element]:
                if inner_count == 0:
                    service_time_lan+=str(lan_inf[element][element2])
                    inner_count+=1
                else:
                    service_time_lan+=","+str(lan_inf[element][element2])
            if LAN_IN_services == "":
                LAN_IN_services += service_time_lan+","+service_batch+","+service_reply
            else:
                LAN_IN_services +=";"+service_time_lan+","+service_batch+","+service_reply

LAN_OUT_services = LAN_IN_services
f_out.write(LAN_IN_services+"\n")
f_out.write(LAN_OUT_services+"\n")
#weight = "1"
f_out.write(weight+"\n")
#NODES
#CENTRAL
wan_id = number_of_elements
id_wan_central = wan_id
#Let's get the info from the catalog
with open("../../tests_topology/catalog/central/"+central_type+".json") as central_infos:
  central_inf = json.load(central_infos)
aggregation_rates_central = ""
service_time_central = ""
for element in central_inf:
    if element == 'prob_command_generated':
        prob_command_generated_central = str(central_inf[element])
    elif element == "scheduler_type":
        central_scheduler = central_inf[element]
    elif element == "service_times":
        inner_count = 0
        for element2 in central_inf[element]:
            if inner_count == 0:
                service_time_central+=str(central_inf[element][element2])
                inner_count+=1
            else:
                service_time_central+="/"+str(central_inf[element][element2])
    else:
        inner_count = 0
        for element2 in central_inf[element]:
            if inner_count == 0:
                aggregation_rates_central+=str(central_inf[element][element2])
                inner_count+=1
            else:
                aggregation_rates_central+="/"+str(central_inf[element][element2])

with open("../../tests_topology/catalog/disk/"+disk_type+".json") as disk_infos:
  disk_inf = json.load(disk_infos)
aggregation_rates_disk = ""
service_time_disk = ""

for element in disk_inf:
    if element == "disk_type":
        disk_type_string = disk_inf[element]
    elif element == "service_times":
        inner_count = 0
        service_time_disk = ""
        for element2 in disk_inf[element]:
            if inner_count == 0:
                service_time_disk+=str(disk_inf[element][element2])
                inner_count+=1
            else:
                service_time_disk+="/"+str(disk_inf[element][element2])

to_write ="0;-1;10;NODE,"+central_scheduler+",CENTRAL,"+aggregation_rates_central+",-1,"+delay_lower_router_central+","
to_write+=service_time_central+","+prob_command_generated_central+","+disk_type_string+","+service_time_disk+"\n"
associated_wan_down =str(id_wan_central)+";0;3;WAN,WAN_TYPE0,"+delay_wan
f_out.write(to_write)
LP_index = 0
LP_start_list="0"+","+str(id_wan_central)+","
LP_num_elements = 2
wan_id+=1
#REGIONAL+LOCAL
local_per_regional = number_of_locals/number_of_regionals
associated_lan_down = ""
lan_id = number_of_elements+number_of_WANS
count = number_of_elements+1 #the wans!
sensor_actuator_string = ""
flagCentral = True
index = 1
indexLocal = number_of_regionals+1

with open("../../tests_topology/catalog/regional/"+regional_type+".json") as regional_infos:
  regional_inf = json.load(regional_infos)
aggregation_rates_regional = ""
service_time_regional = ""

for element in regional_inf:
    if element == 'prob_command_generated':
        prob_command_generated_regional = str(regional_inf[element])
    elif element == "scheduler_type":
        regional_scheduler = regional_inf[element]
    elif element == "service_times":
        inner_count = 0
        for element2 in regional_inf[element]:
            if inner_count == 0:
                service_time_regional+=str(regional_inf[element][element2])
                inner_count+=1
            else:
                service_time_regional+="/"+str(regional_inf[element][element2])
    else:
        inner_count = 0
        for element2 in regional_inf[element]:
            if inner_count == 0:
                aggregation_rates_regional+=str(regional_inf[element][element2])
                inner_count+=1
            else:
                aggregation_rates_regional+="/"+str(regional_inf[element][element2])

with open("../../tests_topology/catalog/local/"+local_type+".json") as local_infos:
  local_inf = json.load(local_infos)
aggregation_rates_local = ""
service_time_local = ""

for element in local_inf:
    if element == 'prob_command_generated':
        prob_command_generated_local = str(local_inf[element])
    elif element == "scheduler_type":
        local_scheduler = local_inf[element]
    elif element == "service_times":
        inner_count = 0
        for element2 in local_inf[element]:
            if inner_count == 0:
                service_time_local+=str(local_inf[element][element2])
                inner_count+=1
            else:
                service_time_local+="/"+str(local_inf[element][element2])
    else:
        inner_count = 0
        for element2 in local_inf[element]:
            if inner_count == 0:
                aggregation_rates_local+=str(local_inf[element][element2])
                inner_count+=1
            else:
                aggregation_rates_local+="/"+str(local_inf[element][element2])




while index < (1+number_of_regionals):
    upper_node = wan_id
    to_write = str(index)+";"+str(id_wan_central)+";8;NODE,"+regional_scheduler+",REGIONAL,"+aggregation_rates_regional+","+delay_lower_router_regional+","+delay_upper_router_regional+","
    to_write+=service_time_regional+","+prob_command_generated_regional+"\n"
    f_out.write(to_write)

    associated_wan_down +=str(wan_id)+";"+str(index)+";3;WAN,WAN_TYPE0,"+delay_wan
    #to_write = str(LP_index)+";"+str(2)+";"+str(index)+","+str(wan_id)+"\n"counter_elements
    #f_out_LP.write(to_write)

    if flagCentral:
        LP_start_list+=str(index)+","+str(wan_id)
        LP_num_elements+=2
        flagCentral = False
    else:
        LP_start_list = str(index)+","+str(wan_id)
        LP_num_elements = 2
    tot = 0

    for j in range(different_amounts_sensors_type1) :
        for l in range(different_amounts_sensors_type2):
            for k in range(different_amounts_actuators):
                if(number_of_locals_with_x_sensors_y_actuators_per_regional[index-1][j][l][k] != 0):
                    for last in range(number_of_locals_with_x_sensors_y_actuators_per_regional[index-1][j][l][k]):
                        counter_elements = 2

                        to_write = str(indexLocal)+";"+str(count)+";8;NODE,"+local_scheduler+",LOCAL,"+aggregation_rates_local+","+delay_lower_router_local+","+delay_upper_router_local+","
                        to_write+=service_time_local+","+prob_command_generated_local+"\n"
                        f_out.write(to_write)

                        LP_start_list+=","+str(indexLocal)+","+str(lan_id)

                        associated_lan_down+=str(lan_id)
                        upper_lan_node = indexLocal
                        associated_lan_down+=";"+str(indexLocal)+";3;LAN,LAN_TYPE0,"+str(delay_lan_list[0])+"\n" #here need type, delay_lan_list[type]
                        for sens in range(j):
                            sensor_actuator_string+= str(sensors_start)+";"+str(lan_id)+";4;SENSOR,"+list_sensors[0]['job_type']+",SENSOR_TYPE"+str(0)+","+list_sensors[0]['measure_type']+"\n"
                            LP_start_list+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        for sens2 in range(l):
                            sensor_actuator_string+= str(sensors_start)+";"+str(lan_id)+";4;SENSOR,"+list_sensors[1]['job_type']+",SENSOR_TYPE"+str(1)+","+list_sensors[1]['measure_type']+"\n"
                            LP_start_list+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        for act in range(k):
                            sensor_actuator_string += str(sensors_start)+";"+str(lan_id)+";6;ACTUATOR,"+job_type+",ACTUATOR_TYPE"+str(0)+","+measure_type+","+str(rate_trans_act)+","+str(service_time_commands_act)+"\n"
                            LP_start_list+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        lan_id+=1
                        indexLocal+=1
                        #total_write = LP_start+str(counter_elements)+LP_to_write+"\n"
                        tot +=counter_elements
                    count+=1
    tot = LP_num_elements+tot
    total_write = str(LP_index)+";"+str(tot)+";"+LP_start_list+"\n"
    f_out_LP.write(total_write)
    LP_index+=1
    wan_id+=1
    index+=1





f_out.write(sensor_actuator_string)
f_out.write(associated_wan_down)
f_out.write(associated_lan_down)
f_out.close()
