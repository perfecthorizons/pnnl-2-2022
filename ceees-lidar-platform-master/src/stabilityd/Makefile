CPP=g++-8.1.0
INCLUDES=-I../CML/inc -I../vnproglib-1.1-cpp -I../spdlog-1.x/include
CPPFLAGS=$(INCLUDES) -std=c++2a -Wall -Wpedantic

LD=g++-8.1.0
LDFLAGS=-L../CML/ -lMotionLib -L../vnproglib-1.1-cpp/build/bin/ -lvncxx

stablityd: daemonize.o stabilityd.o
	$(LD) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	find . -name "*.o" -delete

