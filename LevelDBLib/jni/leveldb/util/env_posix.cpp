// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <deque>
#include <set>
#include "leveldb/env.h"
#include "leveldb/slice.h"
#include "port/port.h"
#include "util/logging.h"
#include "util/mutexlock.h"
#include "util/posix_logger.h"
#include "LogUtil.h"

#ifdef BUILD_HADOOP
#include "filesystem/VirtualMemFileSystem.h"
#endif

namespace leveldb {

namespace {

static Status IOError(const std::string& context, int err_number) {
  return Status::IOError(context, strerror(err_number));
}

static Status IOError(const std::string& context, const std::string& msg) {
  return Status::IOError(context, msg.data());
}

#ifdef BUILD_HADOOP

	static VirtualMemFileSystem fileSystem;

	class PosixSequentialFile: public SequentialFile {
	 private:
	  VirtualMemFile * file_;

	 public:
	  PosixSequentialFile(VirtualMemFile *file)
	      : file_(file){ }
	  virtual ~PosixSequentialFile() {file_->close();}

	  virtual Status Read(size_t n, Slice* result, char* scratch) {
		LOGI((file_->getFileName() + " read PosixSequentialFile ").data());

	    Status s;
	    string str;
	    size_t r;

	    LOGI("%s read", file_->getFileName().data());
	    file_->read(result, n);

	    return s;
	  }

	  virtual Status Skip(uint64_t n) {
	    if (file_->skip(n)) {
	      return IOError(file_->getFileName(), file_->getFileName() + "skip error");
	    }
	    return Status::OK();
	  }
	};

	// pread() based random-access
	class PosixRandomAccessFile: public RandomAccessFile {
	 private:
		VirtualMemFile * file_;

	 public:
	  PosixRandomAccessFile(VirtualMemFile *file)
	 : file_(file) { }
	  virtual ~PosixRandomAccessFile() {file_->close();}

	  virtual Status Read(uint64_t offset, size_t n, Slice* result,
	                      char* scratch) const {
		LOGI((file_->getFileName() + " read PosixRandomAccessFile ").data());
	    Status s;
	    string str;
	    size_t r;

	    file_->read(result, offset, n);
	    return s;
	  }
	};

#else

	class PosixSequentialFile: public SequentialFile {
	 private:
	  std::string filename_;
	  FILE* file_;

	 public:
	  PosixSequentialFile(const std::string& fname, FILE* f)
	      : filename_(fname), file_(f) { }
	  virtual ~PosixSequentialFile() { fclose(file_); }

	  virtual Status Read(size_t n, Slice* result, char* scratch) {
	    Status s;
	    size_t r = fread_unlocked(scratch, 1, n, file_);
	    *result = Slice(scratch, r);
	    if (r < n) {
	      if (feof(file_)) {
	        // We leave status as ok if we hit the end of the file
	      } else {
	        // A partial read with an error: return a non-ok status
	        s = IOError(filename_, errno);
	      }
	    }
	    return s;
	  }

	  virtual Status Skip(uint64_t n) {
	    if (fseek(file_, n, SEEK_CUR)) {
	      return IOError(filename_, errno);
	    }
	    return Status::OK();
	  }
	};

	// pread() based random-access
	class PosixRandomAccessFile: public RandomAccessFile {
	 private:
	  std::string filename_;
	  int fd_;

	 public:
	  PosixRandomAccessFile(const std::string& fname, int fd)
	      : filename_(fname), fd_(fd) { }
	  virtual ~PosixRandomAccessFile() { close(fd_); }

	  virtual Status Read(uint64_t offset, size_t n, Slice* result,
	                      char* scratch) const {
	    Status s;
	    ssize_t r = pread(fd_, scratch, n, static_cast<off_t>(offset));
	    *result = Slice(scratch, (r < 0) ? 0 : r);
	    if (r < 0) {
	      // An error: return a non-ok status
	      s = IOError(filename_, errno);
	    }
	    return s;
	  }
	};

