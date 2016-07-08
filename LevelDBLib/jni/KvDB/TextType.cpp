#include "TextType.h"

namespace kvdbEngine
{

TextType::TextType(void)
{
	length = 0;
}


TextType::~TextType(void)
{

}

bool TextType::setLength(unsigned int unlength)
{
	length = unlength;
	return true;
}

unsigned int TextType::getLength()
{
	return length;
}

bool TextType::save( SMDStream& stream )
{
	return stream.writeInt(length);
}

bool TextType::readArr( SMDStream & stream )
{
	return stream.readInt(length);
}

}
