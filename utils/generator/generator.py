# coding=utf-8
import argparse
import json
import numpy as np
import sys

parser=argparse.ArgumentParser(description="Generates topology.txt and LP.txt from the given topology configuration.")
parser.add_argument("catalog_path",help="location of the catalog folder containing types referenced in the config")
parser.add_argument("config_path",help="location of the topology configuration")
parser.add_argument("out",help="location where topology.txt and LP.txt should be created")
args=parser.parse_args()

f_out = open(args.out+"/topology.txt", "w")

f_out_txt = open(args.out+"/jsonAdditionalInfo.txt","w")

numbersString = ""
txt_path =args.config_path
with open(txt_path) as f:
    lines = f.readlines()
    type_dict = {}
    dict_types_association = {}
    temp_list = ['regional','local','actuator','sensor','lan']
    for element in temp_list:
        type_dict[element] = []
        dict_types_association[element] = {}

    cci = lines[0][36:-1].split(",")
    central_type = cci[0]
    delay_lower_router_central = cci[1]
    disk_type = cci[2]

    dict_amounts_regionals_one_type = {}
    number_of_regionals_amount = lines[1][36:-1]
    number_of_regionals = 0
    number_of_locals = 0
    countReg=0
    dict_total = {}

    for i in range(len(lines)-1):
        if len(lines[i+1]) <= 1:
            break
        dict_regional_local_amount = {}
        temp = lines[i+1]
        #print(temp)
        reg = temp.split(":")[0].strip()
        loc = temp.split(":")[1].strip()
        reg_type = reg.split(" ")[1].strip() #TypeN
        if reg_type not in type_dict['regional']:
            dict_types_association['regional'][reg_type] = len(type_dict['regional'])
            type_dict['regional'].append(reg_type)

        reg_amount = reg.split(" ")[0].strip() #how many
        number_of_regionals+=int(reg_amount)

        ##ADD HOW MANY REGIONALS, WHAT THE HELL MARCO
        for element in loc.split(","):
            splitting = element.strip().split(" ")
            if splitting[1] not in type_dict['local']:
                dict_types_association['local'][reg_type] = len(type_dict['local'])
                type_dict['local'].append(splitting[1])
            number_of_locals+=int(splitting[0])*int(reg_amount)
            dict_regional_local_amount[splitting[1]] = splitting[0]
        dict_total[countReg] = [reg_amount,reg_type,dict_regional_local_amount]
        countReg+=1


f_out_txt.write(str(number_of_regionals)+"\n")
f_out_txt.write(str(int(number_of_locals))+"\n")

#f_out_txt.write(regionals_list_for_jsonMerge+"\n")
sensors_start =1+number_of_regionals+number_of_locals
#number_of_lans = 1 #to change, future work
#f_out_txt.write(str(number_of_lans)+"\n")
number_of_WANS = 1 + number_of_regionals