	// Helper class to limit mmap file usage so that we do not end up
	// running out virtual memory or running into kernel performance
	// problems for very large databases.
	class MmapLimiter {
	 public:
	  // Up to 1000 mmaps for 64-bit binaries; none for smaller pointer sizes.
	  MmapLimiter() {
	    SetAllowed(sizeof(void*) >= 8 ? 1000 : 0);
	  }

	  // If another mmap slot is available, acquire it and return true.
	  // Else return false.
	  bool Acquire() {
	    if (GetAllowed() <= 0) {
	      return false;
	    }
	    MutexLock l(&mu_);
	    intptr_t x = GetAllowed();
	    if (x <= 0) {
	      return false;
	    } else {
	      SetAllowed(x - 1);
	      return true;
	    }
	  }

	  // Release a slot acquired by a previous call to Acquire() that returned true.
	  void Release() {
	    MutexLock l(&mu_);
	    SetAllowed(GetAllowed() + 1);
	  }

	 private:
	  port::Mutex mu_;
	  port::AtomicPointer allowed_;

	  intptr_t GetAllowed() const {
	    return reinterpret_cast<intptr_t>(allowed_.Acquire_Load());
	  }

	  // REQUIRES: mu_ must be held
	  void SetAllowed(intptr_t v) {
	    allowed_.Release_Store(reinterpret_cast<void*>(v));
	  }

	  MmapLimiter(const MmapLimiter&);
	  void operator=(const MmapLimiter&);
	};

	// mmap() based random-access
	class PosixMmapReadableFile: public RandomAccessFile {
	 private:
	  std::string filename_;
	  void* mmapped_region_;
	  size_t length_;
	  MmapLimiter* limiter_;

	 public:
	  // base[0,length-1] contains the mmapped contents of the file.
	  PosixMmapReadableFile(const std::string& fname, void* base, size_t length,
	                        MmapLimiter* limiter)
	      : filename_(fname), mmapped_region_(base), length_(length),
	        limiter_(limiter) {
	  }

	  virtual ~PosixMmapReadableFile() {
	    munmap(mmapped_region_, length_);
	    limiter_->Release();
	  }

	  virtual Status Read(uint64_t offset, size_t n, Slice* result,
	                      char* scratch) const {
	    Status s;
	    if (offset + n > length_) {
	      *result = Slice();
	      s = IOError(filename_, EINVAL);
	    } else {
	      *result = Slice(reinterpret_cast<char*>(mmapped_region_) + offset, n);
	    }
	    return s;
	  }
	};
#endif

#ifdef BUILD_HADOOP
class PosixWritableFile : public WritableFile {
 private:
  VirtualMemFile *_file;

 public:
  PosixWritableFile(VirtualMemFile *file)
      : _file(file){ }

  ~PosixWritableFile() { _file->close(); }

  virtual Status Append(const Slice& data) {

	  if(!_file->write(data.data(), data.size()))
		  return IOError(_file->getFileName(), _file->getFileName() + "PosixWritableFile append error");
	  else
	  {
		  return Status::OK();
	  }
  }

  virtual Status Close() {
	  _file->close();

	  return Status::OK();
  }

  virtual Status Flush() {
    if(_file->flush() == false)
    	return Status::IOError(_file->getFileName() + " flush failed");

    return Status::OK();
  }

  Status SyncDirIfManifest() {
	  return Status::OK();
  }

  virtual Status Sync() {
    // Ensure new files referred to by the manifest are in the filesystem.
	  return Status::OK();
  }
};
static int LockOrUnlock(int fd, bool lock) {
	  VirtualMemFile * file = fileSystem.getFile(fd);
	  if(file == NULL)
		  return -1;

	  if(lock && file->lockFile())
		  return 0;
	  else if(!lock && file->unlockFile())
		  return 0;
	  else
		  return -1;
}
#else
class PosixWritableFile : public WritableFile {
 private:
  std::string filename_;
  FILE* file_;

 public:
  PosixWritableFile(const std::string& fname, FILE* f)
      : filename_(fname), file_(f) { }

  ~PosixWritableFile() {
    if (file_ != NULL) {
      // Ignoring any potential errors
      fclose(file_);
    }
  }

