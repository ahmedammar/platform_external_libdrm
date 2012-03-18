LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=				\
	drmstat.c

LOCAL_C_INCLUDES +=				\
	$(LOCAL_PATH)/..			\
	$(LOCAL_PATH)/../include/drm

LOCAL_MODULE := drmstat
LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES :=			\
	libdrm					\

include $(BUILD_EXECUTABLE)
