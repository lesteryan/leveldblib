/*
 * FdManager.h
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#ifndef FDMANAGER_H_
#define FDMANAGER_H_

#include <set>

class FdManager {
public:
	FdManager();
	virtual ~FdManager();

	static int allocFd() {
		int fd = 3;
		while (_fdPool.find(fd) != _fdPool.end())
			fd++;

		_fdPool.insert(fd);

		return fd;
	}

	static void recycleFd(int fd) {
		_fdPool.erase(fd);
	}

private:
	static std::set<int> _fdPool;
};

#endif /* FDMANAGER_H_ */
