



ifdef BUILD_ARM_VVTK
-include sources/libraries/arm_rts/Makefile.mk
-include sources/libraries/libvvtk/Makefile.mk
-include sources/libraries/mp4v2/Makefile.mk
ifdef FEATURE_AI
-include sources/libraries/librealtex/Makefile.mk
endif
endif

ifdef BUILD_LINUX64
-include sources/libraries/linux64/Makefile.mk
endif
