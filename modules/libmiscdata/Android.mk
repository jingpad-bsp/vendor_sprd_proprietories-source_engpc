LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_32_BIT_ONLY := true

LOCAL_SRC_FILES := miscdata_rw.c miscdata.c crc16.c bt_wifi_mac.c translate_packet.c phasecheck.c

ifeq ($(strip $(TARGET_USERIMAGES_USE_UBIFS)),true)
LOCAL_CFLAGS += -DCONFIG_NAND
endif

LOCAL_MODULE := libmiscdata
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_RELATIVE_PATH := npidevice
LOCAL_PROPRIETARY_MODULE := true

LOCAL_C_INCLUDES:= \
    vendor/sprd/proprietories-source/engpc/sprd_fts_inc \
    vendor/sprd/proprietories-source/engpc/libmiscdata \
    kernel/include/uapi/mtd

LOCAL_SHARED_LIBRARIES:= liblog libc libcutils

include $(BUILD_SHARED_LIBRARY)
