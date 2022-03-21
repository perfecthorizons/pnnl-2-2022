export CPP=g++
export CPPFLAGS=-std=c++1z -g -Wall -O2 -DPRODUCTION 
export LD=g++
export LDFLAGS=-L$(CURDIR)/../../lib-src/CML -ldl -lpthread -lMotionLib -lwiringPi -lrt -lm -lcurses $(XSFLAGS)

OBJLIBS= xspublic
XSDIR=$(CURDIR)/../../lib-src/xspublic
XSFLAGS=-L$(XSDIR)/xscontroller -L$(XSDIR)/xscommon -L$(XSDIR)/xstypes -lxscontroller -lxscommon -lxstypes

INCLUDES=   -I$(CURDIR)/
# INCLUDES += -I$(CURDIR)/../../lib-src/vnproglib-1.1/cpp/include # Remove this later
INCLUDES += -I$(CURDIR)/../../lib-src/CML/inc
INCLUDES += -I$(XSDIR)

all: main main_autorun #$(OBJLIBS) tests

main: main.o md5.o dynamic_functions.o alerts.o motorcontroller.o alerts.o pid_file.o xsens.o
	@echo "MAKING MAIN..."
	$(LD) $^ $(LDFLAGS) -o $@

main_autorun: main_autorun.o md5.o dynamic_functions.o alerts.o motorcontroller.o alerts.o pid_file.o xsens.o
	@echo "MAKING MAIN_AUTORUN..."
	$(LD) $^ $(LDFLAGS) -o $@

tests: test_datalogger
	@echo "MAKING TESTS..."

test_datalogger: test_datalogger.o
	$(LD) $^ -lpthread -o $@

xspublic:
	$(MAKE) -C $(XSDIR) $(MFLAGS)

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
#	cd dyn_functions && $(MAKE) clean
