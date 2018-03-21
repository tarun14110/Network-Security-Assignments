CONFIG_MODULE_SIG=n
CONFIG_MODULE_SIG_ALL=n
obj-m := hello.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
