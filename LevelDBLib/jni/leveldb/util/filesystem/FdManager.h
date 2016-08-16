/*
 * FdManager.h
 *
 *  Created on: 2016年7月15日
 *      Author: yanxi
 */

#ifndef FDMANAGER_H_
#define FDMANAGER_H_

#include <set>

namespace leveldb_navi {

class FdManager {
public:
    FdManager(){}
    virtual ~FdManager(){}

    static int allocFd() ;

    static void recycleFd(int fd);

private:
	static std::set<int> _fdPool;
};
    
}
#endif /* FDMANAGER_H_ */
