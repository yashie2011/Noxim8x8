#!/bin/sh

cmd="./noxim -pir 0.01 poisson -sim 100000 -warmup 100 -size 8 8 -buffer 12 -pwr default_router.pwr -lpls -qos 0.5 -routing xy -dimx 8 -dimy 8 -traffic benchmark"

extension=.log.2
logname="$1$extension"
tracepath=/media/yaswanth/nasstore/gpu_traces/
tracedir="$tracepath$1"
echo $tracedir
echo $tracepath

cd bin
rm node\[*
echo "current dir $PWD"
find $tracedir -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD"
$cmd > $logname


cd ../bin_XYYX
rm node\[*
echo "current dir $PWD"
find ../bin -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD" 
rm ../bin/node\[*
$cmd > $logname


cd ../bin_new
rm node\[*
echo "current dir $PWD"
find ../bin_XYYX -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD"
rm ../bin_XYYX/node\[*
$cmd > $logname

cd ../bin_new_journal
rm node\[*
echo "current dir $PWD"
find ../bin_new -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD"
rm ../bin_new/node\[*
$cmd > $logname

cd ../bin_DA2
rm node\[*
echo "current dir $PWD"
find ../bin_new_journal -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD"
rm ../bin_new_journal/node\[*
$cmd > $logname

cd ../bin_cfnoc
rm node\[*
echo "current dir $PWD"
find ../bin_DA2 -name "node\[*" -exec "cp" "{}" . ";"
echo "copied traces to $PWD"
rm ../bin_DA2/node\[*
$cmd > $logname
rm node\[*

echo "Done."

cd ..
exit 0
