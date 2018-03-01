#!/bin/sh

cmd="./noxim -pir 0.01 poisson -sim 100000 -warmup 100 -size 8 8 -buffer 12 -pwr default_router.pwr -lpls -qos 0.5 -routing xy -dimx 12 -dimy 12 -traffic benchmark"

cd bin_XYYX
echo "current dir $PWD"
find /media/yaswanth/nasstore/gpu_traces/STO12 -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD"
$cmd > STO12.log


cd ../bin_cfnoc
echo "current dir $PWD"
find ../bin_XYYX -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD" 
rm ../bin_XYYX/node\[*
$cmd > STO12.log
rm node\[*

echo "Done."

cd ..
exit 0
