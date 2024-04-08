CXXFLAGS	+= -I./sources/app

VPATH += sources/app

OBJ += $(OBJ_DIR)/app.o
OBJ += $(OBJ_DIR)/task_list.o
OBJ += $(OBJ_DIR)/task_webrtc.o
