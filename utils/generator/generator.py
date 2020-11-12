# coding=u        print(element[2:])
utf-8
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
        number_of_regionals+=int(element[0])
        number_of_locals+=int(element[0]) * int(element[2:].strip())

    aggregation_rates = lines[4][36:-1]+","
    service_time_central = lines[5][36:-1]+","
    service_time_regionals = lines[6][36:-1]+","
    service_time_locals = lines[7][36:-1]+","
    delay_upper_router = lines[8][36:-1]+","
    delay_lower_router = lines[9][36:-1]+","
    delay_lan = lines[10][36:-1]+"\n"
    delay_wan = lines[11][36:-1]+"\n"
    prob_command_generated_central =lines[12][36:-1]+","
    prob_command_generated_regional =lines[13][36:-1]
    prob_command_generated_local =lines[14][36:-1]
    service_time_disks =lines[15][36:-1]+"\n"
    service_time_commands_act =lines[16][36:-1]+"\n"
    rate_trans_act = lines[17][36:-1]+","
    sens_tele_type1 = lines[18][36:-1]
    sens_trans_type2 = lines[19][36:-1]
    service_tele = lines[20][36:-1]
    service_trans = lines[21][36:-1]
    service_command = lines[22][36:-1]
    weight = lines[23][36:-1]
    types_of_lans = 1
types_of_sensors = 2
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
    regionals_temp = int(element[0])
    locals_temp = int(element[2:].strip())
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
f_out_txt.write(str(rate_trans_act[:-1])+"\n")
#sens_tele_type1 = "0.0002"
f_out_txt.write(str(sens_tele_type1)+"\n")
sens_trans_type1 = "0.0"
sens_tele_type2 = "0.0"
#sens_trans_type2 = "0.0005"
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
#service_tele = "0.2"
#service_trans = "0.2"
#service_command = "0.1"
service_batch = "0.0"
service_reply = "0.0"
####################################
LAN_IN_services = service_tele +"," +service_trans+","+service_command+","+service_batch+","+service_reply
f_out.write(LAN_IN_services+"\n")

LAN_OUT_services = LAN_IN_services
f_out.write(LAN_OUT_services+"\n")
#weight = "1"
f_out.write(weight+"\n")
#NODES
#CENTRAL
wan_id = number_of_elements
id_wan_central = wan_id
to_write ="0;-1;10;NODE,SCHEDULER1,CENTRAL,"+aggregation_rates+delay_lower_router+delay_lower_router
to_write+=service_time_central+prob_command_generated_central+"RAID3,"+service_time_disks
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
while index < (1+number_of_regionals):
    upper_node = wan_id
    to_write = str(index)+";"+str(id_wan_central)+";8;NODE,SCHEDULER2,REGIONAL,"+aggregation_rates+delay_lower_router+delay_lower_router
    to_write+=service_time_regionals+prob_command_generated_regional+"\n"
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
                        to_write = str(indexLocal)+";"+str(count)+";8;NODE,SCHEDULER2,LOCAL,"+aggregation_rates+delay_lower_router+delay_lower_router
                        to_write+=service_time_locals+prob_command_generated_local+"\n"
                        f_out.write(to_write)

                        LP_start_list+=","+str(indexLocal)+","+str(lan_id)

                        associated_lan_down+=str(lan_id)
                        upper_lan_node = indexLocal
                        associated_lan_down+=";"+str(indexLocal)+";3;LAN,LAN_TYPE0,"+delay_lan
                        for sens in range(j):
                            sensor_actuator_string+= str(sensors_start)+";"+str(lan_id)+";4;SENSOR,BATCH,SENSOR_TYPE0,MEASURE0\n"
                            LP_start_list+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        for sens2 in range(l):
                            sensor_actuator_string+= str(sensors_start)+";"+str(lan_id)+";4;SENSOR,BATCH,SENSOR_TYPE1,MEASURE0\n"
                            LP_start_list+=","+str(sensors_start)
                            counter_elements+=1
                            sensors_start+=1
                        for act in range(k):
                            sensor_actuator_string += str(sensors_start)+";"+str(lan_id)+";6;ACTUATOR,BATCH,ACTUATOR_TYPE0,MEASURE0,"+rate_trans_act+service_time_commands_act
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