  virtual Status Append(const Slice& data) {
    size_t r = fwrite_unlocked(data.data(), 1, data.size(), file_);
    if (r != data.size()) {
      return IOError(filename_, errno);
    }
    return Status::OK();
  }

  virtual Status Close() {
    Status result;
    if (fclose(file_) != 0) {
      result = IOError(filename_, errno);
    }
    file_ = NULL;
    return result;
  }

  virtual Status Flush() {
    if (fflush_unlocked(file_) != 0) {
      return IOError(filename_, errno);
    }
    return Status::OK();
  }

  Status SyncDirIfManifest() {
    const char* f = filename_.c_str();
    const char* sep = strrchr(f, '/');
    Slice basename;
    std::string dir;
    if (sep == NULL) {
      dir = ".";
      basename = f;
    } else {
      dir = std::string(f, sep - f);
      basename = sep + 1;
    }
    Status s;
    if (basename.end_with("manifest")) {
      int fd = open(dir.c_str(), O_RDONLY);
      if (fd < 0) {
        s = IOError(dir, errno);
      } else {
        if (fsync(fd) < 0) {
          s = IOError(dir, errno);
        }
        close(fd);
      }
    }
    return s;
  }

  virtual Status Sync() {
    // Ensure new files referred to by the manifest are in the filesystem.
    Status s = SyncDirIfManifest();
    if (!s.ok()) {
      return s;
    }
    if (fflush_unlocked(file_) != 0 ||
        fdatasync(fileno(file_)) != 0) {
      s = Status::IOError(filename_, strerror(errno));
    }
    return s;
  }
};

static int LockOrUnlock(int fd, bool lock) {
  errno = 0;
  struct flock f;
  memset(&f, 0, sizeof(f));
  f.l_type = (lock ? F_WRLCK : F_UNLCK);
  f.l_whence = SEEK_SET;
  f.l_start = 0;
  f.l_len = 0;        // Lock/unlock entire file
  return fcntl(fd, F_SETLK, &f);
}
#endif

class PosixFileLock : public FileLock {
 public:
  int fd_;
  std::string name_;
};

// Set of locked files.  We keep a separate set instead of just
// relying on fcntrl(F_SETLK) since fcntl(F_SETLK) does not provide
// any protection against multiple uses from the same process.
class PosixLockTable {
 private:
  port::Mutex mu_;
  std::set<std::string> locked_files_;
 public:
  bool Insert(const std::string& fname) {
    MutexLock l(&mu_);
    return locked_files_.insert(fname).second;
  }
  void Remove(const std::string& fname) {
    MutexLock l(&mu_);
    locked_files_.erase(fname);
  }
};

class PosixEnv : public Env {
 public:
  PosixEnv();
  virtual ~PosixEnv() {
    char msg[] = "Destroying Env::Default()\n";
    fwrite(msg, 1, sizeof(msg), stderr);
    abort();
  }

#ifdef BUILD_HADOOP
  virtual Status NewSequentialFile(const std::string& fname,
                                     SequentialFile** result) {
	  VirtualMemFile * file = fileSystem.createFile(fname);
	  file->seek(0);
	  *result = new PosixSequentialFile(file);

	  return Status::OK();
    }

    virtual Status NewRandomAccessFile(const std::string& fname,
                                       RandomAccessFile** result) {

  	  VirtualMemFile * file = fileSystem.createFile(fname);
  	  *result = new PosixRandomAccessFile(file);
  	  return Status::OK();
    }

    virtual Status NewWritableFile(const std::string& fname,
                                   WritableFile** result) {
    	  VirtualMemFile * file = fileSystem.createFile(fname);
    	  file->clean();
    	  *result = new PosixWritableFile(file);
    	  return Status::OK();
    }

    virtual Status NewAppendableFile(const std::string& fname,
                                     WritableFile** result) {

    	VirtualMemFile * file = fileSystem.createFile(fname);
    	file->seek(file->getFileSize());
		*result = new PosixWritableFile(file);
		return Status::OK();
    }

    virtual bool FileExists(const std::string& fname) {
      return fileSystem.isContainFile(fname);
    }

