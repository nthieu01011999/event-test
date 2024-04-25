CXXFLAGS	+= -I./sources/app/webrtc

VPATH += sources/app/webrtc

# OBJ += $(OBJ_DIR)/stream.o
OBJ += $(OBJ_DIR)/helpers.o
OBJ += $(OBJ_DIR)/datachannel_hdl.o
# OBJ += $(OBJ_DIR)/h26xsource.o
# OBJ += $(OBJ_DIR)/audiosource.o
# OBJ += $(OBJ_DIR)/dispatchqueue.o
# OBJ += $(OBJ_DIR)/streamsource.o
