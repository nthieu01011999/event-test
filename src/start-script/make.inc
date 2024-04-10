_DIRNAME   = $(notdir $(shell pwd))
BUILD_OUT ?= $(shell pwd)/out

all:clean install

clean:
	rm -rf  $(ROMFS_DIR)/init
install:

romfs:
	mkdir -p  $(ROMFS_DIR)/init
	cp -a start_agent.sh $(ROMFS_DIR)/init/
	chmod +x $(ROMFS_DIR)/init/start_agent.sh




