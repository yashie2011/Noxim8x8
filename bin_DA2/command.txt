./noxim -pir 0.01 poisson  -sim 100000 -warmup 2000 -size 8 8 -buffer 12 -pwr ../pwr/default_router.pwr -lpls  -qos 0.5 -routing xy -dimx 4 -dimy 4 -traffic benchmark > test.log
