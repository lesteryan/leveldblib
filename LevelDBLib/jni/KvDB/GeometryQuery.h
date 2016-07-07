
#ifndef __QUERYGEOMETRY_H__
#define __QUERYGEOMETRY_H__

#include "SMDSStorage.h"
using namespace kvdbEngine;

class GeometryQuery
{
public:
    GeometryQuery(const std::string& strSmdTabPath);

    bool open(void);

    bool close(void);

    bool getGeometryByRoadId(int nRoadId, std::vector<int>& vecGeometry);

private:
    std::string  _strSmdTabPath;

    bool         _bIsOpen;
    SMDTable     _smdTab;
    string       _strTabName;
};

#endif // __QUERYGEOMETRY_H__

