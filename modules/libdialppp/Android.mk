LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := pppdial.cpp

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := libpppDial
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_RELATIVE_PATH := npidevice

LOCAL_C_INCLUDES:= \
    $(TOP)/vendor/sprd/proprietories-source/engpc/sprd_fts_inc \
    $(TOP)/vendor/sprd/modules/libatci

LOCAL_SHARED_LIBRARIES := libcutils libutils liblog libhidlbase libhidltransport libhwbinder libatci

include $(BUILD_SHARED_LIBRARY)
