from subprocess import call
import os
import linecache
import sys

filename = "BFS/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "CONV/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "CP/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "EV/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "FWT/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "LIB/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "NN/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "NQU/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "RAY/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "STO/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

filename = "TRANS/slice_s.log"
cmd = "python power0.py "+filename
os.system(cmd)

