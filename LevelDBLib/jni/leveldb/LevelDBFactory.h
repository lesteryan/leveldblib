//
//  LevelDBFactory.h
//  libdataengine
//
//  Created by yanxi on 16/8/15.
//  Copyright © 2016年 sogou. All rights reserved.
//
//
//  为解决单进程内同一leveldb文件被多个leveldb实例打开而造成的打开失败，创建leveldb工厂类对leveldb实例进行管理，防止同一leveldb文件被多次打开
//
//

#ifndef LevelDBFactory_H__
#define LevelDBFactory_H__

#include <vector>
#include "LevelDB.h"
#include "NaviLock.h"
#include "NaviGuard.h"

namespace leveldb_navi {
    
    class LevelDBFactory
    {
    public:
        static LevelDB* create(const std::string& _dbPath); //所有打开leveldb均应通过此接口而不是直接构造leveldb
        static void remove(LevelDB* leveldb);   //所有对leveldb的关闭操作均应执行此接口而不是直接close leveldb
        
    private:
        LevelDBFactory();
        ~LevelDBFactory();
        static NaviLock naviLock;
        static std::vector<LevelDB *> leveldbs;
    };
}

#endif /* LevelDBFactory_H__ */
