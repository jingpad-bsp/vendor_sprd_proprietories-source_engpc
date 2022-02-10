LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_32_BIT_ONLY := true

#STD_OS_DEFINE := OS_MINIAP
STD_OS_DEFINE := OS_ANDROID
#STD_OS_DEFINE := OS_YUN
#STD_OS_DEFINE := OS_KAI

file_list := $(wildcard $(LOCAL_PATH)/*.cpp)

ifeq ($(STD_OS_DEFINE), OS_ANDROID)
include $(LOCAL_PATH)/android/include.mk
else ifeq ($(STD_OS_DEFINE), OS_YUN)
include $(LOCAL_PATH)/yunos/include.mk
else ifeq ($(STD_OS_DEFINE), OS_KAI)
include $(LOCAL_PATH)/kaios/include.mk
else ifeq ($(STD_OS_DEFINE), OS_MINIAP)
include $(LOCAL_PATH)/miniap/include.mk
else
include $(LOCAL_PATH)/android/include.mk
endif

LOCAL_SRC_FILES := $(file_list:$(LOCAL_PATH)/%=%)

LOCAL_SHARED_LIBRARIES := libc \
                          liblog \
                          libutils \
                          libcutils \
                          libdl

LOCAL_MODULE := libengpc_adapt
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)
