# Copyright (c) 2011 The LevelDB Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

# Definitions of build variable common between Android and non-Android builds.

# Also add /include directory for extra leveldb includes.
C_INCLUDES = . ./include

SOURCES = \
	./leveldb/db/builder.cc \
	./leveldb/db/dbformat.cc \
	./leveldb/db/db_impl.cc \
	./leveldb/db/memtable.cc \
	./leveldb/db/version_edit.cc \
	./leveldb/db/version_set.cc \
	./leveldb/db/write_batch.cc \
	./leveldb/db/c.cc \
	./leveldb/db/db_iter.cc \
	./leveldb/db/filename.cc \
	./leveldb/db/log_reader.cc \
	./leveldb/db/log_writer.cc \
	./leveldb/db/repair.cc \
	./leveldb/db/table_cache.cc \
	./leveldb/table/block_builder.cc \
	./leveldb/table/block.cc \
	./leveldb/table/format.cc \
	./leveldb/table/iterator.cc \
	./leveldb/table/merger.cc \
	./leveldb/table/table_builder.cc \
	./leveldb/table/table.cc \
	./leveldb/table/filter_block.cc \
	./leveldb/table/two_level_iterator.cc \
	./leveldb/util/arena.cc \
	./leveldb/util/cache.cc \
	./leveldb/util/coding.cc \
	./leveldb/util/comparator.cc \
	./leveldb/util/env_posix.cc \
	./leveldb/util/hash.cc \
	./leveldb/util/logging.cc \
	./leveldb/util/status.cc \
	./leveldb/util/crc32c.cc \
	./leveldb/util/env.cc \
	./leveldb/util/histogram.cc \
	./leveldb/util/options.cc \
	./leveldb/util/filter_policy.cc \
	./leveldb/util/bloom.cc \
	./leveldb/port/port_posix.cc