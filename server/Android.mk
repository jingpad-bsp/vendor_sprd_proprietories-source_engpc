LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_32_BIT_ONLY := true

#ENGPC_AP_CALI := true
ifeq ($(ENGPC_AP_CALI), true)
LOCAL_CFLAGS += -DENGPC_AP_CALI
endif

#LOCAL_CFLAGS += -D_USE_STD_CPP_LIB_

file_list := $(wildcard $(LOCAL_PATH)/*.cpp)
file_list += $(wildcard $(LOCAL_PATH)/chnl/*.cpp)
file_list += $(wildcard $(LOCAL_PATH)/module/*.cpp)
file_list += $(wildcard $(LOCAL_PATH)/trans/*.cpp)
file_list += $(wildcard $(LOCAL_PATH)/common/*.cpp)

LOCAL_SRC_FILES := $(file_list:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += \
        $(TOP)/vendor/sprd/proprietories-source/engpc/sprd_fts_inc \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/include \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/chnl \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/trans \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/common \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/module \
        $(TOP)/vendor/sprd/proprietories-source/engpc/server/adapt \
        vendor/sprd/modules/wcn/bt/libbqbbt/include/ \

LOCAL_SHARED_LIBRARIES := libc \
                          liblog \
                          libutils \
                          libcutils \
                          libdl \
                          libengpc_adapt \

LOCAL_MODULE := engpc
LOCAL_INIT_RC := engpc.rc
LOCAL_MODULE_TAGS := optional
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_EXECUTABLE)
include $(LOCAL_PATH)/adapt/Android.mk
