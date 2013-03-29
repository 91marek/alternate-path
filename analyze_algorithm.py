# -*- coding: utf-8 -*-
import csv
import datetime
import subprocess
import time 
import random
import re

# IMPORTANT! Be sure to use: cp config.py.default config.py
from config import *

parms = [] #exc_time, path_len
map = {} #path_len, [exc_time]
subprocess.check_call(["./gg", "graph.dot", "-n", str(vertex_capacity), "-e", str(edges_capacity), "-p", str(move_probability)])
for i in xrange(num_of_paths):	
	source_vertex = random.randint(0, vertex_capacity-1)
	target_vertex = random.randint(0, vertex_capacity-1)
	if source_vertex == target_vertex:
		source_vertex = (target_vertex + 1)%vertex_capacity
	print source_vertex, target_vertex
	start = datetime.datetime.now()
	subprocess.call(["./ap", "-f", "graph.dot", "-s", str(source_vertex), "-t", str(target_vertex), "-o", "result", "--no-html"])
	end = datetime.datetime.now()
	path_len_file = open("result/report.txt", 'r')
	nums = re.findall(r'\b\d+\b', path_len_file.readline())
	path_len = nums[-1]
	path_len_file.close()
	duration = (end-start).total_seconds()
	parms.append((duration, path_len))
	if path_len in map:
		map[path_len].append(duration)
	else:
		map[path_len] = [duration]
with open('times.csv', 'wb') as f:
	writer = csv.writer(f, delimiter=';')
	writer.writerow(["czas_wykonania", "dlugosc_sciezki"])
	for exc_time, path_len in parms:
		writer.writerow([str(exc_time), str(path_len)])
	writer.writerow(["srednia_dlugosc_sciezki:", "=ŚREDNIA(B2:B"+str(len(parms)+1)+")"])
with open('avg_times.csv', 'wb') as f:
	writer = csv.writer(f, delimiter=';')
	writer.writerow(["dlugosc_sciezki", "sredni_czas_wykonania"])
	for key in sorted(map.keys(), key=int):
		writer.writerow([str(key), str(sum(map[key])/float(len(map[key])))])
subprocess.check_call(["rm", "-r", "result"])
print 'Generating graph picture ...'
subprocess.check_call(["neato", "-Tgif", "-ograph.gif", "graph.dot", "-Goverlap=false", "-Gsplines=true"])
