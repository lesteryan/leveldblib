
#include "Reference.h"
#include "NaviGuard.h"

Reference::Reference()
               :_nReferenceNum(0)
{
}

Reference::~Reference()
{
}

void Reference::reset(void)
{
    NaviGuard oGuard(_Lock);

    _nReferenceNum = 0;
}

int Reference::referenceNum(void)
{
    NaviGuard oGuard(_Lock);
    return _nReferenceNum;
}

void Reference::increase(void)
{
    NaviGuard oGuard(_Lock);
    _nReferenceNum++;
}

void Reference::decrease(void)
{
    NaviGuard oGuard(_Lock);
    _nReferenceNum--;
}


