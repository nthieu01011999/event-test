

HAL_VERSION=2024.03.22

CXXFLAGS += -Isources/libraries/libvvtk/$(HAL_VERSION)/include

LDLIBS += -Lsources/libraries/libvvtk/$(HAL_VERSION)/lib
