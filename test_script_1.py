#!/usr/bin/python

import os
import subprocess


#benchmarks = ['convtex8', 'dct8x8', 'fwt8', 'LIB8', 'monte8', 'MUM8', 'NN8', 'NQU8', 'RAY8', 'STO8' ]
benchmarks = ['BFS8']
works = ['bin_cfnoc']
d = dict(os.environ)
d["SC_SIGNAL_WRITE_CHECK"] = 'DISABLE'
NUM_CORES = 56

for bench in benchmarks:
	for w in works:
		os.chdir(w)
		#copy the traces to the binary directory of noxim works
		for i in range(NUM_CORES):
			copy_path = '/media/yaswanth/nasstore/gpu_traces/%s/node[%d].txt' % (bench, i)
			subprocess.call(['cp', copy_path, '.'])
		#execute the noxim command
		noxim_cmd = './noxim -pir 0.01 poisson -sim 100000 -warmup 100 -size 8 8 -buffer 12 -pwr default_router.pwr -lpls -qos 0.5 -routing xy -dimx 8 -dimy 8 -traffic benchmark'
		cmd = noxim_cmd.split()
		with open('test.log', 'w') as outfile:
			subprocess.call(noxim_cmd, shell=True, env = d, stdout = outfile)
		# get the tail of the output file and save it in the results.log
		with open(bench+'.log', 'w')as of:
			subprocess.call(['tail', '-100', 'test.log'], stdout = of)
		os.chdir('..')
	
	
