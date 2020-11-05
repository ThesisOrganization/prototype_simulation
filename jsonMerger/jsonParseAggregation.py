import json
import numpy as np
import os.path
def result_string_calculation(dict,params, string1, string2):
    ret = "$"
    flag = True
    for elem2 in params:
        if flag :
            flag = False
            ret+= str(dict[element][string1][elem2][string2])
        else:
            ret+="$ & $"
            ret+= str(dict[element][string1][elem2][string2])
    ret+="$"
    return ret

def utilization_factor_total(dict,string1,params):
    total = 0
    for elem2 in params:
        total += dict[element][string1][elem2]['utilization_factor']
    return total

def aux(begin_table,dict,string):
    table_string = begin_table
    table_string+="$\\lambda_t$ & $\\lambda_e$ & $\\lambda_c$ & $\\lambda_b$"
    table_string+= "\\"
    table_string+= "\\"
    table_string+="\n"
    f_out.write(table_string)

    lambdas_string = result_string_calculation(dict,params, string, 'lambda_in')
    lambdas_string+="\\\\"
    to_print = "\\midrule\n"+ lambdas_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}"
    f_out.write(to_print)

    to_print= begin_table
    to_print+="$D_t$ & $D_e$ & $D_c$ & $D_b$"
    to_print+= "\\"
    to_print+= "\\"
    to_print+="\n"
    f_out.write(to_print)

    D_string = result_string_calculation(dict,params, string, 'service_demand')

    D_string+="\\\\"
    to_print = "\\midrule\n"+ D_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}"
    f_out.write(to_print)

    to_print = begin_table
    to_print+="$U_t$ & $U_e$ & $U_c$ & $U_b$"
    to_print+="\\"
    to_print+="\\"
    to_print+="\n"
    f_out.write(to_print)
    U_string = result_string_calculation(dict,params, string, 'utilization_factor')
    total = utilization_factor_total(dict,string,params)

    U_string+="\\\\"
    to_print = "\\midrule\n"+ U_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}\n"
    f_out.write(to_print)

    total_u = "\\centering Total Utlization Factor = $" + f"{total:.3}" + "$\n"
    f_out.write(total_u)

    f_out.write(begin_table)
    to_print="$R_t$ & $R_e$ & $R_c$ & $R_b$"
    to_print+= "\\"
    to_print+= "\\"
    to_print+="\n"
    f_out.write(to_print)

    R_string = result_string_calculation(dict,params, string, 'response_time')
    R_string+="\\\\"
    to_print = "\\midrule\n"+ R_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}\n"
    f_out.write(to_print)

with open('../tree_simulator/simulation_results.json') as f_simulator:
  data_simulator = json.load(f_simulator)
with open('../model_computation/model_res.json') as f_model:
  data_model = json.load(f_model)

flagStable = True
count = 0;
dict_simulator = {}
dict_model = {}
num_elements = len(data_simulator)
ordered_id_list = []
while(count < num_elements):
    id_simulator = data_simulator[count]['id']
    dict_simulator[id_simulator] = {}
    ordered_id_list.append(id_simulator);
    for element in data_simulator[count]:
        if element != 'id':
            dict_simulator[id_simulator][element] = data_simulator[count][element]
        if element == 'stable':
            if (dict_simulator[id_simulator][element] != 1):
                flagStable = False
    id_model = data_model[count]['id']
    dict_model[id_model] = {}
    for element in data_model[count]:
        if element != 'id':
            dict_model[id_model][element] = data_model[count][element]

    count+=1

ordered_id_list.sort()
dict_res = {}
list = ['lambda_in','service_demand','utilization_factor','response_time']
params = ["telemetry","transition","command","batch"]
######NEW PART
for element in ordered_id_list:
    dict_res[element] = {}
    for i in list:
        dict_res[element][i] = []
        for c in params:
            type = dict_simulator[element]['type']
            if type == 'lan':
                if c == 'command':
                    dict_res[element][i].append(dict_simulator[element]['lan_in'][c][i])
                else:
                    dict_res[element][i].append(dict_simulator[element]['lan_out'][c][i])
            else:
                dict_res[element][i].append(dict_simulator[element]['parameters'][c][i])
    dict_res[element]["stable"] = dict_simulator[element]["stable"];

