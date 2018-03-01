
prev_line = 0
line_no = 0
same_ts = 0
for i in range(0,11):
	file_name = 'node['+str(i)+'].txt'
	with open(file_name) as f:
		for line in f:
			new_line=line.split()
			if (line_no > 0):
				old_line=prev_line.split()
				if(old_line[8] == new_line[8]):
					same_ts += 1
					print "shit happens at " + str(old_line[8]) 
			line_no += 1
			prev_line = line
print "no. of same time stamps "+ str(same_ts)
