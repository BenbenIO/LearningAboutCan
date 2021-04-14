################
# Project: CANDISCOVERY
# Author: Benjamin
# Date: 2021/04
################

# Directory
IDIR = include
ODIR = obj

# Compilation rules
CC = gcc
CX = g++
CCFLAGS = -O2 -O3 -Wall $(INCLUDES) -std=c++14 -g
LNKCC = $(CX)
LNKFLAGS = $(CCFLAGS)

# Core components
INCLUDES += -I$(IDIR)
LIBRARIES += -lboost_program_options

# Files
SOURCES_send = src/send.cpp
TARGET_send = CAN_send.exe
SOURCES_read = src/read.cpp
TARGET_read = CAN_read.exe
SOURCES_ssc = src/SimpleSocketCan.cpp src/DBSparser.cpp
SOURCES_testdbs = test/test_dbs.cpp
TARGET_testdbs = test_dbs.exe

# Compilation rule
all: send read

dbs_test:
	$(LNKCC) $(LNKFLAGS) $(SOURCES_testdbs) $(SOURCES_ssc) $(LIBRARIES) -o $(TARGET_testdbs)

send:
	$(LNKCC) $(LNKFLAGS) $(SOURCES_send) $(SOURCES_ssc) $(LIBRARIES) -o $(TARGET_send)

read:
	$(LNKCC) $(LNKFLAGS) $(SOURCES_read) $(SOURCES_ssc) $(LIBRARIES) -o $(TARGET_read)

# Other option
.PHONY: clean

clean:
	rm -rf *.exe *~ *.a *.so *.lo
