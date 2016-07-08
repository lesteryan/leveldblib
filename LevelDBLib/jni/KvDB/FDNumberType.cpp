#include "FDNumberType.h"

namespace kvdbEngine
{

FDNumberType::FDNumberType(void)
{
	this->scale = -1;
}


FDNumberType::~FDNumberType(void)
{
}


bool FDNumberType::setScale(short scale)
{
	this->scale = scale;
	return true;
}

short FDNumberType::getScale()
{
	return scale;
}

bool FDNumberType::save( SMDStream& stream )
{
	return stream.writeShort(scale);
}

bool FDNumberType::readArr( SMDStream & stream )
{
	return stream.readShort(scale);
}

}
