#include "DoubleField.h"

namespace kvdbEngine
{

DoubleField::DoubleField(const string &name)
{
	init(name,FIELD_TYPE_DOUBLE,name);
}


DoubleField::~DoubleField(void)
{

}


}
