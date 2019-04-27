#!/usr/bin/python
import os
import subprocess 
import itertools

address_mins = ["2163483648", "2166358016", "2147745792", "2147745792", "2147483648", "2147483648", "2147578624", "2147483648", "2248146944", "2147746048" ]
address_maxs = ["2211483648", "2185232384", "2148532224", "2147745892", "2181038080", "2214592512", "2148364032", "2147745792", "2248540160", "2148270336"]
benches = ["blackscholes", "convtex", "dct", "ev", "fwt", "hist", "nn", "dxtc", "mergesort", "ray"]
archs = ["bin", "bin_app_base", "bin_app_noc", "bin_new_gpu","bin_dap_jour", "bin_dap_new"]
cmd = []

for arch in archs:
	# iterate thru benches
	for (bench, addr_min, addr_max) in zip(benches, address_mins, address_maxs):
		cmd.extend([os.path.join(arch,"noxim"),"-trf","trace_"+bench+".db"])
	# gen address ranges for each bench
		cmd.extend([ "-addr_min", addr_min, "-addr_max", addr_max, "-sim", str(1000000)])
		print cmd
	# exec cmd
		out_file = arch+"_"+bench+"_out_64.log"
		with open(out_file, "w")as outfile:
			subprocess.call(cmd, stdout=outfile)
	# save output in file
		del cmd[:]
