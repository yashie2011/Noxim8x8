import matplotlib.pyplot as plt

prev = 0
my_list = list()
with open("node[01].txt") as f:
	for line in f:
		new_line=line.split()
		print new_line[8]
		diff = int(new_line[8]) - (prev)
		if (diff > 100):
			diff = diff-120
		my_list.append(diff)
		prev = int(new_line[8])

plt.plot(my_list)
plt.ylabel('app_execution_times')
plt.show()
		
