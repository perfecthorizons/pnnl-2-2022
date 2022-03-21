export CPP=g++
export CPPFLAGS=-std=c++1z -ggdb -Wall -Wpedantic
export LD=g++
export LDFLAGS=-L$(CURDIR)/../../../lib-src -ldl -lpthread -lvncxx -lMotionLib

INCLUDES=   -I$(CURDIR)/../
INCLUDES += -I$(CURDIR)/../../../lib-src/vnproglib-1.1/cpp/include
INCLUDES += -I$(CURDIR)/../../../lib-src/CML/inc

all: vectornav_test

vectornav_test: vectornav.o vectornav_test.o
	$(LD) $^ $(LDFLAGS)

%.o: %.cpp
	@echo "COMPILING $^"
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $^ -o $@

