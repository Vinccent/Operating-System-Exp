source := plypy
cdevname := plypy_chrdev
# major := $(shell awk '$$2 == "plypy_chrdev" {print $$1}' /proc/devices)
# major := $(shell awk -v mod="plypy_chrdev" '$$2==mod{print $$1}' /proc/devices)
major := `cat /proc/devices | awk '$$2 == "plypy_chrdev" {print $$1}'`

ifneq ($(KERNELRELEASE),)
	obj-m:=$(source).o
else
	KERNELDIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)
endif
build:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install:
	insmod $(source).ko
	mknod /dev/$(cdevname) c $(major) 0

remove:
	rmmod $(source)
	rm /dev/$(cdevname)

clean:
	rm modules.order Module.symvers *.ko *.o

