// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <ctype.h>
#include <stdio.h>
#include <util/LogUtil.h>
#include "db/filename.h"
#include "db/dbformat.h"
#include "leveldb/env.h"
#include "util/logging.h"

namespace leveldb {

// A utility routine: write "data" to the named file and Sync() it.
extern Status WriteStringToFileSync(Env* env, const Slice& data,
		const std::string& fname);

static std::string MakeFileName(const std::string& dbpath,
		const std::string& name, uint64_t number, const char* suffix) {
	char buf[100];
	snprintf(buf, sizeof(buf), "%s%s.%06llu.%s", dbpath.data(), name.data(), number, suffix);
	return buf;
}

static std::string MakeFileName(const std::string& dbpath,
		const std::string& dbname, const char* suffix) {
	char buf[100];
	snprintf(buf, sizeof(buf), "%s%s.%s", dbpath.data(), dbname.data(), suffix);
	return buf;
}

static std::string MakeFileName(const std::string& dbname, uint64_t number, const char* suffix) {
	char buf[100];
	snprintf(buf, sizeof(buf), "%s.%06llu.%s", dbname.data(), number, suffix);
	return buf;
}

static std::string MakeFileName(const std::string& dbname, const char* suffix) {
	char buf[100];
	snprintf(buf, sizeof(buf), "%s.%s", dbname.data(), suffix);
	return buf;
}

std::string LogFileName(const std::string& dbpath, const std::string& dbname,
		uint64_t number) {
	assert(number > 0);
	return MakeFileName(dbpath, dbname, number, "log");
}

std::string TableFileName(const std::string& dbpath, const std::string& dbname,
		uint64_t number) {
	assert(number >= 0);
	if(number == DB_NUM_NONE)
		return MakeFileName(dbpath, dbname, "ldb");
	else
		return MakeFileName(dbpath, dbname, number, "ldb");
}

std::string SSTTableFileName(const std::string& dbpath, const std::string& dbname,
		uint64_t number) {
	assert(number > 0);
	return MakeFileName(dbpath, dbname, number, "sst");
}

std::string DescriptorFileName(const std::string& dbpath,
		const std::string& dbname, uint64_t number) {
//	return MakeFileName(dbpath, dbname, number, "manifest");
	return MakeFileName(dbpath, dbname, "manifest");
}

std::string DescriptorFileName(const std::string& dbname, uint64_t number) {
//	return MakeFileName(dbname, number, "manifest");
	return MakeFileName(dbname, "manifest");
}

std::string CurrentFileName(const std::string& dbpath,
		const std::string& dbname) {
	return MakeFileName(dbpath, dbname, "current");
}

std::string LockFileName(const std::string& dbpath, const std::string& dbname) {
	return MakeFileName(dbpath, dbname, "lock");
}

std::string TempFileName(const std::string& dbpath, const std::string& dbname,
		uint64_t number) {
	assert(number > 0);
	return MakeFileName(dbpath, dbname, number, "dbtmp");
}

std::string InfoLogFileName(const std::string& dbpath,
		const std::string& dbname) {
	return MakeFileName(dbpath, dbname, "lg");
}

// Return the name of the old info log file for "dbname".
std::string OldInfoLogFileName(const std::string& dbpath,
		const std::string& dbname) {
	return MakeFileName(dbpath, dbname, "oldlg");
}

// Owned filenames have the form:
//    dbname/CURRENT
//    dbname/LOCK
//    dbname/LOG
//    dbname/LOG.old
//    dbname/MANIFEST-[0-9]+
//    dbname/[0-9]+.(log|sst|ldb)
bool ParseFileName(const std::string& dbpath, const std::string& fname, uint64_t* number, FileType* type) {
	Slice rest(fname);
	std::string suffix;
	uint64_t tmpNumber;
	if (GetFileSuffix(fname, suffix) == false) {
		LOGE("get file suffix failed");
		return false;
	}

	if (suffix == "current") {
		*number = 0;
		*type = kCurrentFile;
	} else if (suffix == "lock") {
		*number = 0;
		*type = kDBLockFile;
	} else if (suffix == "lg" || suffix == "oldlg") {
		*number = 0;
		*type = kInfoLogFile;
	} else if (suffix == "manifest") {
//		rest.remove_prefix(fname.find_first_of('.'));
//		uint64_t num;
//		if (!ConsumeDecimalNumber(&rest, &num)) {
//		  return false;
//		}
//		if (rest.empty()) {
//		  return false;
//		}
		if((fname.substr(0, fname.find_first_of('.') + 1) + "manifest") != fname)
			return false;
		*type = kDescriptorFile;
//		*number = num;
		*number = 1;
	} else {
		// Avoid strtoull() to keep filename format independent of the
		// current locale
		uint64_t num;
		rest.remove_prefix(fname.find_first_of('.') + 1);
		if (!ConsumeDecimalNumber(&rest, &num)) {
			return false;
		}
		if (suffix == "log") {
			*type = kLogFile;
		} else if (suffix == "sst" || suffix == "ldb") {
			*type = kTableFile;
		} else if (suffix == "dbtmp") {
			*type = kTempFile;
		} else {
			return false;
		}
		*number = num;
	}
	return true;
}

Status SetCurrentFile(Env* env, const std::string& dbpath,
		const std::string& dbname, uint64_t descriptor_number) {	//不需要current文件
	// Remove leading "dbname/" and add newline to manifest file name
//	LOGE("SetCurrentFile");
//	std::string manifest = DescriptorFileName(dbname, descriptor_number);
//	LOGE((std::string("current manifest  ") + manifest).data());
//	Slice contents = manifest;
////  assert(contents.starts_with(dbname + "/"));
////  contents.remove_prefix(dbname.size() + 1);
//	std::string tmp = TempFileName(dbpath, dbname, descriptor_number);
//	Status s = WriteStringToFileSync(env, contents.ToString() + "\n", tmp);
//	if (s.ok()) {
//		s = env->RenameFile(tmp, CurrentFileName(dbpath, dbname));
//	}
//	if (!s.ok()) {
//		env->DeleteFile(tmp);
//	}
//	return s;
	return Status::OK();
}

}  // namespace leveldb
