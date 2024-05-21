# Set the source code directory relative to the Makefile's location
SRCCODE := $(CURDIR)/..

# Set the cross-compiler prefix
# Set the cross-compiler prefix
CROSS_COMPILE := /home/it/Downloads/event-test/sourcebase/cross-compile/rts39xx_sdk_v5.0.1/toolchain/asdk-10.3.1-a7-EL-5.4-u1.0-a32nh-220218/bin/arm-linux-

# Define compiler and linker binaries
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld

# Include additional build configurations
include $(SRCCODE)/.vbuild/build.mk

# Define targets for building, cleaning, installing, and ROMFS handling
obj-clean := $(foreach obj,$(obj-Y) $(dir),$(obj)-clean)
obj-install := $(foreach obj,$(obj-Y),$(obj)-install)
obj-romfs := $(foreach obj,$(obj-Y),$(obj)-romfs)

all:
	@echo "Building with $(CC)"
	$(CC) -o myprogram $(SRCCODE)/test-hello/main.c

clean: $(obj-clean)
	rm -f myprogram

install: $(obj-install)

romfs: $(obj-romfs)

# Generic rules for directories
%:
	[ ! -d $* ] || $(MAKE) -f make.inc -C $*

%-clean:
	[ ! -d $* ] || $(MAKE) -f make.inc -C $* clean

%-install:
	[ ! -d $* ] || $(MAKE) -f make.inc -C $* install

%-romfs:
	[ ! -d $* ] || $(MAKE) -f make.inc -C $* romfs

$(obj-Y) $(obj-N) $(obj-clean) $(obj-install): dummy

.PHONY: all clean install romfs
dummy:
	@echo "This is a dummy target"
