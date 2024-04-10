_DIRNAME   = $(notdir $(shell pwd))
BUILD_OUT ?= $(shell pwd)/out

# Compiler

CFLAGS =  -Wall 
CFLAGS += -I./
CFLAGS += -I$(PRJ_ROOT)/middleware/inc
CFLAGS += -I$(OUTPUT)/external/include


LDFLAGS  = -ldl -Wl,-L$(OUTPUT)/external/lib 
LDFLAGS += -lpthread -lcunit -L$(PRJ_ROOT)/middleware/lib -lvvtkhal

# Project name
PROJECT = test-vvtkhal

###########################################################

# Directories
OBJDIR = $(BUILD_OUT)
SRCDIR = $(shell pwd)


# Files and folders
SRCS    = $(shell find $(SRCDIR) -name '*.c' ! -name "._*")
OBJS    = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

all:$(OBJDIR)/$(PROJECT) install

# Targets
$(OBJDIR)/$(PROJECT): buildrepo $(OBJS) 
	$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -rf $(OBJDIR)
	
buildrepo:
	@$(call make-repo)

install:

romfs:
	$(STRIP) $(OBJDIR)/$(PROJECT)
	cp $(OBJDIR)/$(PROJECT) $(ROMFS_DIR)/bin/

# Create obj directory structure
define make-repo
	mkdir -p $(OBJDIR) $(OBJDIR)
endef



