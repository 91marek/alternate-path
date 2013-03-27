import csv
import datetime
import subprocess
import time 

times=[]
for i in xrange(10):
	subprocess.check_call(["./generator", "graph.dot", "-n", "20", "-e", "2", "-p", "30"])
	start = datetime.datetime.now()
	# definicja procesu
	subprocess.check_call(["./ap", "-f", "graph.dot", "-s", "1", "-t", "15", "-o", "result"])
	end = datetime.datetime.now()
	times.append((end-start).total_seconds())
file_name="times"
with open(file_name+'.csv', 'wb') as f:	
	writer = csv.writer(f, delimiter=';')
	for t in times:
		writer.writerow(str(t))
print times
