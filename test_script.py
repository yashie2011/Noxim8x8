#!/usr/bin/python

import os
import subprocess


benchmarks = ['MUM8']
#benchmarks = ['BFS8', 'dct8x8', 'LIB8','monte8']
#works = ['bin','bin_new_journal','bin_XYYX', 'bin_new', 'bin_DA2', 'bin_cfnoc']

for bench in benchmarks:
	subprocess.call(['./test_script_faster_2.sh', bench])
	
