LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/leveldb.mk
include $(LOCAL_PATH)/kvdb.mk

LOCAL_MODULE := LevelDB
LOCAL_LDLIBS += -latomic -landroid -llog
LOCAL_CPP_FEATURES := exceptions rtti
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/ $(LOCAL_PATH)/KvDB/ $(LOCAL_PATH)/leveldb/include/ $(LOCAL_PATH)/leveldb/
LOCAL_CPP_EXTENSION := .cc
LOCAL_CFLAGS := -DLEVELDB_PLATFORM_POSIX -DOS_ANDROID=1
LOCAL_SRC_FILES := $(SOURCES:%.cc=%.cc) $(SOURCES_KVDB) LevelDB.cc
include $(BUILD_SHARED_LIBRARY)
