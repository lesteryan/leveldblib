
#ifndef __REFERENCE_H__
#define __REFERENCE_H__

#include "NaviLock.h"

class Reference
{
public:
    Reference();
    ~Reference();

    void reset(void);

    int referenceNum(void);

    void increase(void);

    void decrease(void);

private:
    NaviLock _Lock;
    
    int  _nReferenceNum;
};

#endif //  __REFERENCE_H__