number_of_lans = 0
####################################
local_infos_dict = {}
for i in type_dict['local']:
    with open(args.catalog_path+"/local/"+i.capitalize()+".json") as local_infos:
        local_inf = json.load(local_infos)
        local_infos_dict[i] = {}
        aggregation_rates_local = ""
        service_time_local = ""
        lan_count = 0
        local_infos_dict[i]['lan'] = {}
        for element in local_inf:
            if element == 'prob_command_generated':
                local_infos_dict[i][element] = str(local_inf[element])
            elif element == "scheduler_type":
                local_infos_dict[i][element] = local_inf[element]
            elif element == "service_times":
                inner_count = 0
                for element2 in local_inf[element]:
                    if inner_count == 0:
                        service_time_local+=str(local_inf[element][element2])
                        inner_count+=1
                    else:
                        service_time_local+="/"+str(local_inf[element][element2])
            elif element == "aggregation_rates":
                inner_count = 0
                for element2 in local_inf[element]:
                    if inner_count == 0:
                        aggregation_rates_local+=str(local_inf[element][element2])
                        inner_count+=1
                    else:
                        aggregation_rates_local+="/"+str(local_inf[element][element2])
            elif element == "delay_upper_router":
                local_infos_dict[i][element] = str(local_inf[element])
            elif element == "delay_lower_router":
                local_infos_dict[i][element] = str(local_inf[element])
            elif element == "cores":
                local_infos_dict[i][element] = str(local_inf[element])
            elif element == "cost":
                local_infos_dict[i][element] = str(local_inf[element])
            else:
                #lan case
                #element == lan0

                local_infos_dict[i]['lan'][element] = {}
                for lan_element in local_inf[element]:
                    if lan_element == 'type_lan':
                        if local_inf[element][lan_element] not in type_dict['lan']:
                            dict_types_association['lan'][local_inf[element][lan_element]] = len(type_dict['lan'])
                            type_dict['lan'].append(local_inf[element][lan_element])

                        local_infos_dict[i]['lan'][element][lan_element] = local_inf[element][lan_element]
                    elif lan_element == "amount":
                        local_infos_dict[i]['lan'][element][lan_element] = int(local_inf[element][lan_element])

                    else:
                        #lan_element == "sensor_type0"
                        if lan_element.split("_")[0] == "sensor":
                            type = lan_element.split("_")[1] #type
                            if type not in type_dict['sensor']:
                                dict_types_association['sensor'][type] = len(type_dict['sensor'])
                                type_dict['sensor'].append(type)

                            if 'sensor' not in local_infos_dict[i]['lan'][element]:
                                local_infos_dict[i]['lan'][element]['sensor'] = {}
                            local_infos_dict[i]['lan'][element]['sensor'][type] = local_inf[element][lan_element]
                        else:
                            if 'actuator' not in local_infos_dict[i]['lan'][element]:
                                local_infos_dict[i]['lan'][element]['actuator'] = {}

                            type = lan_element.split("_")[1] #type
                            if type not in type_dict['actuator']:
                                dict_types_association['actuator'][type] = len(type_dict['actuator'])
                                type_dict['actuator'].append(type)

                            local_infos_dict[i]['lan'][element]['actuator'][type] = local_inf[element][lan_element]

    local_infos_dict[i]['service_times'] = service_time_local
    local_infos_dict[i]['aggregation_rates'] = aggregation_rates_local

weight = ""
for type in type_dict['actuator']:
    with open(args.catalog_path+"/actuator/"+type.capitalize()+".json") as act_infos:
        act_inf = json.load(act_infos)
        if weight == "":
            weight+=str(act_inf["weight"])
        else:
            weight+=";"+str(act_inf["weight"])

total_sensors = 0
total_actuators = 0


for i in range(countReg):
    amount_same_regionals = dict_total[i][0]
    for local_types in dict_total[i][2]:
        local_amounts_this_iteration = dict_total[i][2][local_types] #locali tipo local_types

        for lans in local_infos_dict[local_types]['lan']:
            number_of_lans+=local_infos_dict[local_types]['lan'][lans]['amount'] * int(local_amounts_this_iteration) * int(amount_same_regionals)

            if 'sensor' in local_infos_dict[local_types]['lan'][lans]:
                for sens_type in local_infos_dict[local_types]['lan'][lans]['sensor']:
                    total_sensors+=local_infos_dict[local_types]['lan'][lans]['sensor'][sens_type] * local_infos_dict[local_types]['lan'][lans]['amount'] * int(local_amounts_this_iteration) * int(amount_same_regionals)
            if 'actuator' in local_infos_dict[local_types]['lan'][lans]:
                for act_type in local_infos_dict[local_types]['lan'][lans]['actuator']:
                    total_actuators+=local_infos_dict[local_types]['lan'][lans]['actuator'][act_type] * local_infos_dict[local_types]['lan'][lans]['amount'] * int(local_amounts_this_iteration) * int(amount_same_regionals)


#USER SHOULD INPUT THESE, add new ones if necessary
####################################

f_out_txt.write(str(total_sensors)+"\n")
f_out_txt.write(str(total_actuators)+"\n")

number_of_elements = 1 + number_of_regionals + number_of_locals+total_sensors+total_actuators

number_of_elements_to_write = 1 + number_of_regionals+number_of_locals+number_of_lans+total_sensors+total_actuators+number_of_WANS #change x2 because LAN
numbersString = str(number_of_elements_to_write)+"\n"
numbersString+=str(len(type_dict['actuator']))+"\n"
numbersString+=str(len(type_dict['sensor']))+"\n"
numbersString+=str(len(type_dict['lan']))+"\n"
f_out.write(numbersString);

#USER SHOULD CHANGE THESE
####################################
#rate_trans_act = "0.0005,"
dict_sensors = {}

for i in type_dict['sensor']:
    with open(args.catalog_path+"/sensor/"+i+".json") as sensor_infos:
        dict_sensors[i] = {}
        sensor_inf = json.load(sensor_infos)
        for element in sensor_inf:
            dict_sensors[i][element] = sensor_inf[element]

