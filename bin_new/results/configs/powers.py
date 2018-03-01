from subprocess import call
import os


# read a line from slice files and update the below line
with open("slice_1.log") as f:
	for line in f:
		# change the router.cfg file
		cmnd = "python edit_config.py "+line
		os.system(cmnd)
		os.system("cat router.cfg | sed '19!d'")
		# then call the power computation program. 
		call(["./dsent", "-cfg" , "router.cfg"])

# Get the Dynamic and static power from the output text 


# Add the powers and print the final power of each slice. 


#print line
#new_line = line.split()
#print len(new_line)
#print new_line[2]



#with open("router.cfg") as f:
	#for line in f:
		#new_line=line.split()
		#print new_line[3]