list_regional = []
list_local = []
list_lan = []
list_actuators = []
for element in ordered_id_list:
    type = dict_model[element]['type']
    if(type == 'node'):
        node_type = dict_model[element]['node_type']
        if(node_type == 'regional'):
            list_regional.append(element)
        elif(node_type == 'local'):
            list_local.append(element)
    elif(type == 'lan'):
        list_lan.append(element)
    elif(type == 'actuator'):
        list_actuators.append(element)
#######################
#print("#######15%!########")
flag = False
if(flag):
    baseline_coefficient = 0.15
    dict_regional_similar = {}
    for element in list_regional:
        for element2 in list_regional:
            if(element < element2):
                allclose_result = True
                for c in list:
                    allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],baseline_coefficient)
                allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]
                if(allclose_result):
                    if(element not in dict_regional_similar):
                        dict_regional_similar[element] = []
                    dict_regional_similar[element].append(element2)
    to_pop_list = []
    for element in dict_regional_similar.keys():
        for element2 in dict_regional_similar.keys():
            if element in dict_regional_similar[element2]:
                to_pop_list.append(element)
    for element in to_pop_list:
        dict_regional_similar.pop(element,None)
    print(dict_regional_similar)
    print("END REGIONAL##############")
    dict_local_similar = {}
    for element in list_local:
        for element2 in list_local:
            if(element < element2):
                allclose_result = True
                for c in list:
                    allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],baseline_coefficient)
                allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]
                if(allclose_result):
                    if(element not in dict_local_similar):
                        dict_local_similar[element] = []
                    dict_local_similar[element].append(element2)
    to_pop_list = []
    for element in dict_local_similar.keys():
        for element2 in dict_local_similar.keys():
            if element in dict_local_similar[element2]:
                to_pop_list.append(element)
    for element in to_pop_list:
        dict_local_similar.pop(element,None)
    print(dict_local_similar)
    print("END LOCAL##############")
    dict_lan_similar = {}

    for element in list_lan:
        for element2 in list_lan:
            if(element < element2):
                allclose_result = True
                for c in list:
                    allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],baseline_coefficient)
                allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]

                if(allclose_result):
                    if(element not in dict_lan_similar):
                        dict_lan_similar[element] = []
                    dict_lan_similar[element].append(element2)

    to_pop_list = []
    for element in dict_lan_similar.keys():
        for element2 in dict_lan_similar.keys():
            if element in dict_lan_similar[element2]:
                to_pop_list.append(element)
    for element in to_pop_list:
        dict_lan_similar.pop(element,None)
    print(dict_lan_similar)
    print("END LAN ######################")
    dict_actuator_similar = {}

    for element in list_actuators:
        for element2 in list_actuators:
            if(element < element2):
                allclose_result = True
                for c in list:
                    allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],baseline_coefficient)
                allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]
                if(allclose_result):
                    if(element not in dict_actuator_similar):
                        dict_actuator_similar[element] = []
                    dict_actuator_similar[element].append(element2)

    to_pop_list = []
    for element in dict_actuator_similar.keys():
        for element2 in dict_actuator_similar.keys():
            if element in dict_actuator_similar[element2]:
                to_pop_list.append(element)
    for element in to_pop_list:
        dict_actuator_similar.pop(element,None)
    print(dict_actuator_similar)
    print("END ACTUATORS ######################")
#####################
similarity_coefficient = 0.2
print("#####20%!#####")
dict_regional_similar = {}
for element in list_regional:
    for element2 in list_regional:
        if(element < element2):
            allclose_result = True
            for c in list:
                allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],similarity_coefficient)
            allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]

            if(allclose_result):
                if(element not in dict_regional_similar):
                    dict_regional_similar[element] = []
                dict_regional_similar[element].append(element2)
to_pop_list = []
for element in dict_regional_similar.keys():
    for element2 in dict_regional_similar.keys():
        if element in dict_regional_similar[element2]:
            to_pop_list.append(element)
for element in to_pop_list:
    dict_regional_similar.pop(element,None)
print(dict_regional_similar)
print("END REGIONAL##############")
dict_local_similar = {}
for element in list_local:
    for element2 in list_local:
        if(element < element2):
            allclose_result = True
            for c in list:
                allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],similarity_coefficient)
            allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]

            if(allclose_result):
                if(element not in dict_local_similar):
                    dict_local_similar[element] = []
                dict_local_similar[element].append(element2)
to_pop_list = []
for element in dict_local_similar.keys():
    for element2 in dict_local_similar.keys():
        if element in dict_local_similar[element2]:
            to_pop_list.append(element)
