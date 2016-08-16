//
//  LevelDBFactory.cpp
//  libdataengine
//
//  Created by yanxi on 16/8/15.
//  Copyright © 2016年 sogou. All rights reserved.
//

#include "LevelDBFactory.h"
#include "util/PathParser.h"

std::vector<LevelDB*> LevelDBFactory::leveldbs;
NaviLock LevelDBFactory::naviLock;

LevelDBFactory::LevelDBFactory()
{
    
}

LevelDBFactory::~LevelDBFactory()
{
    for(int i = 0 ; i < leveldbs.size() ; i++)
    {
        if(leveldbs[i] != NULL)
            delete leveldbs[i];
        
    }
}

LevelDB* LevelDBFactory::create(const std::string& _dbPath)
{
    NaviGuard guard(naviLock);
    
    std::string dbPath, dbName;
    if(!PathParser::parseFilePath(_dbPath, dbPath, dbName))
        return NULL;
    
    for(int i = 0 ; i < leveldbs.size() ; i++)
        if(leveldbs[i]->comparePath(dbPath, dbName))
        {
            leveldbs[i]->reference.increase();
            return leveldbs[i];
        }
    
    LevelDB* leveldb = new LevelDB();
    Status statue = leveldb->open(_dbPath);
    
    if(statue.ok() == false)
        return NULL;
    
    leveldb->reference.increase();
    leveldbs.push_back(leveldb);
    
    return leveldb;
}

void LevelDBFactory::remove(LevelDB *leveldb)
{
    NaviGuard guard(naviLock);
    
    if(leveldb == NULL)
        return ;
    
    if(leveldb->reference.referenceNum() == 1)
    {
        for(std::vector<LevelDB*>::iterator iterator = leveldbs.begin() ; iterator != leveldbs.end() ; iterator++)
            if((*iterator) == leveldb)
            {
                delete (*iterator);
                leveldbs.erase(iterator);
                return ;
            }
    }
    else
        leveldb->reference.decrease();
}