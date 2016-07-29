LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/leveldb.mk
include $(LOCAL_PATH)/kvdb.mk

LOCAL_MODULE := LevelDB
LOCAL_LDLIBS += -latomic -landroid -llog -pthread
LOCAL_CPP_FEATURES := exceptions rtti
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include/ $(LOCAL_PATH)/KvDB/ $(LOCAL_PATH)/leveldb/include/ $(LOCAL_PATH)/leveldb/
LOCAL_CFLAGS := -DLEVELDB_PLATFORM_POSIX -DOS_ANDROID=1 #-DBUILD_HADOOP=1
LOCAL_SRC_FILES := $(SOURCES:%.cc=%.cc) $(SOURCES_KVDB) LevelDBAPI.cpp
include $(BUILD_SHARED_LIBRARY)