for element in to_pop_list:
    dict_local_similar.pop(element,None)
print(dict_local_similar)
print("END LOCAL##############")
dict_lan_similar = {}

for element in list_lan:
    for element2 in list_lan:
        if(element < element2):
            allclose_result = True
            for c in list:
                allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],similarity_coefficient)
            allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]

            if(allclose_result):
                if(element not in dict_lan_similar):
                    dict_lan_similar[element] = []
                dict_lan_similar[element].append(element2)

to_pop_list = []
for element in dict_lan_similar.keys():
    for element2 in dict_lan_similar.keys():
        if element in dict_lan_similar[element2]:
            to_pop_list.append(element)
for element in to_pop_list:
    dict_lan_similar.pop(element,None)
print(dict_lan_similar)
print("END LAN ######################")
dict_actuator_similar = {}

for element in list_actuators:
    for element2 in list_actuators:
        if(element < element2):
            allclose_result = True
            for c in list:
                allclose_result=allclose_result and np.allclose(dict_res[element][c],dict_res[element2][c],similarity_coefficient)
            allclose_result=allclose_result and dict_res[element]["stable"] == dict_res[element2]["stable"]

            if(allclose_result):
                if(element not in dict_actuator_similar):
                    dict_actuator_similar[element] = []
                dict_actuator_similar[element].append(element2)

to_pop_list = []
for element in dict_actuator_similar.keys():
    for element2 in dict_actuator_similar.keys():
        if element in dict_actuator_similar[element2]:
            to_pop_list.append(element)
for element in to_pop_list:
    dict_actuator_similar.pop(element,None)
print(dict_actuator_similar)
print("END ACTUATORS ######################")

############END NEW PART

f_out = open("complete_results.tex", "w")
#title
initial_header = "\\documentclass{article}\n\\usepackage{booktabs}\n\\usepackage{float}\n\\title{Results}\n\\begin{document}\n\\maketitle\n"
f_out.write(initial_header);
begin_table ="\\begin{table}[H]\n\\centering\n\\begin{tabular}{@{}cccc@{}}\n\\toprule\n"
complete_table = "\\begin{table}[H]\n\\centering\n\\begin{tabular}{@{}cccc|cccc@{}}\n\\toprule\n$S_t$ & $S_e$ & $S_c$ & $S_b$ & $aggr_t$ & $aggr_e$ & $aggr_c$ & $aggr_b$\\\\"
semi_complete_table = "\\begin{table}[H]\n\\centering\n\\begin{tabular}{@{}cccc@{}}\n\\toprule\n$S_t$ & $S_e$ & $S_c$ & $S_b$\\\\"
key_union = [];
for element in dict_regional_similar.keys():
    for element2 in dict_regional_similar[element]:
        key_union.append(element2)
for element in dict_local_similar.keys():
    for element2 in dict_local_similar[element]:
        key_union.append(element2)
for element in dict_lan_similar.keys():
    for element2 in dict_lan_similar[element]:
        key_union.append(element2)
for element in dict_actuator_similar.keys():
    for element2 in dict_actuator_similar[element]:
        key_union.append(element2)

with open('../tree_simulator/LP.txt') as f:
    lines = f.readlines()
    elements = lines[0].strip() #elementi
    LP = lines[1].strip() #Lp
f.close()

if os.path.isfile('../tree_simulator/outputs/sequential_stats'):
	with open("../tree_simulator/outputs/sequential_stats") as f:
			run="sequential"
			lines = f.readlines()
			time = lines[6][29:].strip()
			avg_mem = lines[15][29:].strip()
			peak_mem = lines[16][29:].strip()
else:
	with open("../tree_simulator/outputs/execution_stats") as f:
			lines = f.readlines()
			run="parallel with "+lines[4][21:-5].strip()+" cores and "+lines[28][29:].strip()+" working threads"
			time = lines[23][29:].strip()
			avg_mem = lines[48][29:].strip()
			peak_mem = lines[49][29:].strip()
f.close()

