# The name of the kernel module
obj-m += tm.o

# Path to the kernel's build directory (use the kernel version automatically)
KDIR := /lib/modules/$(shell uname -r)/build

# Path to the current directory (where the Makefile is located)
PWD := $(shell pwd)

# Default target - builds the kernel module
all:
	make -C $(KDIR) M=$(PWD) modules

# Clean target - removes object files and other build artifacts
clean:
	make -C $(KDIR) M=$(PWD) clean

