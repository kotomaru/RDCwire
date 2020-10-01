TARGET = RDCwire
TARGET2 = RDCwirefin

SRCS = $(TARGET).cpp
OBJS = $(TARGET).o
SRCS2 = $(TARGET2).cpp
OBJS2 = $(TARGET2).o

ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)
ROOTGLIBS = $(shell root-config --glibs)

CXXFLAGS   = $(ROOTCFLAGS) -Wall -fPIC
CXXLIBS    = $(ROOTLIBS)
CC = g++

$(TARGET):
	$(CC) $(SRCS) $(CXXLIBS) $(CXXFLAGS) -o $@

$(TARGET2):
	$(CC) $(SRCS2) $(CXXLIBS) $(CXXFLAGS) -o $@

clean:
	rm -f $(TARGET) $(OBJS) $(TARGET2) $(OBJS2)
