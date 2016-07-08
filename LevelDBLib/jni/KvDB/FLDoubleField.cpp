#include "FLDoubleField.h"

namespace kvdbEngine
{

FLDoubleField::FLDoubleField(const string &name, unsigned short scale)
{
	init(name,FIELD_TYPE_FLDOUBLE,name);
	setScale(scale);
}

FLDoubleField::~FLDoubleField(void)
{
}

}
