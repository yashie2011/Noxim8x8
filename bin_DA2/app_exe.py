#!/usr/bin/python

import matplotlib.pyplot as plt
import random 
import math
import os 
import sys

prev = 0
line_no = 0
prev_line = " "
my_list = list()
trace_count = 0
filename=(str(sys.argv[2]))
temp_line = ""
with open(str(sys.argv[1])) as f:
	for line in f:
		new_line=line.split()
		#print new_line[8]
		diff = int(new_line[8]) - (prev)
		if (line_no > 0): 
			if (diff > 120): 
				comp_time = math.floor(diff- random.uniform(100, 120))
			else :
				comp_time = 0
			my_list.append(diff)
			prev = int(new_line[8])
			line_no = line_no+1
			temp_line = prev_line.rstrip('\n')
			temp_line = temp_line+" "+ str(comp_time)+ '\n'
			#final = '\n'.join(temp_line)
		line_no +=1
		trace_count +=1
		prev_line = line
		temp_f=open(filename, 'a')
		temp_f.write(temp_line)
		if(trace_count > 20000):
			break

#plt.plot(my_list)
#plt.ylabel('app_execution_times')
#plt.show()