    virtual Status GetChildren(const std::string& dir, const std::string& dbname,
                               std::vector<std::string>* result) {
    	if(!fileSystem.getChildrenName(dir, dbname, result))
    		return IOError(dir, dir+ "get child error");
    	else
    		return Status::OK();
    }

    virtual Status DeleteFile(const std::string& fname) {
    	LOGE((std::string("delete file ") + fname).data());
		if(!fileSystem.deleteFile(fname))
			return IOError(fname, fname + "delete error");
		else
			return Status::OK();
    }

    virtual Status CreateDir(const std::string& name) {
    	if(!fileSystem.createDir(name))
			return IOError(name, name + "create dir error");
		else
			return Status::OK();
    }

    virtual Status DeleteDir(const std::string& name) {
    	if(!fileSystem.deleteDir(name))
			return IOError(name, name + "delete dir error");
		else
			return Status::OK();
    }

    virtual Status GetFileSize(const std::string& fname, uint64_t* size) {
    	VirtualMemFile* file = fileSystem.getFile(fname);
    	if(file == NULL)
			return IOError(fname, fname + "get file size error");

    	*size = file->getFileSize();
    	return Status::OK();
    }

    virtual Status RenameFile(const std::string& src, const std::string& target) {
    	VirtualMemFile* file = fileSystem.getFile(src);
    	if(file == NULL)
    		return IOError(src, src + "rename error");

    	file->rename(target);
		return Status::OK();
    }

    virtual Status LockFile(const std::string& fname, FileLock** lock) {
      *lock = NULL;
      Status result;

      VirtualMemFile *file = fileSystem.getFile(fname);
      if(file == NULL)
    	  file = fileSystem.createFile(fname);
      if (file == NULL) {
        result = IOError(fname, fname + "create file error");
      } else if (!locks_.Insert(fname)) {
        result = Status::IOError("lock " + fname, "already held by process");
      } else if (LockOrUnlock(file->getFd(), true) == -1) {
        result = IOError("lock " + fname, fname + "lock file error");
        locks_.Remove(fname);
      } else {
        PosixFileLock* my_lock = new PosixFileLock;
        my_lock->fd_ = file->getFd();
        my_lock->name_ = fname;
        *lock = my_lock;
      }
      return result;
    }

    virtual Status UnlockFile(FileLock* lock) {
      PosixFileLock* my_lock = reinterpret_cast<PosixFileLock*>(lock);
      Status result;
      if (LockOrUnlock(my_lock->fd_, false) == -1) {
        result = IOError("unlock", "unlock file error");
      }
      locks_.Remove(my_lock->name_);
      delete my_lock;
      return result;
    }
#else
  virtual Status NewSequentialFile(const std::string& fname,
                                     SequentialFile** result) {
      FILE* f = fopen(fname.c_str(), "r");
      if (f == NULL) {
        *result = NULL;
        return IOError(fname, errno);
      } else {
        *result = new PosixSequentialFile(fname, f);
        return Status::OK();
      }
    }

    virtual Status NewRandomAccessFile(const std::string& fname,
                                       RandomAccessFile** result) {
      *result = NULL;
      Status s;
      int fd = open(fname.c_str(), O_RDONLY);
      if (fd < 0) {
        s = IOError(fname, errno);
      } else if (mmap_limit_.Acquire()) {
        uint64_t size;
        s = GetFileSize(fname, &size);
        if (s.ok()) {
          void* base = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
          if (base != MAP_FAILED) {
            *result = new PosixMmapReadableFile(fname, base, size, &mmap_limit_);
          } else {
            s = IOError(fname, errno);
          }
        }
        close(fd);
        if (!s.ok()) {
          mmap_limit_.Release();
        }
      } else {
        *result = new PosixRandomAccessFile(fname, fd);
      }
      return s;
    }

    virtual Status NewWritableFile(const std::string& fname,
                                   WritableFile** result) {
      Status s;
      FILE* f = fopen(fname.c_str(), "w");
      if (f == NULL) {
        *result = NULL;
        s = IOError(fname, errno);
      } else {
        *result = new PosixWritableFile(fname, f);
      }
      return s;
    }

