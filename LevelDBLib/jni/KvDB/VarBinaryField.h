#ifndef _VAR_BINARY_FIELD_H_
#define _VAR_BINARY_FIELD_H_
#include "TextType.h"
namespace kvdbEngine
{

class VarBinaryField :
	public TextType
{
public:
	VarBinaryField(string name);
	~VarBinaryField(void);
};

}
#endif
