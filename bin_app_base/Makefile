TARGET_ARCH = linux64
# TARGET_ARCH = macosx64

CC     = g++
OPT    = -O3
DEBUG  = -g
OTHER  = -std=gnu++11 # -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)  $(DEBUG)

VPATH = ../src_app_base:.
MODULE = noxim
SRCS = ./db_node.cpp ./NoximNoC.cpp ./NoximRouter.cpp ./NoximProcessingElement.cpp ./NoximBuffer.cpp \
	./NoximStats.cpp ./NoximGlobalStats.cpp ./NoximGlobalRoutingTable.cpp \
	./NoximLocalRoutingTable.cpp ./NoximGlobalTrafficTable.cpp ./NoximReservationTable.cpp \
	./NoximPower.cpp ./NoximCmdLineParser.cpp ./NoximMain.cpp 
OBJS = $(SRCS:.cpp=.o)

include ./Makefile.defs
