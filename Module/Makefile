source := mymodules
ifneq ($(KERNELRELEASE),)
	obj-m:=$(source).o
else
	KERNELDIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)
endif
build:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
