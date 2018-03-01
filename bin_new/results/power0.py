from subprocess import call
import os
import linecache

total_dyn_pwr = 0
total_stat_pwr = 0
filename=(str(sys.argv[1]))
# read a line from slice files and update the below line
with open(filename) as f:
	for line in f:
		# change the router.cfg file
		cmnd = "python configs/edit_config0.py "+line
		os.system(cmnd)
		os.system("cat configs/router.cfg | sed '19!d'")
		# then call the power computation program. 
		os.system("./dsent -cfg configs/router.cfg > test.log")
		temp =linecache.getline('test.log', 44)
		temp2 = linecache.getline('test.log', 45)
		dyn_pwr = temp.split()
		print dyn_pwr
		stat_pwr = temp2.split()
		total_dyn_pwr = total_dyn_pwr + float(dyn_pwr[2])
		total_stat_pwr = total_stat_pwr + float(stat_pwr[2])
		os.system("rm -rf test.log")
		linecache.clearcache()
		
# Get the Dynamic and static power from the output text 
print "total dynamic power of this slice = "+ str(total_dyn_pwr)
print "total static power of this slice = "+ str(total_stat_pwr)
# Add the powers and print the final power of each slice. 