    virtual Status NewAppendableFile(const std::string& fname,
                                     WritableFile** result) {
      Status s;
      FILE* f = fopen(fname.c_str(), "a");
      if (f == NULL) {
        *result = NULL;
        s = IOError(fname, errno);
      } else {
        *result = new PosixWritableFile(fname, f);
      }
      return s;
    }

    virtual bool FileExists(const std::string& fname) {
      return access(fname.c_str(), F_OK) == 0;
    }

    virtual Status GetChildren(const std::string& dir, const std::string& dbname,
                               std::vector<std::string>* result) {
      result->clear();
      DIR* d = opendir(dir.c_str());
      if (d == NULL) {
        return IOError(dir, errno);
      }
      struct dirent* entry;
      while ((entry = readdir(d)) != NULL) {
      	if(strncmp(entry->d_name, dbname.data(), dbname.length()) == 0)
      	{
      			result->push_back(entry->d_name);
      			LOGW((std::string("get child -> ") + entry->d_name).data());
      	}
      }
      closedir(d);
      return Status::OK();
    }

    virtual Status DeleteFile(const std::string& fname) {
  	  LOGE((std::string("delete file ") + fname).data());
      Status result;
      if (unlink(fname.c_str()) != 0) {
        result = IOError(fname, errno);
      }
      return result;
    }

    virtual Status CreateDir(const std::string& name) {
      Status result;
      if (mkdir(name.c_str(), 0755) != 0) {
        result = IOError(name, errno);
      }
      return result;
    }

    virtual Status DeleteDir(const std::string& name) {
      Status result;
      if (rmdir(name.c_str()) != 0) {
        result = IOError(name, errno);
      }
      return result;
    }

    virtual Status GetFileSize(const std::string& fname, uint64_t* size) {
      Status s;
      struct stat sbuf;
      if (stat(fname.c_str(), &sbuf) != 0) {
        *size = 0;
        s = IOError(fname, errno);
      } else {
        *size = sbuf.st_size;
      }
      return s;
    }

    virtual Status RenameFile(const std::string& src, const std::string& target) {
      Status result;
      if (rename(src.c_str(), target.c_str()) != 0) {
        result = IOError(src, errno);
      }
      return result;
    }

    virtual Status LockFile(const std::string& fname, FileLock** lock) {
      *lock = NULL;
      Status result;
      int fd = open(fname.c_str(), O_RDWR | O_CREAT, 0644);
      if (fd < 0) {
        result = IOError(fname, errno);
      } else if (!locks_.Insert(fname)) {
        close(fd);
        result = Status::IOError("lock " + fname, "already held by process");
      } else if (LockOrUnlock(fd, true) == -1) {
        result = IOError("lock " + fname, errno);
        close(fd);
        locks_.Remove(fname);
      } else {
        PosixFileLock* my_lock = new PosixFileLock;
        my_lock->fd_ = fd;
        my_lock->name_ = fname;
        *lock = my_lock;
      }
      return result;
    }

    virtual Status UnlockFile(FileLock* lock) {
      PosixFileLock* my_lock = reinterpret_cast<PosixFileLock*>(lock);
      Status result;
      if (LockOrUnlock(my_lock->fd_, false) == -1) {
        result = IOError("unlock", errno);
      }
      locks_.Remove(my_lock->name_);
      close(my_lock->fd_);
      delete my_lock;
      return result;
    }
#endif



  virtual void Schedule(void (*function)(void*), void* arg);

  virtual void StartThread(void (*function)(void* arg), void* arg);

  virtual Status GetTestDirectory(std::string* result) {
    const char* env = getenv("TEST_TMPDIR");
    if (env && env[0] != '\0') {
      *result = env;
    } else {
      char buf[100];
      snprintf(buf, sizeof(buf), "/tmp/leveldbtest-%d", int(geteuid()));
      *result = buf;
    }
    // Directory may already exist
    CreateDir(*result);
    return Status::OK();
  }

  static uint64_t gettid() {
    pthread_t tid = pthread_self();
    uint64_t thread_id = 0;
    memcpy(&thread_id, &tid, std::min(sizeof(thread_id), sizeof(tid)));
    return thread_id;
  }

