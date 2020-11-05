# coding=utf-8
import numpy as np
f_out = open("../../tree_simulator/topology.txt", "w")
f_out_LP = open("../../tree_simulator/LP.txt", "w")

f_out_txt = open("../../jsonMerger/jsonAdditionalInfo.txt","w")
numbersString = ""

#TODO, don't touch
types_of_lans = 1
types_of_sensors = 2
types_of_actuators = 1

#USER SHOULD CHANGE THESE
####################################
aggregation_rates = "3/3/4/6,"
sevice_time_central = "1.5/1.0/1.0/0.5/1.0,"
service_time_regionals ="0.5/0.2/0.3/0.2/0.2,"
service_time_locals ="1.5/1.0/1.0/0.5/1.0,"
delay_upper_router = "0.3,"
delay_lower_router = "0.2,"
delay_lan = "0.3\n"
delay_wan = "0.5\n"
prob_command_generated_central = "0.1,"
prob_command_generated_regional = "0.1"
prob_command_generated_local = "0.1"
service_time_disks = "0.4/0.73/0.00/0.23\n"
service_time_commands_act = "0.6\n"
#1 CENTRAL, implicit
number_of_regionals = 20
f_out_txt.write(str(number_of_regionals)+"\n")
number_of_locals = 100
f_out_txt.write(str(int(number_of_locals/number_of_regionals))+"\n")
sensors_start = 1+number_of_regionals+number_of_locals
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
for i in range(number_of_regionals):
    number_of_locals_with_x_sensors_y_actuators_per_regional[i][num_sens_tel][num_sens_trans][num_act] = 5 #[id_regionale][#sensori tipo 1][#sensori_tipo2][#attuatori]
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
f_out_LP.write(str(1+number_of_regionals+number_of_locals)+"\n");
#USER SHOULD CHANGE THESE
####################################
rate_trans_act = "0.0005,"
f_out_txt.write(str(rate_trans_act[:-1])+"\n")
sens_tele_type1 = "0.0002"
f_out_txt.write(str(sens_tele_type1)+"\n")
sens_trans_type1 = "0.0"
sens_tele_type2 = "0.0"
sens_trans_type2 = "0.0005"
f_out_txt.write(str(sens_trans_type2)+"\n")
####################################
sensor_rates_string1 = sens_tele_type1+","+sens_trans_type1+";" #in base al tipo fai più di ste string?
sensor_rates_string2 = sens_tele_type2+","+sens_trans_type2 #in base al tipo fai più di ste string?
sensor_rates_string = sensor_rates_string1+sensor_rates_string2
f_out.write(sensor_rates_string+"\n")

associated_wan_up = ""
#lan
#USER SHOULD CHANGE THESE
####################################
service_tele = "0.2"
service_trans = "0.2"
service_command = "0.1"
service_batch = "0.3"
service_reply = "0.11"
####################################
LAN_IN_services = service_tele +"," +service_trans+","+service_command+","+service_batch+","+service_reply
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
f_out.write(to_write)
LP_index = 0
to_write = str(LP_index)+";"+str(2)+";"+"0"+","+str(id_wan_central)+"\n"
LP_index+=1
f_out_LP.write(to_write)
wan_id+=1
#REGIONAL+LOCAL
index = 1
while index < (1+number_of_regionals):
    upper_node = wan_id
    to_write = str(index)+";"+str(id_wan_central)+";8;NODE,SCHEDULER2,REGIONAL,"+aggregation_rates+delay_lower_router+delay_lower_router
    to_write+=service_time_regionals+prob_command_generated_regional+"\n"
    f_out.write(to_write)

    associated_wan_down +=str(wan_id)+";"+str(index)+";3;WAN,WAN_TYPE0,"+delay_wan
    to_write = str(LP_index)+";"+str(2)+";"+str(index)+","+str(wan_id)+"\n"
    LP_index+=1
    f_out_LP.write(to_write)

    wan_id+=1
    index+=1

total_so_far = 1+number_of_regionals
inner = 0
local_per_regional = number_of_locals/number_of_regionals
associated_lan_down = ""
lan_id = number_of_elements+number_of_WANS
count = number_of_elements+1 #the wans!
sensor_actuator_string = ""
for i in range(number_of_regionals):
    for j in range(different_amounts_sensors_type1) :
        for l in range(different_amounts_sensors_type2):
            for k in range(different_amounts_actuators):
                if(number_of_locals_with_x_sensors_y_actuators_per_regional[i][j][l][k] != 0):
                    for last in range(number_of_locals_with_x_sensors_y_actuators_per_regional[i][j][l][k]):
                        LP_start = str(LP_index)+";"
                        counter_elements = 2
                        to_write = str(index)+";"+str(count)+";8;NODE,SCHEDULER2,LOCAL,"+aggregation_rates+delay_lower_router+delay_lower_router
                        to_write+=service_time_locals+prob_command_generated_local+"\n"
                        f_out.write(to_write)
                        LP_to_write=";"+str(index)+","+str(lan_id)
                        associated_lan_down+=str(lan_id)

                        upper_lan_node = index
                        associated_lan_down+=";"+str(index)+";3;LAN,LAN_TYPE0,"+delay_lan
                        for sens in range(j):
                            sensor_actuator_string+= str(sensors_start)+";"+str(lan_id)+";4;SENSOR,BATCH,SENSOR_TYPE0,MEASURE0\n"
                            LP_to_write+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        for sens2 in range(l):
                            sensor_actuator_string+= str(sensors_start)+";"+str(lan_id)+";4;SENSOR,BATCH,SENSOR_TYPE1,MEASURE0\n"
                            LP_to_write+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        for act in range(k):
                            sensor_actuator_string += str(sensors_start)+";"+str(lan_id)+";6;ACTUATOR,BATCH,ACTUATOR_TYPE0,MEASURE0,"+rate_trans_act+service_time_commands_act
                            LP_to_write+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        lan_id+=1
                        index+=1
                        total_write = LP_start+str(counter_elements)+LP_to_write+"\n"
                        f_out_LP.write(total_write)
                        LP_index+=1

                    count+=1


f_out.write(sensor_actuator_string)
f_out.write(associated_wan_down)
f_out.write(associated_lan_down)
f_out.close()
