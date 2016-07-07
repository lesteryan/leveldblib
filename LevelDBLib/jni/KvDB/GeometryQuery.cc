#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/time.h>

using std::string;
using std::vector;
using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;

#include "SMDIndexSegment.h"
#include "SMDSStorage.h"
#include "SMDDataSegment.h"
#include "TableDef.h"
#include "ShortField.h"
#include "LongField.h"
#include "FloatField.h"
#include "DoubleField.h"
#include "TextField.h"
#include "BinaryField.h"
#include "VarIntField.h"
#include "IntegerField.h"
#include "VarTextField.h"
#include "VarBinaryField.h"
#include "FLDoubleField.h"

#include "MyStream.h"
#include "GeometryQuery.h"


long gettime(){
    timeval time;
    gettimeofday(&time, NULL);

    return time.tv_usec;
    // return time.tv_sec * 1000 + time.tv_usec / 1000;
}

inline long us_sub(long begin, long end){
    if (end > begin) {
        return (end - begin);
    }
    else{
        return (1000000 - begin + end);
    }
}

GeometryQuery::GeometryQuery(const std::string& strSmdTabPath)
             : _strSmdTabPath(strSmdTabPath), _strTabName(""), _bIsOpen(false)
{
}

bool GeometryQuery::open(void) {
    if (_bIsOpen){
        return true;
    }

    if (!_smdTab.open(_strSmdTabPath)) {
        return false;
    }

    std::size_t nStartPos = _strSmdTabPath.find_last_of("/");
    if (std::string::npos == nStartPos) {
        return false;
    }
    else {
        std::size_t nEndPos = _strSmdTabPath.find_last_of(".");
        if (std::string::npos == nStartPos) {
            return false;
        }
        _strTabName = _strSmdTabPath.substr(nStartPos+1, nEndPos - nStartPos - 1);
    }

    _bIsOpen = true;
    return true;
}

bool GeometryQuery::close(void) {
    _smdTab.close();
    _bIsOpen = false;
    return _bIsOpen;
}

bool GeometryQuery::getGeometryByRoadId(int nRoadId, std::vector<int>& vecGeometry) {
    if (!_bIsOpen) {
        return false;
    }

    Record* rd2 = _smdTab.search(_strTabName, nRoadId);
    if (rd2 == NULL) {
        // cout<<"false to find:"<<nRoadId<<endl;

        return false;
    }

    int nSize = 0;
    char *pcBuf = NULL;
    rd2->getFieldAsBinary(0, pcBuf, nSize);
    MyStream oStream(pcBuf, nSize);

    int nFirX = 0, nFirY = 0, nCount = 0;
    int arrayPoint[2] = {0};
    bool bFirstPoint = true;
    while (oStream.rest_size() != 0) {
        arrayPoint[nCount++ % 2] = oStream.readVarInt();
        if (nCount % 2 == 0) {
            if (bFirstPoint) {
                bFirstPoint = false;
                nFirX = arrayPoint[0];
                nFirY = arrayPoint[1];
                vecGeometry.push_back(nFirX);
                vecGeometry.push_back(nFirY);
                // cout<<nFirX<<":"<<nFirY<<endl;
            }
            else {
                // cout<<arrayPoint[0] + nFirX<<":"<<arrayPoint[1] + nFirY<<endl;
                vecGeometry.push_back(arrayPoint[0] + nFirX);
                vecGeometry.push_back(arrayPoint[1] + nFirY);
            }
        }
    }
    free(pcBuf);
    delete rd2;
    return true;
}

