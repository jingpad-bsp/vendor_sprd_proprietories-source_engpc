LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_32_BIT_ONLY := true

LOCAL_SRC_FILES := main.cpp

LOCAL_MODULE := engpcctl
LOCAL_SHARED_LIBRARIES := liblog libcutils libengpcctl
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_32_BIT_ONLY := true

LOCAL_SRC_FILES := engpcctl.cpp

LOCAL_C_INCLUDES += \
        $(TOP)/vendor/sprd/proprietories-source/engpc/sprd_fts_inc \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/include \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/chnl \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/trans \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/common \

LOCAL_MODULE := libengpcctl
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)