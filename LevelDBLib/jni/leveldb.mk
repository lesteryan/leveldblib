# Copyright (c) 2011 The LevelDB Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

# Definitions of build variable common between Android and non-Android builds.

# Also add /include directory for extra leveldb includes.
C_INCLUDES = . ./include

SOURCES = \
	./leveldb/db/builder.cpp \
	./leveldb/db/dbformat.cpp \
	./leveldb/db/db_impl.cpp \
	./leveldb/db/memtable.cpp \
	./leveldb/db/version_edit.cpp \
	./leveldb/db/version_set.cpp \
	./leveldb/db/write_batch.cpp \
	./leveldb/db/c.cpp \
	./leveldb/db/db_iter.cpp \
	./leveldb/db/filename.cpp \
	./leveldb/db/log_reader.cpp \
	./leveldb/db/log_writer.cpp \
	./leveldb/db/repair.cpp \
	./leveldb/db/table_cache.cpp \
	./leveldb/table/block_builder.cpp \
	./leveldb/table/block.cpp \
	./leveldb/table/format.cpp \
	./leveldb/table/iterator.cpp \
	./leveldb/table/merger.cpp \
	./leveldb/table/table_builder.cpp \
	./leveldb/table/table.cpp \
	./leveldb/table/filter_block.cpp \
	./leveldb/table/two_level_iterator.cpp \
	./leveldb/util/arena.cpp \
	./leveldb/util/cache.cpp \
	./leveldb/util/coding.cpp \
	./leveldb/util/comparator.cpp \
	./leveldb/util/env_posix.cpp \
	./leveldb/util/hash.cpp \
	./leveldb/util/logging.cpp \
	./leveldb/util/status.cpp \
	./leveldb/util/crc32c.cpp \
	./leveldb/util/env.cpp \
	./leveldb/util/histogram.cpp \
	./leveldb/util/options.cpp \
	./leveldb/util/filter_policy.cpp \
	./leveldb/util/bloom.cpp \
	./leveldb/port/port_posix.cpp