dict_actuators = {}
for i in type_dict['actuator']:
    with open(args.catalog_path+"/actuator/"+i+".json") as actuator_infos:
      actuator_inf = json.load(actuator_infos)
    dict_actuators[i] = {}

    for element in actuator_inf:
        dict_actuators[i][element] = actuator_inf[element]

sensor_rates_string = ""

for i in type_dict['sensor']:
    if sensor_rates_string == "" :
        sensor_rates_string+=str(dict_sensors[i]["rate_sensors_telemetry"])+","+str(dict_sensors[i]["rate_sensors_transition"])
    else:
        sensor_rates_string+=";"+str(dict_sensors[i]['rate_sensors_telemetry'])+","+str(dict_sensors[i]['rate_sensors_transition'])

f_out.write(sensor_rates_string+"\n")

associated_wan_up = ""

service_batch = "0.0"
service_reply = "0.0"
####################################
LAN_IN_services = ""

LAN_OUT_services = ""
dict_lan_delay = {}
dict_lan_cost = {}
for i in type_dict['lan']:
    with open(args.catalog_path+"/lan/Type"+str(i)+".json") as lan_infos:
      lan_inf = json.load(lan_infos)
    dict_lan_delay[i] = 0
    aggregation_rates_lan = ""
    service_time_lan = ""

    for element in lan_inf:
        if element == "delay":
            dict_lan_delay[i] = lan_inf[element]
        elif element == "cost":
            dict_lan_cost[i] = lan_inf[element]

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
with open(args.catalog_path+"/central/"+central_type+".json") as central_infos:
  central_inf = json.load(central_infos)
aggregation_rates_central = ""
service_time_central = ""
for element in central_inf:
    if element == 'prob_command_generated':
        prob_command_generated_central = str(central_inf[element])
    elif element == "scheduler_type":
        central_scheduler = central_inf[element]
    elif element == "cost":
        central_cost = central_inf["cost"]
    elif element == "service_times":
        inner_count = 0
        for element2 in central_inf[element]:
            if inner_count == 0:
                service_time_central+=str(central_inf[element][element2])
                inner_count+=1
            else:
                service_time_central+="/"+str(central_inf[element][element2])
    elif element == "cores":
        central_cores = str(central_inf[element])
    elif element == "delay_upper_router":
        delay_upper_router_central = str(central_inf[element])
    elif element == "delay_lower_router":
        delay_lower_router_central = str(central_inf[element])
    elif element == "delay_wan":
        delay_wan_central = str(central_inf[element])
    elif element == "cost_wan":
        cost_wan_central = str(central_inf[element])
    else:
        inner_count = 0
        for element2 in central_inf[element]:
            if inner_count == 0:
                aggregation_rates_central+=str(central_inf[element][element2])
                inner_count+=1
            else:
                aggregation_rates_central+="/"+str(central_inf[element][element2])

with open(args.catalog_path+"/disk/"+disk_type+".json") as disk_infos:
  disk_inf = json.load(disk_infos)
aggregation_rates_disk = ""
service_time_disk = ""

for element in disk_inf:
    if element == "disk_type":
        disk_type_string = disk_inf[element]
    if element == "cost":
        disk_cost = disk_inf[element]
    elif element == "service_times":
        inner_count = 0
        service_time_disk = ""
        for element2 in disk_inf[element]:
            if inner_count == 0:
                service_time_disk+=str(disk_inf[element][element2])
                inner_count+=1
            else:
                service_time_disk+="/"+str(disk_inf[element][element2])

dict_regional = {}
for i in type_dict['regional']:
    with open(args.catalog_path+"/regional/"+i+".json") as regional_infos:
        regional_inf = json.load(regional_infos)
    dict_regional[i] = {}
    aggregation_rates = ""
    service_time = ""
    for element in regional_inf:
        if element == 'prob_command_generated':
            dict_regional[i][element] = str(regional_inf[element])
        elif element == "scheduler_type":
            dict_regional[i][element] = regional_inf[element]
        elif element == "service_times":
            inner_count = 0
            for element2 in regional_inf[element]:
                if inner_count == 0:
                    service_time+=str(regional_inf[element][element2])
                    inner_count+=1
                else:
                    service_time+="/"+str(regional_inf[element][element2])
        elif element == "cores":
            dict_regional[i][element] = str(regional_inf[element])
        elif element == "cost":
            dict_regional[i][element] = str(regional_inf[element])
        elif element == "delay_upper_router":
            dict_regional[i][element] = str(regional_inf[element])
        elif element == "delay_lower_router":
            dict_regional[i][element] = str(regional_inf[element])
        elif element == "delay_wan":
            dict_regional[i][element] = str(regional_inf[element])
        elif element == "cost_wan":
            dict_regional[i][element] = str(regional_inf[element])
        else:
            inner_count = 0
            for element2 in regional_inf[element]:
                if inner_count == 0:
                    aggregation_rates+=str(regional_inf[element][element2])
                    inner_count+=1
                else:
                    aggregation_rates+="/"+str(regional_inf[element][element2])

    dict_regional[i]['aggregation_rates'] = aggregation_rates
    dict_regional[i]['service_time'] = service_time

