% fname: qos_0_5__routing_xy__topology_4x4__.m
% ../bin/noxim -qos 0.5 -routing xy -dimx 4 -dimy 4  -sim 10000 -warmup 2000 -size 8 8 -buffer 4 -pwr ../pwr/default_router.pwr -lpls 

function [max_pir, max_throughput, min_delay] = qos_0_5__routing_xy__topology_4x4__(symbol)

data = [
%             pir      avg_delay     throughput      max_delay   total_energy       rpackets         rflits
             0.01        24.4898      0.0948672            165    2.42595e-06           1278          12143
             0.01        24.4898      0.0948672            165    2.42595e-06           1278          12143
             0.01        24.4898      0.0948672            165    2.42595e-06           1278          12143
