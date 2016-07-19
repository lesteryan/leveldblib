#include "FdManager.h"

std::set<int> FdManager::_fdPool;

int FdManager::allocFd() {
    int fd = 3;
    while (_fdPool.find(fd) != _fdPool.end())
        fd++;

    _fdPool.insert(fd);

    return fd;
}

void FdManager::recycleFd(int fd) {
    _fdPool.erase(fd);
}