to_write ="0;-1;13;NODE,"+central_scheduler+",CENTRAL,"+central_cores+","+str(central_cost)+","+aggregation_rates_central+","+delay_upper_router_central+","+delay_lower_router_central+","
to_write+=service_time_central+","+prob_command_generated_central+","+disk_type_string+","+service_time_disk+","+str(disk_cost)+"\n"
associated_wan_down =str(id_wan_central)+";0;4;WAN,0,"+str(cost_wan_central)+","+delay_wan_central+"\n"
f_out.write(to_write)
LP_index = 0
LP_start_list="0"+","+str(id_wan_central)+","
LP_num_elements = 2
wan_id+=1
#REGIONAL+LOCAL
associated_lan_down = ""
lan_id = number_of_elements+number_of_WANS
count = number_of_elements+1 #the wans!
sensor_actuator_string = ""
flagCentral = True
index = 1
indexLocal = number_of_regionals+1

f_out_txt.write("REGIONALS\n")
locals_string_start = "LOCALS\n"
locals_string = ""
actuator_start = "ACTUATORS\n"
act_string = ""
lan_start = "LAN\n"
lan_string = ""
for i in range(countReg):
    upper_node = wan_id
    amount_same_regionals = dict_total[i][0]
    regional_count = 0
    while regional_count < int(amount_same_regionals):
        regional_type_now = dict_total[i][1]

        to_write = str(index)+";"+str(id_wan_central)+";10;NODE,"+dict_regional[regional_type_now]['scheduler_type']+",REGIONAL,"+dict_regional[regional_type_now]['cores']+","+dict_regional[regional_type_now]['cost']+","+dict_regional[regional_type_now]['aggregation_rates']+","+str(dict_regional[regional_type_now]['delay_lower_router'])+","+str(dict_regional[regional_type_now]['delay_upper_router'])+","
        to_write+=dict_regional[regional_type_now]['service_time']+","+dict_regional[regional_type_now]['prob_command_generated']+"\n"
        f_out.write(to_write)

        regional_string = str(index)+";"+regional_type_now#regional id, type, #below local of each type

        associated_wan_down +=str(wan_id)+";"+str(index)+";4;WAN,0,"+str(dict_regional[regional_type_now]['cost_wan'])+","+dict_regional[regional_type_now]['delay_wan']+"\n"
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
        local_string_for_regional = ''
        for local_types in dict_total[i][2]:

            local_amounts_this_iteration = dict_total[i][2][local_types]

            regional_string+=";"+local_types+","+local_amounts_this_iteration


            same_locals = 0
            prob_command_generated_local = local_infos_dict[local_types]['prob_command_generated']
            local_scheduler = local_infos_dict[local_types]['scheduler_type']
            cores_local = local_infos_dict[local_types]['cores']
            service_time_local = local_infos_dict[local_types]['service_times']
            aggregation_rates_local = local_infos_dict[local_types]['aggregation_rates']
            delay_lower_router_local = str(local_infos_dict[local_types]['delay_lower_router'])
            delay_upper_router_local = str(local_infos_dict[local_types]['delay_upper_router'])
            cost_local = str(local_infos_dict[local_types]['cost'])
            while(same_locals < int(local_amounts_this_iteration)):


                to_write = str(indexLocal)+";"+str(count)+";10;NODE,"+local_scheduler+",LOCAL,"+cores_local+","+cost_local+","+aggregation_rates_local+","+delay_lower_router_local+","+delay_upper_router_local+","
                to_write+=service_time_local+","+prob_command_generated_local+"\n"
                f_out.write(to_write)
                if locals_string == "":
                    locals_string+=str(count)+","+str(indexLocal)+","+str(local_types)
                else:
                    locals_string+=";"+str(count)+","+str(indexLocal)+","+str(local_types)


                LP_start_list+=","+str(indexLocal)
                counter_elements = 1

                for lans in local_infos_dict[local_types]['lan']:
                    type_lan = local_infos_dict[local_types]['lan'][lans]['type_lan']
                    amount = local_infos_dict[local_types]['lan'][lans]['amount']
                    lan_cost = dict_lan_cost[type_lan]
                    amount_count = 0
                    while amount_count < amount:
                        associated_lan_down+=str(lan_id)
                        associated_lan_down+=";"+str(indexLocal)+";4;LAN,"+str(lan_cost)+","+str(dict_types_association['lan'][type_lan])+","+str(dict_lan_delay[type_lan])+"\n" #here need type, delay_lan_list[type]
                        LP_start_list+=","+str(lan_id)
                        if lan_string == "":
                            lan_string+=str(indexLocal)+","+str(lan_id)+",Type"+type_lan
                        else:
                            lan_string+=";"+str(indexLocal)+","+str(lan_id)+",Type"+type_lan
                        son_list = ""
                        counter_elements+=1
                        if 'sensor' in local_infos_dict[local_types]['lan'][lans]:

                            for sens_type in local_infos_dict[local_types]['lan'][lans]['sensor']:
                                inner_sens_count = 0
                                if son_list == "":
                                    son_list+=","+str(sens_type)+"/"+str(local_infos_dict[local_types]['lan'][lans]['sensor'][sens_type])
                                else:
                                    son_list+="."+str(sens_type)+"/"+str(local_infos_dict[local_types]['lan'][lans]['sensor'][sens_type])


                                while inner_sens_count < local_infos_dict[local_types]['lan'][lans]['sensor'][sens_type]:
                                    inner_sensor_count = 0
                                    sensor_actuator_string+= str(sensors_start)+";"+str(lan_id)+";5;SENSOR,"+str(dict_sensors[sens_type]['cost'])+","+dict_sensors[sens_type]['job_type']+","+str(dict_types_association['sensor'][sens_type])+","+dict_sensors[sens_type]['measure_type']+"\n"
                                    LP_start_list+=","+str(sensors_start)

                                    counter_elements+=1
                                    sensors_start+=1
                                    inner_sens_count+=1

                        if 'actuator' in local_infos_dict[local_types]['lan'][lans]:
                            son_list+=","
                            for act_type in local_infos_dict[local_types]['lan'][lans]['actuator']:
                                inner_act_count = 0
                                if son_list[-1] == ",":
                                    son_list+=act_type+"/"+str(local_infos_dict[local_types]['lan'][lans]['actuator'][act_type])
                                else:
                                    son_list+="."+act_type+"/"+str(local_infos_dict[local_types]['lan'][lans]['actuator'][act_type])

                                while inner_act_count < local_infos_dict[local_types]['lan'][lans]['actuator'][act_type]:
                                    sensor_actuator_string += str(sensors_start)+";"+str(lan_id)+";7;ACTUATOR,"+str(dict_actuators[act_type]['cost'])+","+dict_actuators[act_type]['job_type']+","+str(dict_types_association['actuator'][act_type])+","+dict_actuators[act_type]['measure_type']+","+str(dict_actuators[act_type]['rate_trans_act'])+","+str(dict_actuators[act_type]['service_time_commands_act'])+"\n"
                                    if act_string == "":
                                        act_string+=str(lan_id)+","+str(sensors_start)+","+str(act_type)
                                    else:
                                        act_string+=";"+str(lan_id)+","+str(sensors_start)+","+str(act_type)
                                    LP_start_list+=","+str(sensors_start)

                                    counter_elements+=1
                                    sensors_start+=1
                                    inner_act_count+=1
                        lan_string+=son_list
                        #print(lan_string)
                        lan_id+=1
                        amount_count+=1

                tot +=counter_elements
                indexLocal+=1
                same_locals+=1
        f_out_txt.write(regional_string+"\n")
        count+=1
        regional_count+=1

        tot = LP_num_elements+tot
        LP_index+=1
        wan_id+=1
        index+=1

f_out_txt.write(locals_string_start+locals_string+"\n")
f_out_txt.write(actuator_start+act_string+"\n")
f_out_txt.write(lan_start+lan_string)
f_out.write(sensor_actuator_string)
f_out.write(associated_wan_down)
f_out.write(associated_lan_down)
f_out.close()
