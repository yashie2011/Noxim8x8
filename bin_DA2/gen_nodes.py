
with open("fastWalshTransform_trace.txt") as f:
	for line in f:
		new_line=line.split()
		if new_line[1] < 10:
			temp_filename = 'node[\0'+new_line[1]+'].txt'
		else:
			temp_filename = 'node['+new_line[1]+'].txt'
		temp_f=open(temp_filename, 'a')
		temp_f.write(line)
