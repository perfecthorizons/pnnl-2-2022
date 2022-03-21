ifneq ($(KERNELRELEASE),)
obj-m	:= copleycan.o
copleycan-objs := src/main.o src/cardintf.o src/send.o src/recv.o src/test.o src/utils.o \
	      src/fileinfo.o src/MsgQueue.o src/command.o src/bdm.o src/xmitbuff.o \
	      src/usb.o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement
else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
endif


clean :
	rm -f *.o
	rm -f *.ko
	rm -f .*.cmd
	rm -f *mod.c
	rm -f src/*.o
	rm -f src/.*.cmd
	rm -rf .tmp_versions
	(cd inc; rm -f *.ver)
	
install: copleycan.ko
	sudo mkdir -p /lib/modules/$(shell uname -r)/misc
	sudo cp copleycan.ko /lib/modules/$(shell uname -r)/misc
	sudo depmod
	sudo modprobe copleycan
