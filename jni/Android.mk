LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := appfence
LOCAL_SRC_FILES := zygote_helper.c test.c ptraceaux.c sandbox_helper.c file_toolkit.c binder_helper.c

include $(BUILD_EXECUTABLE)
