LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := appfence
LOCAL_SRC_FILES := atrace.c trace.c chpath.c 

include $(BUILD_EXECUTABLE)
