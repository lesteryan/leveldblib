#include "BinaryField.h"

namespace kvdbEngine
{

BinaryField::BinaryField(const string &name,int length)
{
	init(name,FIELD_TYPE_BINARY,name);
	this->length = length;
	//≥ı ºªØbinary
}


BinaryField::~BinaryField(void)
{
	
}

}

