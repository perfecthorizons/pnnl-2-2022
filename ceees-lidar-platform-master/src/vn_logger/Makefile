export CPP=g++
export CPPFLAGS=-std=c++1z -ggdb -Wall -O2 -Wpedantic -DPRODUCTION
export LD=g++
export LDFLAGS=-L$(CURDIR)/../../lib -ldl -lpthread -lvncxx -lMotionLib -lrt -lm

INCLUDES=   -I$(CURDIR)/
INCLUDES += -I$(CURDIR)/../../lib-src/vnproglib-1.1/cpp/include
INCLUDES += -I$(CURDIR)/../../lib-src/CML/inc

main: main.o md5.o dynamic_functions.o vectornav.o alerts.o motorcontroller.o alerts.o pid_file.o
	@echo "MAKING MAIN..."
	$(LD) $^ $(LDFLAGS) -o $@

all: main tests

tests: test_datalogger
	@echo "MAKING TESTS..."

test_datalogger: test_datalogger.o
	$(LD) $^ -lpthread -o $@

%.o: %.cpp
	@echo "COMPILING $^"
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $^ -o $@

libvncxx.a:
	@echo bang!

libMotion.a:
	@echo bang!

dynamic_functions_source:
	cd dyn_functions && $(MAKE)

.PHONY : clean
clean:
	-rm -f *.o main
	cd dyn_functions && $(MAKE) clean
