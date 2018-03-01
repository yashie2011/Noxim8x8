TARGET_ARCH = linux64
# TARGET_ARCH = macosx64

CC     = g++
OPT    = -O3
DEBUG  = -g
OTHER  = # -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER) $(DEBUG) -std=gnu++11

VPATH = ../src_cfnoc:.
MODULE = noxim
SRCS = ./token_issuer.cpp ./NoximNoC.cpp ./NoximRouter.cpp ./benchmark.cpp ./NoximProcessingElement.cpp ./NoximBuffer.cpp \
	./NoximStats.cpp ./NoximGlobalStats.cpp ./NoximGlobalRoutingTable.cpp \
	./NoximLocalRoutingTable.cpp ./NoximGlobalTrafficTable.cpp ./NoximReservationTable.cpp \
	./NoximPower.cpp ./NoximCmdLineParser.cpp ./NoximMain.cpp 
OBJS = $(SRCS:.cpp=.o)

include ./Makefile.defs
