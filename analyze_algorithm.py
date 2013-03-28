import csv
import datetime
import subprocess
import time 
import random
#configurable params
vertex_capacity = 100
edges_capacity = 2
move_probability = 20 # from 0 to 100
#end

parms = [] #exc_time, path_len
subprocess.check_call(["./gg", "graph.dot", "-n", str(vertex_capacity), "-e", str(edges_capacity), "-p", str(move_probability)])
for i in xrange(30):	
	source_vertex = random.randint(0, vertex_capacity-1)
	target_vertex = random.randint(0, vertex_capacity-1)
	if source_vertex == target_vertex:
		source_vertex = (target_vertex + 1)%vertex_capacity

	print source_vertex, target_vertex
	start = datetime.datetime.now()
	subprocess.call(["./ap", "-f", "graph.dot", "-s", str(source_vertex), "-t", str(target_vertex), "-o", "result"])
	end = datetime.datetime.now()
	path_len_file = open("path_len.out", 'r')
	path_len = float(path_len_file.readline())
	path_len_file.close()
	parms.append(((end-start).total_seconds(), path_len))
file_name="times"
with open(file_name+'.csv', 'wb') as f:	
	writer = csv.writer(f, delimiter=';')
	writer.writerow(["czas wykonania", "dlugosc sciezki"])
	for exc_time, path_len in parms:
		writer.writerow([str(exc_time), str(path_len)])
subprocess.check_call(["rm", "graph.dot", "path_len.out"])
