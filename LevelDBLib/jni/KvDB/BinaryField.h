#ifndef _BINARY_FIELD_H_
#define _BINARY_FIELD_H_

#include "TextType.h"

namespace kvdbEngine
{

class BinaryField :
	public TextType
{
public:
	BinaryField(const string &name,int length);
	~BinaryField(void);
};

}
#endif


