CC = g++
DEBUG = y 

vpath %.h inc
vpath %.cpp c:c/threads:c/can:c/ecat
vpath %.o obj

CPPFLAGS = -O2 -fPIC -Wall -pthread -W -Wpointer-arith -pedantic -Wno-unused-parameter -I inc -I inc/can -I inc/ecat

ifdef DEBUG
	CPPFLAGS += -DDEBUG -ggdb3 -O0
endif

SOURCE += Amp.cpp AmpFile.cpp AmpFW.cpp AmpParam.cpp AmpPDO.cpp AmpPVT.cpp AmpStruct.cpp AmpUnits.cpp \
	  AmpVersion.cpp Can.cpp CanOpen.cpp CML.cpp CopleyIO.cpp CopleyIOFile.cpp CopleyNode.cpp \
	  Error.cpp EtherCAT.cpp ecatdc.cpp EventMap.cpp File.cpp Filter.cpp Firmware.cpp Geometry.cpp IOmodule.cpp \
	  Linkage.cpp Network.cpp Node.cpp Path.cpp PDO.cpp Reference.cpp SDO.cpp Threads.cpp TrjScurve.cpp \
	  Utils.cpp InputShaper.cpp LSS.cpp

SOURCE += can_copley.cpp 

SOURCE += ecat_linux.cpp

SOURCE += Threads_posix.cpp 

obj/%.o : %.cpp
	@echo ........... Compiling $<
	@mkdir -p obj
	@$(CC) -c -o $@ $< $(CPPFLAGS)

libMotionLib.a :  $(patsubst %.cpp,obj/%.o, $(SOURCE) )
	@echo ........... Linking $@
	@ar -r $@ $^

libMotionLib.so : $(patsubst %.cpp,obj/%.o, $(SOURCE) )
	@echo ........... Linking $@
	@gcc -o $@ -shared -lpthread -lrt -ldl $^

clean :
	rm -f libMotionLib.so
	rm -f obj/*.o
	rm -f dep/*.d
	rm -f CML.zip
	rm -f -r manual

manual: manual/latex/refman.pdf

manual/latex/refman.pdf: libMotionLib.a
	doxygen
	sed -i 's/latex_count=8/latex_count=20/g' manual/latex/Makefile
	(cd manual/latex; make)
	cp doc/CCC_logo.gif manual/html

package: manual cmldir
	makecl
	mv ChangeLog CML/ChangeLog.txt
	cp manual/latex/refman.pdf CML
	cp -R manual/html CML/html
	zip -r CML.zip CML
	rm -r CML

codepack: cmldir
	zip -r CML.zip CML
	rm -r CML

cmldir: libMotionLib.a
	rm -f CML.zip
	mkdir -p CML/c/can
	mkdir -p CML/c/ecat
	mkdir -p CML/c/threads
	mkdir -p CML/inc/can
	mkdir -p CML/inc/ecat
	mkdir -p CML/examples/move
	mkdir -p CML/examples/pdotest
	mkdir -p CML/examples/twoaxis
	mkdir -p CML/examples/gantry
	mkdir -p CML/examples/iofile
	cp c/*.cpp CML/c
	cp c/can/*.cpp CML/c/can
	cp c/ecat/*.cpp CML/c/ecat
	cp c/threads/*.cpp CML/c/threads
	cp Makefile CML/
	cp inc/*.h CML/inc
	cp inc/can/*.h CML/inc/can
	cp inc/ecat/*.h CML/inc/ecat
	cp doc/readme.txt CML
	cp doc/ecatlicense.txt CML
	cp examples/pdotest/*.cpp CML/examples/pdotest
	cp examples/pdotest/*.dsp CML/examples/pdotest
	cp examples/pdotest/*.txt CML/examples/pdotest
	cp examples/pdotest/Makefile CML/examples/pdotest
	cp examples/move/*.cpp CML/examples/move
	cp examples/move/*.dsp CML/examples/move
	cp examples/move/*.txt CML/examples/move
	cp examples/move/Makefile CML/examples/move
	cp examples/twoaxis/*.cpp CML/examples/twoaxis
	cp examples/twoaxis/*.dsp CML/examples/twoaxis
	cp examples/twoaxis/*.txt CML/examples/twoaxis
	cp examples/twoaxis/Makefile CML/examples/twoaxis
	cp examples/gantry/*.cpp CML/examples/gantry
	cp examples/gantry/*.dsp CML/examples/gantry
	cp examples/gantry/*.txt CML/examples/gantry
	cp examples/gantry/Makefile CML/examples/gantry
	cp examples/iofile/*.cpp CML/examples/iofile
	cp examples/iofile/*.dsp CML/examples/iofile
	cp examples/iofile/*.txt CML/examples/iofile
	cp examples/iofile/*.cci CML/examples/iofile
	cp examples/iofile/Makefile CML/examples/iofile
	rm CML/c/can/can_ni.cpp
	rm CML/inc/can/can_ni.h
	fixcr -d CML/*/*.cpp CML/*/*/*.cpp CML/*/*.h CML/*/*/*.h CML/*.txt CML/examples/*/*.txt

dep/%.d : %.cpp
	@echo ........... Building $@
	@mkdir -p dep
	@gcc -MM $(CPPFLAGS) -MT $(patsubst c/%.cpp,obj/%.o,$<) $< > $@
	@gcc -MM $(CPPFLAGS) -MT $(patsubst c/%.cpp,dep/%.d,$<) $< >> $@

include $(patsubst %.cpp,dep/%.d,$(SOURCE))

