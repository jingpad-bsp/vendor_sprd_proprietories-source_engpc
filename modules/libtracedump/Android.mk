LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_32_BIT_ONLY := true

LOCAL_SRC_FILES := diag_trace_dump.c

LOCAL_MODULE := libtracedump
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES:= \
	$(TOP)/vendor/sprd/proprietories-source/engpc/sprd_fts_inc \
	$(TOP)/vendor/sprd/proprietories-source/engpc/modules/libtracedump

LOCAL_SHARED_LIBRARIES:= liblog libc libcutils

LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
