
prev_line = 0
total_time = 0
line_no = 0
for i in range(0,11):
	file_name = 'node['+str(i)+'].txt'
	with open(file_name) as f:
		for line in f:
			new_line=line.split()
			total_time += float(new_line[9])
			line_no += 1
print "no. of same time stamps "+ str(total_time/line_no)
