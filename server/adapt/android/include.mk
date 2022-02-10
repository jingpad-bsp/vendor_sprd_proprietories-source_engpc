
file_list += $(wildcard $(LOCAL_PATH)/android/*.cpp)
LOCAL_C_INCLUDES    += $(LOCAL_PATH)/android/

ifeq ($(PLATFORM_VERSION),10)
    LOCAL_CFLAGS += -DENGPC_VSER_INIT
endif