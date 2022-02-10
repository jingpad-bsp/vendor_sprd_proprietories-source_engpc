LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := PQTune.$(TARGET_BOARD_PLATFORM)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_RELATIVE_PATH := npidevice
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(TOP)/vendor/sprd/proprietories-source/engpc/sprd_fts_inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/PQParsers
LOCAL_C_INCLUDES += $(LOCAL_PATH)/PQCores
LOCAL_C_INCLUDES += external/libxml2/include

LOCAL_SRC_FILES := PQDiag.cpp 

LOCAL_SRC_FILES += Minui/graphics.c \
                   Minui/graphics_drm.c

LOCAL_SRC_FILES += PQCores/PQTuneCore.cpp \
				   PQCores/PQTuneAllCores.cpp

LOCAL_SRC_FILES += PQParsers/ParserCore.cpp \
		PQParsers/AbcParsers.cpp \
		PQParsers/BldParsers.cpp \
		PQParsers/CmsParsers.cpp \
		PQParsers/GammaParsers.cpp \
		PQParsers/sharkl5/AbcParsersLiteR2p0.cpp \
		PQParsers/sharkl5/BldParsersLiteR2p0.cpp \
		PQParsers/sharkl5/CmsParsersLiteR2p0.cpp \
		PQParsers/sharkl5/GammaParsersLiteR2p0.cpp \
		PQParsers/roc1/AbcParsersR3p0.cpp \
		PQParsers/sharkl5Pro/CmsParsersR4p0.cpp \
		PQParsers/sharkl5Pro/AbcParsersR4p0.cpp \
		PQParsers/sharkl5Pro/HsvParsers.cpp 

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

ifeq ($(subst ",,$(TARGET_RECOVERY_PIXEL_FORMAT)),RGBX_8888)
LOCAL_CFLAGS += -DRECOVERY_RGBX
endif

ifeq ($(subst ",,$(TARGET_RECOVERY_PIXEL_FORMAT)),BGRA_8888)
LOCAL_CFLAGS += -DRECOVERY_BGRA
endif

$(warning "TARGET_ARCH= $(TARGET_ARCH)")
ifeq ($(strip $(TARGET_ARCH)),arm64)
	LOCAL_CFLAGS += -DTARGET_ARCH_ARM64
else
ifeq ($(strip $(TARGET_ARCH)),x86_64)
	LOCAL_CFLAGS += -DTARGET_ARCH_x86_64
else
	LOCAL_CFLAGS += -DTARGET_ARCH_ARM
endif
endif
LOCAL_SHARED_LIBRARIES += libcutils   \
			  libutils \
			  libdl \
			  libxml2 \
			  liblog \
			  libdrm

include $(BUILD_SHARED_LIBRARY)