#If you don't use generator.py comment this part and lines 38
stringAdditionalInfo = ""
with open("jsonAdditionalInfo.txt") as f:
    lines = f.readlines()
    stringAdditionalInfo+="There is one Central node, between the Central node and the Regional layer there is one WAN. Between each regional and its Locals there is a WAN.\\\\"
    stringAdditionalInfo+="There are "+str(lines[0])+" regional nodes, each of them has "+str(lines[1])+" local nodes. In total "+str(int(lines[0])*int(lines[1]))+" local nodes.\\\\"
    stringAdditionalInfo+="Each local node has "+str(lines[2])+" LANs below. Each LAN has:"#LAN (è 1)
    stringAdditionalInfo+="\\begin{itemize}\n"
    stringAdditionalInfo+="\\item "+str(lines[6])+"sensors sending telemetries with rate: "+str(lines[10]).strip()+".\n"#sensori telemetry per ogni locale
    stringAdditionalInfo+="\\item "+str(lines[7])+"sensors sending transitions with rate: "+str(lines[9]).strip()+".\n"#sensori trans per ogni locale
    stringAdditionalInfo+="\\item "+str(lines[8])+"actuators sending trasitions with rate: "+str(lines[11]).strip()+".\n"#actuators per ogni locale
    stringAdditionalInfo+="\\end{itemize}\n"
    stringAdditionalInfo+="In total there are "+str(lines[4])+ "total sensors and "+str(lines[5])+" total actuators.\\\\"
    #stringAdditionalInfo+=lines[3]#WAN


to_write = "\\section{General Informations}"
to_write+="Run type: "+run+".\\\\"
to_write+= "Number of elements in the topology: "+elements+".\\\\"
to_write+= "Number of LPs used in the simulation: "+LP+".\\\\"
to_write+= "Simulation duration: "+time+".\\\\"
to_write+= "Average memory usage: "+avg_mem+".\\\\"
to_write+= "Peak memory usage: "+peak_mem+".\\\\"

if(flagStable):
    to_write+= "All elements reached stability in the simulation.\\\\"
else:
    to_write+= "Not all elements reached stability, in their sections it will be highlited!\\\\"

to_write+="\\subsection{Topology Informations}"
to_write+=stringAdditionalInfo
f_out.write(to_write)

