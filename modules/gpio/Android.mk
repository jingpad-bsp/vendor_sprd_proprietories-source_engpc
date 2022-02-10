LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_32_BIT_ONLY := true

LOCAL_SRC_FILES := gpio.cpp

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := libgpio
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_RELATIVE_PATH := npidevice

ifneq (,$(findstring 9863, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkl3
else ifneq (,$(findstring 803, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkl3
else ifneq (,$(findstring 7731, $(TARGET_BOARD_PLATFORM)))
PLATFORM := pike2
else ifneq (,$(findstring 9832, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkle
else ifneq (,$(findstring 9820, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkle
else ifneq (,$(findstring 8521, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkle
else ifneq (,$(findstring 8541, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkle
else ifneq (,$(findstring 9853, $(TARGET_BOARD_PLATFORM)))
PLATFORM := isharkl2
else ifneq (,$(findstring 8781, $(TARGET_BOARD_PLATFORM)))
PLATFORM := isharkl2
else ifneq (,$(findstring ums312, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkl5
else ifneq (,$(findstring ums512, $(TARGET_BOARD_PLATFORM)))
PLATFORM := sharkl5Pro
else ifneq (,$(findstring ud710, $(TARGET_BOARD_PLATFORM)))
PLATFORM := roc1
else
PLATFORM := default
endif

ifeq ($(KERNEL_PATH), kernel4.4)
LOCAL_CFLAGS += -DKERNEL_OLD
else
LOCAL_CFLAGS += -DKERNEL_NEW
endif

$(warning TOP:$(TOP) LOCAL_PATH:$(LOCAL_PATH) TARGET_BOARD_PLATFORM:$(TARGET_BOARD_PLATFORM)  platform:$(PLATFORM))
LOCAL_C_INCLUDES:= \
    $(TOP)/vendor/sprd/proprietories-source/engpc/sprd_fts_inc \
    $(LOCAL_PATH)/$(PLATFORM) \
	$(TARGET_BSP_KERNEL_PATH)/include/uapi

LOCAL_SHARED_LIBRARIES:= \
    libcutils \
    liblog \
    libc

include $(BUILD_SHARED_LIBRARY)
