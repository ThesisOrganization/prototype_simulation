# coding=utf-8
f_out = open("../../tree_simulator/topology.txt", "w")

numbersString = ""

types_of_lans = 1
types_of_sensors = 1
types_of_actuators = 1

aggregation_rates = "10/2/3/8,"
sevice_time_central = "0.82/0.35/0.1/0.3/0.52,"
service_time_regionals ="0.32/0.13/0.8/0.12/0.23,"
service_time_locals ="0.32/0.13/0.8/0.12/0.23,"
delay_upper_router = "0.3,"
delay_lower_router = "0.2,"
delay_lan = "0.3\n"
delay_wan = "0.5\n"
prob_command_generated_central = "0.32,"
prob_command_generated_regional = "0.4"
prob_command_generated_local = "0.2"
service_time_disks = "0.4/0.73/0.00/0.23\n"

rate_trans_act = "0.15,"
service_time_commands_act = "0.6\n"
#1 CENTRAL
number_of_regionals = 2
number_of_locals = 4
number_of_sensors_per_local = 5
number_of_actuators_per_local = 1
number_of_lans_per_local = 1 #to change
number_of_WANS = 1 + number_of_regionals

number_of_elements = 1 + number_of_regionals + number_of_locals*(1+number_of_sensors_per_local + number_of_actuators_per_local)
number_of_elements_to_write = 1 + number_of_regionals + number_of_locals*(1+number_of_sensors_per_local + number_of_actuators_per_local+number_of_lans_per_local)+number_of_WANS
numbersString = str(number_of_elements_to_write)+"\n"
numbersString+=str(types_of_actuators)+"\n"
numbersString+=str(types_of_sensors)+"\n"
numbersString+=str(types_of_lans)+"\n"
f_out.write(numbersString);

sens_tele = "0.2"
sens_trans = "0.1"

sensor_rates_string = sens_tele+","+sens_trans #in base al tipo fai più di ste string?
f_out.write(sensor_rates_string+"\n")

associated_wan_up = ""
#lan
service_tele = "0.42"
service_trans = "0.33"
service_command = "0.12"
service_batch = "0.3"

LAN_IN_services = service_tele +"," +service_trans+","+service_command+","+service_batch
f_out.write(LAN_IN_services+"\n")

LAN_OUT_services = LAN_IN_services #lo aggiusto dopo, giuro
f_out.write(LAN_OUT_services+"\n")
weight = "1"
f_out.write(weight+"\n")
#NODES
#CENTRAL
wan_id = number_of_elements
id_wan_central = wan_id
to_write ="0;-1;10;NODE,SCHEDULER1,CENTRAL,"+aggregation_rates+delay_lower_router+delay_lower_router
to_write+=sevice_time_central+prob_command_generated_central+"RAID3,"+service_time_disks
associated_wan_down =str(id_wan_central)+";0;3;WAN,WAN_TYPE0,"+delay_wan
wan_id+=1
f_out.write(to_write)
#REGIONAL+LOCAL
index = 1
while index < (1+number_of_regionals):
    upper_node = wan_id
    to_write = str(index)+";"+str(id_wan_central)+";8;NODE,SCHEDULER2,REGIONAL,"+aggregation_rates+delay_lower_router+delay_lower_router
    to_write+=service_time_regionals+prob_command_generated_regional+"\n"
    f_out.write(to_write)

    associated_wan_down +=str(wan_id)+";"+str(index)+";3;WAN,WAN_TYPE0,"+delay_wan
    wan_id+=1

    index+=1

total_so_far = 1+number_of_regionals
inner = 0
local_per_regional = number_of_locals/number_of_regionals
associated_lan_down = ""
lan_id = number_of_elements+number_of_WANS
count = number_of_elements+1 #the wans!
while index < (total_so_far+number_of_locals):
    if(inner == local_per_regional):
        inner = 0
        count+=1
    inner+=1
    upper_node = count
    to_write = str(index)+";"+str(upper_node)+";8;NODE,SCHEDULER2,LOCAL,"+aggregation_rates+delay_lower_router+delay_lower_router
    to_write+=service_time_locals+prob_command_generated_local+"\n"
    f_out.write(to_write)

    associated_lan_down+=str(lan_id)
    lan_id+=1
    upper_lan_node = index
    associated_lan_down+=";"+str(index)+";3;LAN,LAN_TYPE1,"+delay_lan
    index+=1

lan_up_id = number_of_elements+number_of_WANS
total_so_far+=number_of_locals
inner=0
while index < (total_so_far+number_of_sensors_per_local*number_of_locals):
    if(inner == number_of_sensors_per_local):
        inner = 0
        lan_up_id+=1
    inner+=1
    upper_node = lan_up_id
    to_write = str(index)+";"+str(upper_node)+";4;SENSOR,BATCH,SENSOR_TYPE0,MEASURE0\n"
    f_out.write(to_write)
    index+=1
total_so_far+=number_of_sensors_per_local*number_of_locals
inner = 0
lan_up_id = number_of_elements+number_of_WANS
while index < (total_so_far+number_of_actuators_per_local*number_of_locals):
    if(inner == number_of_actuators_per_local):
        inner = 0
        lan_up_id+=1
    inner+=1
    upper_node = lan_up_id
    to_write = str(index)+";"+str(upper_node)+";6;ACTUATOR,BATCH,ACTUATOR_TYPE0,MEASURE0,"+rate_trans_act+service_time_commands_act
    f_out.write(to_write)
    index+=1

f_out.write(associated_wan_down)
f_out.write(associated_lan_down)
f_out.close()
