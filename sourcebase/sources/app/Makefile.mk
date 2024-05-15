-include sources/app/json/Makefile.mk
-include sources/app/webrtc/Makefile.mk
-include sources/app/av/Makefile.mk

CXXFLAGS	+= -I./sources/app



VPATH += sources/app

OBJ += $(OBJ_DIR)/app.o
OBJ += $(OBJ_DIR)/app_data.o
OBJ += $(OBJ_DIR)/task_list.o
OBJ += $(OBJ_DIR)/task_webrtc.o
OBJ += $(OBJ_DIR)/task_hello.o
OBJ += $(OBJ_DIR)/task_av.o
OBJ += $(OBJ_DIR)/app_config.o
OBJ += $(OBJ_DIR)/mtce_parameter.o
