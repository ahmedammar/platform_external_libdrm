LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=				\
	drm_pageflip_test.c

LOCAL_C_INCLUDES +=				\
	external/drm/			\
	external/drm/include/drm

LOCAL_MODULE := drm_pageflip_test
LOCAL_MODULE_TAGS := debug

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES :=			\
	libdrm					\
	libcutils

include $(BUILD_EXECUTABLE)
