LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libandroid_policy

LOCAL_SRC_FILES := swl_monitor_PolicyManager.cpp Policy.cpp

LOCAL_SHARED_LIBRARIES := \
    	libandroid_runtime \
	libcutils \
	libnativehelper \
	libutils 

LOCAL_C_INCLUDES += \
	$(JNI_H_INCLUDE)

LOCAL_PRELINK_MODULE := false
# if set LOCAL_MODULE_TAGS as optional, 
# the out folder is symbols/system/ instead of /system/
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)