f_out.write("\\section{Detailed view}")
for element in ordered_id_list:
    if(element not in key_union):
        type = dict_model[element]['type']
        if type == 'node':
            str_to_write = dict_model[element]['node_type'].capitalize()
            to_write = "\\subsection{"+str_to_write+" node "+str(element)+"}\n"
            f_out.write(to_write);
            if dict_simulator[element]["stable"] == 0:
                f_out.write("This element \\textbf{didn't} reach stability in the simulation!\\\\")
            f_out.write("This element finished the simulation at simulation time: "+str(dict_simulator[element]["sim_time"])+".\\\\")

            if(element in dict_regional_similar.keys() or element in dict_local_similar.keys()):
                to_write = "This node has its computed parameters $\\lambda$, utilization factor, service demand and response time similar by " + str(similarity_coefficient*100) + "\% to these other nodes: \\textbf{"
                if dict_model[element]['node_type'] == 'regional':
                    for similar in dict_regional_similar[element]:
                        to_write+= str(similar)+"; "
                    to_write+="}"
                    f_out.write(to_write)

                elif dict_model[element]['node_type'] == 'local':
                    for similar in dict_local_similar[element]:
                        to_write+= str(similar)+"; "
                    to_write+="}"

                    f_out.write(to_write)

            table_string = "\\subsubsection{Given parameters}\n"
            f_out.write(table_string)
            f_out.write(complete_table)

            return1 = result_string_calculation(dict_model,params, 'parameters', 'service_time')
            return1 +=" & "
            return1 += result_string_calculation(dict_model,params, 'parameters', 'aggregation_rate')
            return1 +="\\\\"

            to_print = "\n\\midrule\n"+return1+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}\n"
            f_out.write(to_print)

            table_string = "\\subsubsection{Computed parameters}\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Analytical Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_model,'parameters')
            f_out.write("\\end{minipage}")

            table_string = "\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Simulated Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_simulator,'parameters')
            f_out.write("\\end{minipage}")


            if dict_simulator[element]['node_type'] == 'central' :
                str_to_write = "Central storage of Node "
                to_write = "\\subsection{"+str_to_write+str(element)+"}\n"
                f_out.write(to_write);

                table_string = "\\subsubsection{Given parameters}\n"
                f_out.write(table_string)
                f_out.write(semi_complete_table)

                service_string = result_string_calculation(dict_model,params, 'parameters', 'service_time')
                service_string+="\\\\"

                to_print = "\n\\midrule\n"+service_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}\n"
                f_out.write(to_print)

                table_string = "\\subsubsection{Computed parameters}\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Analytical Model}\n"
                f_out.write(table_string)
                aux(begin_table,dict_model,'parameters')
                f_out.write("\\end{minipage}")

                table_string = "\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Simulated Model}\n"
                f_out.write(table_string)
                aux(begin_table,dict_simulator,'parameters')
                f_out.write("\\end{minipage}")

            f_out.write("\n\\newpage")

        elif type == 'actuator':
            str_to_write = "Actuator "
            to_write = "\\subsection{"+str_to_write+str(element)+"}\n"
            f_out.write(to_write);
            if dict_simulator[element]["stable"] == 0:
                f_out.write("This element \\textbf{didn't} reach stability in the simulation!\\\\")
            f_out.write("This element finished the simulation at simulation time: "+str(dict_simulator[element]["sim_time"])+".\\\\")

            if element in dict_actuator_similar.keys():
                to_write = "This actuator has its computed parameters $\\lambda$, utilization factor, service demand and response time similar by " + str(similarity_coefficient*100) + "\% to these other nodes: \\textbf{"
                for similar in dict_actuator_similar[element]:
                    to_write+= str(similar)+"; "
                to_write+="}"
                f_out.write(to_write)

            table_string = "\\subsubsection{Given parameters}\n"
            f_out.write(table_string)
            f_out.write(semi_complete_table)

            service_string = result_string_calculation(dict_model,params, 'parameters', 'service_time')
            service_string+="\\\\"

            to_print = "\n\\midrule\n"+service_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}\n"
            f_out.write(to_print)

            table_string = "\\subsubsection{Computed parameters}\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Analytical Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_model,'parameters')
            f_out.write("\\end{minipage}")

            table_string = "\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Simulated Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_simulator,'parameters')
            f_out.write("\\end{minipage}")
            f_out.write("\n\\newpage")

        elif type == 'lan':

            str_to_write = "Lan IN "
            to_write = "\\subsection{"+str_to_write+str(element)+"}\n"
            f_out.write(to_write)
            if dict_simulator[element]["stable"] == 0:
                f_out.write("This element \\textbf{didn't} reach stability in the simulation!\\\\")
            f_out.write("This element finished the simulation at simulation time: "+str(dict_simulator[element]["sim_time"])+".\\\\")

            if element in dict_lan_similar.keys():
                to_write = "This LAN has its computed parameters $\\lambda$, utilization factor, service demand and response time similar by "+ str(similarity_coefficient*100) +"\% to these other nodes: \\textbf{"
                for similar in dict_lan_similar[element]:
                    to_write+= str(similar)+"; "
                to_write+="}"
                f_out.write(to_write)

            table_string = "\\subsubsection{Given parameters}\n"
            f_out.write(table_string)
            f_out.write(semi_complete_table)

            service_string = result_string_calculation(dict_model,params, 'lan_in', 'service_time')
            service_string+="\\\\"

            to_print = "\n\\midrule\n"+service_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}"
            to_print+="\n"
            f_out.write(to_print)

            table_string = "\\subsubsection{Computed parameters}\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Analytical Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_model,'lan_in')
            f_out.write("\\end{minipage}")

            table_string = "\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Simulated Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_simulator,'lan_in')
            f_out.write("\\end{minipage}")

            str_to_write = "Lan OUT "
            to_write = "\\subsection{"+str_to_write+str(element)+"}\n"
            f_out.write(to_write);

            table_string = "\\subsubsection{Given parameters}\n"
            f_out.write(table_string)
            f_out.write(semi_complete_table)

            service_string = result_string_calculation(dict_model,params, 'lan_out', 'service_time')
            service_string+="\\\\"

            to_print = "\n\\midrule\n"+service_string+ "\n\\bottomrule\n\\end{tabular}\n\\end{table}\n"
            f_out.write(to_print)

            table_string = "\\subsubsection{Computed parameters}\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Analytical Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_model,'lan_out')
            f_out.write("\\end{minipage}")

            table_string = "\n\\begin{minipage}{0.5\\textwidth}\n\\centering	\\textbf{Simulated Model}\n"
            f_out.write(table_string)
            aux(begin_table,dict_simulator,'lan_out')
            f_out.write("\\end{minipage}")
            f_out.write("\n\\newpage")


f_out.write("\n\\end{document}")

f_out.close()
