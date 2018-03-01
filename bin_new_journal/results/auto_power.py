from subprocess import call
import os
import linecache
import sys

filename = "BFS/slice_1.txt"
cmd = "python powers.py "+filename+"> power_out.txt"
os.system(cmd)

