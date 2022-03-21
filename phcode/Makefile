export CPP=g++
export CPPFLAGS=-std=c++11 -g -Wall -DPRODUCTION #-O2 
export LD=g++
export LDFLAGS=-L$(LIBSOURCE)/CML -ldl -lpthread -lMotionLib -lwiringPi -lrt -lm -lcurses $(XSFLAGS)

OBJLIBS= xspublic CML
XSDIR=$(LIBSOURCE)/xspublic
XSFLAGS=-L$(XSDIR)/xscontroller -L$(XSDIR)/xscommon -L$(XSDIR)/xstypes -lxscontroller -lxscommon -lxstypes

CMLDIR=$(LIBSOURCE)/CML

INCLUDES=   -I$(INCLUDE)
INCLUDES += -I$(LIBSOURCE)/CML/inc
INCLUDES += -I$(XSDIR)

INCLUDE=	inc
SOURCE= 	src
BIN=		bin
LIBSOURCE= lib-src

TARGETS= $(BIN)/main $(BIN)/main_autorun

all: $(OBJLIBS) $(TARGETS)#tests

$(BIN)/main: $(SOURCE)/motorcontroller.o $(SOURCE)/alerts.o $(SOURCE)/xsens.o $(SOURCE)/main.o
	@echo "LINKING MAIN..."
	$(LD) $^ $(LDFLAGS) -o $@

$(BIN)/main_autorun: $(SOURCE)/motorcontroller.o $(SOURCE)/alerts.o $(SOURCE)/xsens.o $(SOURCE)/main_autorun.o
	@echo "LINKING MAIN_AUTORUN..."
	$(LD) $^ $(LDFLAGS) -o $@

tests: test_datalogger
	@echo "MAKING TESTS..."

test_datalogger: test_datalogger.o
	$(LD) $^ -lpthread -o $@

xspublic:
	@echo "MAKING XSENS LIB"
	@$(MAKE) -C $(XSDIR) $(MFLAGS)

CML:
	@echo "MAKING COPLEY LIB"
	@$(MAKE) -C $(CMLDIR) $(MFLAGS)

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
	-rm -f src/*.o $(TARGETS)
#	cd dyn_functions && $(MAKE) clean
