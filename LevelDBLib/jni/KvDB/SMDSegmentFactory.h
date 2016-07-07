#ifndef _SMD_SEGMENTFACTORY_H_
#define _SMD_SEGMENTFACTORY_H_

#include "SMDSegment.h"
#include "SMDIndexSegment.h"
#include "SMDMetaSegment.h"

namespace kvdbEngine
{
class SMDSegmentFactory{

public:
	static SMDSegment* createSegment(SegType type)
	{
		return NULL;
	}
};
}

#endif
