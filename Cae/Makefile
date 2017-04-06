source := chatModule
cdevname := caesium_chrdev
major := `cat /proc/devices | awk '$$2 == "$(cdevname)" {print $$1}'`

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
	rm modules.order Module.symvers chatModule.mod.* *.ko *.o
