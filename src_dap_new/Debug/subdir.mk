################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../._NoximBuffer.cpp \
../._NoximGlobalRoutingTable.cpp \
../._NoximGlobalStats.cpp \
../._NoximLocalRoutingTable.cpp \
../._NoximMain.cpp \
../._NoximNoC.cpp \
../._NoximPower.cpp \
../._NoximProcessingElement.cpp \
../._NoximReservationTable.cpp \
../._NoximRouter.cpp \
../._NoximStats.cpp \
../NoximBuffer.cpp \
../NoximCmdLineParser.cpp \
../NoximGlobalRoutingTable.cpp \
../NoximGlobalStats.cpp \
../NoximGlobalTrafficTable.cpp \
../NoximLocalRoutingTable.cpp \
../NoximMain.cpp \
../NoximNoC.cpp \
../NoximPower.cpp \
../NoximProcessingElement.cpp \
../NoximReservationTable.cpp \
../NoximRouter.cpp \
../NoximStats.cpp \
../benchmark.cpp \
../fc_router.cpp \
../global_vars.cpp 

OBJS += \
./._NoximBuffer.o \
./._NoximGlobalRoutingTable.o \
./._NoximGlobalStats.o \
./._NoximLocalRoutingTable.o \
./._NoximMain.o \
./._NoximNoC.o \
./._NoximPower.o \
./._NoximProcessingElement.o \
./._NoximReservationTable.o \
./._NoximRouter.o \
./._NoximStats.o \
./NoximBuffer.o \
./NoximCmdLineParser.o \
./NoximGlobalRoutingTable.o \
./NoximGlobalStats.o \
./NoximGlobalTrafficTable.o \
./NoximLocalRoutingTable.o \
./NoximMain.o \
./NoximNoC.o \
./NoximPower.o \
./NoximProcessingElement.o \
./NoximReservationTable.o \
./NoximRouter.o \
./NoximStats.o \
./benchmark.o \
./fc_router.o \
./global_vars.o 

CPP_DEPS += \
./._NoximBuffer.d \
./._NoximGlobalRoutingTable.d \
./._NoximGlobalStats.d \
./._NoximLocalRoutingTable.d \
./._NoximMain.d \
./._NoximNoC.d \
./._NoximPower.d \
./._NoximProcessingElement.d \
./._NoximReservationTable.d \
./._NoximRouter.d \
./._NoximStats.d \
./NoximBuffer.d \
./NoximCmdLineParser.d \
./NoximGlobalRoutingTable.d \
./NoximGlobalStats.d \
./NoximGlobalTrafficTable.d \
./NoximLocalRoutingTable.d \
./NoximMain.d \
./NoximNoC.d \
./NoximPower.d \
./NoximProcessingElement.d \
./NoximReservationTable.d \
./NoximRouter.d \
./NoximStats.d \
./benchmark.d \
./fc_router.d \
./global_vars.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/systemc-2.3.1/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


