from subprocess import call
import os
import linecache
import sys

total_dyn_pwr = 0
total_stat_pwr = 0
# read a line from slice files and update the below line
filename=(str(sys.argv[1]))
with open(filename) as f:
	for line in f:
		# change the router_hf.cfg file
		cmnd = "python configs/edit_config.py "+line
		os.system(cmnd)
		os.system("cat configs/router_hf.cfg | sed '19!d'")
		# then call the power computation program. 
		os.system("./dsent -cfg configs/router_hf.cfg > test.log")
		temp =linecache.getline('test.log', 40)
		temp2 = linecache.getline('test.log', 41)
		dyn_pwr = temp.split()
		#print dyn_pwr
		stat_pwr = temp2.split()
		total_dyn_pwr = total_dyn_pwr + float(dyn_pwr[2])
		total_stat_pwr = total_stat_pwr + float(stat_pwr[2])
		os.system("rm -rf test.log")
		linecache.clearcache()
		
power_out_file = open("power_out.txt", 'a')		
# Get the Dynamic and static power from the output text 
output =  "total dynamic power of "+ filename +" = "+ str(total_dyn_pwr)+"\n"
power_out_file.write(output)
output ="total static power of "+ filename +" = "+ str(total_stat_pwr)+"\n"
power_out_file.write(output)
# Add the powers and print the final power of each slice. 



