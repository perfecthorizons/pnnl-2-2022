ifeq ($(MAKELEVEL),0)
	export CPP=g++
	export CPPFLAGS=-std=c++1z -ggdb -Wall -Wpedantic
	export LD=g++
	export LDFLAGS=

	export INCLUDES=-I$(CURDIR)/../../ -I$(CURDIR)/../../../../lib-src/vnproglib-1.1/cpp/include -I$(CURDIR)/../../../../lib-src/CML/inc
endif

CPPFLAGS+= -fPIC -shared -nostartfiles

SOURCES=$(wildcard *.cpp)
TARGETS=$(patsubst %.cpp,%.so,$(SOURCES))

all: $(TARGETS)

%.so: %.cpp
	$(CPP) $(CPPFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f *.so *.o