  virtual Status NewLogger(const std::string& fname, Logger** result) {
    FILE* f = fopen(fname.c_str(), "w");
    if (f == NULL) {
      *result = NULL;
      return IOError(fname, errno);
    } else {
      *result = new PosixLogger(f, &PosixEnv::gettid);
      return Status::OK();
    }
  }

  virtual uint64_t NowMicros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
  }

  virtual void SleepForMicroseconds(int micros) {
    usleep(micros);
  }

#ifdef BUILD_HADOOP
  virtual void CloseAllFile();
  virtual void printFileSystem();
#endif

 private:
  void PthreadCall(const char* label, int result) {
    if (result != 0) {
      fprintf(stderr, "pthread %s: %s\n", label, strerror(result));
      abort();
    }
  }

  // BGThread() is the body of the background thread
  void BGThread();
  static void* BGThreadWrapper(void* arg) {
    reinterpret_cast<PosixEnv*>(arg)->BGThread();
    return NULL;
  }

  pthread_mutex_t mu_;
  pthread_cond_t bgsignal_;
  pthread_t bgthread_;
  bool started_bgthread_;

  // Entry per Schedule() call
  struct BGItem { void* arg; void (*function)(void*); };
  typedef std::deque<BGItem> BGQueue;
  BGQueue queue_;

  PosixLockTable locks_;

#ifndef BUILD_HADOOP
  MmapLimiter mmap_limit_;
#endif
};

PosixEnv::PosixEnv() : started_bgthread_(false) {
  PthreadCall("mutex_init", pthread_mutex_init(&mu_, NULL));
  PthreadCall("cvar_init", pthread_cond_init(&bgsignal_, NULL));
}

void PosixEnv::Schedule(void (*function)(void*), void* arg) {
  PthreadCall("lock", pthread_mutex_lock(&mu_));

  // Start background thread if necessary
  if (!started_bgthread_) {
    started_bgthread_ = true;
    PthreadCall(
        "create thread",
        pthread_create(&bgthread_, NULL,  &PosixEnv::BGThreadWrapper, this));
  }

  // If the queue is currently empty, the background thread may currently be
  // waiting.
  if (queue_.empty()) {
    PthreadCall("signal", pthread_cond_signal(&bgsignal_));
  }

  // Add to priority queue
  queue_.push_back(BGItem());
  queue_.back().function = function;
  queue_.back().arg = arg;

  PthreadCall("unlock", pthread_mutex_unlock(&mu_));
}

void PosixEnv::BGThread() {
  while (true) {
    // Wait until there is an item that is ready to run
    PthreadCall("lock", pthread_mutex_lock(&mu_));
    while (queue_.empty()) {
      PthreadCall("wait", pthread_cond_wait(&bgsignal_, &mu_));
    }

    void (*function)(void*) = queue_.front().function;
    void* arg = queue_.front().arg;
    queue_.pop_front();

    PthreadCall("unlock", pthread_mutex_unlock(&mu_));
    (*function)(arg);
  }
}

#ifdef BUILD_HADOOP

void PosixEnv::CloseAllFile()
{
	fileSystem.closeAll();
}

void PosixEnv::printFileSystem()
{
	LOGI(fileSystem.toString().data());
}

#endif

namespace {
struct StartThreadState {
  void (*user_function)(void*);
  void* arg;
};
}
static void* StartThreadWrapper(void* arg) {
  StartThreadState* state = reinterpret_cast<StartThreadState*>(arg);
  state->user_function(state->arg);
  delete state;
  return NULL;
}

void PosixEnv::StartThread(void (*function)(void* arg), void* arg) {
  pthread_t t;
  StartThreadState* state = new StartThreadState;
  state->user_function = function;
  state->arg = arg;
  PthreadCall("start thread",
              pthread_create(&t, NULL,  &StartThreadWrapper, state));
}

}  // namespace

static pthread_once_t once = PTHREAD_ONCE_INIT;
static Env* default_env;
static void InitDefaultEnv() { default_env = new PosixEnv; }

Env* Env::Default() {
  pthread_once(&once, InitDefaultEnv);
  return default_env;
}

}  // namespace leveldb
