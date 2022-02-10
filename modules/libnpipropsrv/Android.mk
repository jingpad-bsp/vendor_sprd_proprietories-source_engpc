LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := npipropsrv.cpp \
    eng_attok_test.c \

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := libnpipropsrv
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES:= \
    $(TOP)/vendor/sprd/proprietories-source/engpc/sprd_fts_inc \

LOCAL_SHARED_LIBRARIES:= \
    libcutils \
    libdl \
    libutils \
    liblog	\
    libbase \
    libhardware \

AUDIO_NPI_FILE := /vendor/lib/libnpipropsrv.so
SYMLINK := $(TARGET_OUT_VENDOR)/lib/npidevice/libnpipropsrv.so
LOCAL_POST_INSTALL_CMD := $(hide) \
	mkdir -p $(TARGET_OUT_VENDOR)/lib/npidevice; \
	rm -rf $(SYMLINK) ;\
	ln -sf $(AUDIO_NPI_FILE) $(SYMLINK);

include $(BUILD_SHARED_LIBRARY)
