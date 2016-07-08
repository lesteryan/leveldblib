#include "Record.h"
#include "FieldType.h"
#include "TextField.h"
#include "VarTextField.h"
#include "BinaryField.h"
#include "VarBinaryField.h"
#include "VarIntField.h"
#include "FLDoubleField.h"

namespace kvdbEngine
{


Record::Record(TableDef * tableStruct)
{

	this ->tableStruct = tableStruct;
	//初始化Flag
	initFlagBit(tableStruct);

	//初始化FieldSet集合
	initFields(tableStruct);

}


Record::~Record(void)
{
	Release();
}

void Record::Release()
{
    NaviGuard guard(m_lock);
	//释放field
	int size = Fields.size();
	for (int i=0;i<size;i++)
	{
		Enum_FieldType fieldType = Fields[i].fieldType;
//		下边的逻辑走了continue会有字段内存不被释放 
// 		if (readBit(i) == 0)
// 		{
// 			continue;
// 		}
		switch(fieldType)
		{
		case FIELD_TYPE_TEXT:
		case FIELD_TYPE_VAR_TEXT:
		case FIELD_TYPE_BINARY:
		case FIELD_TYPE_VAR_BINARY:
                      // Modified by Kevin.L 2015/09/01 for memory leak
			// if (Fields[i].fieldValue.TextAndBinary.nLength != 0)
		       if (NULL != Fields[i].fieldValue.TextAndBinary.buffer)
			{
				free(Fields[i].fieldValue.TextAndBinary.buffer);
			}
			break;
		}
	}
	Fields.clear();
	if (fieldFlag != NULL)
	{
		free(fieldFlag);
	}
}
unsigned int Record::calcSize( unsigned int num )
{
	int a = num%8;
	int b = num/8;
	if (a>0)
	{
		return b+1;
	}
	else
		return b;
	return 0;
}

void Record::setBit( unsigned int bit )
{
	*fieldFlag = (*fieldFlag) | (0x1<<bit) ;
}

int Record::readBit( unsigned int bit )
{
	if( 0 != ( (*fieldFlag) & (0x1<<bit)))
	{
		return 1;
	}
	else
		return 0;
}

void Record::ClearBit(unsigned int bit)
{
	*fieldFlag = (*fieldFlag) &~ (0x1<<bit);
}

void Record::initBit(int size)
{
	for (int i = 0; i<size *8;i++)
	{
		ClearBit(i);
	}
}

void Record::initFlagBit(TableDef * tableStruct)
{
	//给标志位分配空间
	size = calcSize(tableStruct->getFieldCount());
	fieldFlag = (char *)(malloc(size));

	//初始化字节位
	initBit(size);
}

bool Record::initFields( TableDef * tableStruct )
{
    NaviGuard guard(m_lock);
    
	int size = tableStruct->getFieldCount();
	Fields.resize(size);

	for (int i=0;i<size;i++)
	{
		Enum_FieldType fieldType = tableStruct->getFieldType(i);
		Fields[i].fieldType = fieldType;

		switch(fieldType)
		{
		case FIELD_TYPE_TEXT:

			Fields[i].nFieldSize = ((TextField *)(tableStruct->getField(i)))->getLength();
			Fields[i].fieldValue.TextAndBinary.buffer = (char *)malloc(Fields[i].nFieldSize);
			Fields[i].fieldValue.TextAndBinary.nLength = Fields[i].nFieldSize;
			break;

		case FIELD_TYPE_VAR_TEXT:

			Fields[i].nFieldSize = 0;
			Fields[i].fieldValue.TextAndBinary.buffer = NULL;
			break;
		case FIELD_TYPE_BINARY:

			Fields[i].nFieldSize = ((BinaryField *)(tableStruct->getField(i)))->getLength();
			Fields[i].fieldValue.TextAndBinary.buffer = (char *)malloc(Fields[i].nFieldSize);
			Fields[i].fieldValue.TextAndBinary.nLength = Fields[i].nFieldSize;
			break;
		case FIELD_TYPE_VAR_BINARY:

			Fields[i].nFieldSize = 0;
			Fields[i].fieldValue.TextAndBinary.buffer = NULL;
			break;
		case FIELD_TYPE_FLOAT:
			Fields[i].nFieldSize = sizeof(float);
			break;
		case FIELD_TYPE_DOUBLE:
			Fields[i].nFieldSize = sizeof(double);
			break;
		case FIELD_TYPE_FLDOUBLE:
			Fields[i].fieldValue.FixedLengthDouble.nLength = ((FLDoubleField *)(tableStruct->getField(i)))->getScale();
			Fields[i].nFieldSize = sizeof(long long);
			break;
		case FIELD_TYPE_VAR_INT:
			Fields[i].nFieldSize = sizeof(int);
			break;
		case FIELD_TYPE_SHORT:
			Fields[i].nFieldSize = sizeof(short);
			break;
		case FIELD_TYPE_INT:
			Fields[i].nFieldSize = sizeof(int);
			break;
		case FIELD_TYPE_LONG:
			Fields[i].nFieldSize = sizeof(long);
			break;
		}
	}
	return true;
}

bool Record::setFieldShort( int iIndex,short value )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}

	if (Fields[iIndex].fieldType == FIELD_TYPE_SHORT)
	{
		Fields[iIndex].fieldValue.Short = value;
		setBit(iIndex);
		return true;
	}
	return false;
}

bool Record::setFieldInteger( int iIndex,int value, bool flag /*= true*/ )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_INT)
	{
		Fields[iIndex].fieldValue.Integer = value;
		setBit(iIndex);
		return true;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_VAR_INT)
	{

		FieldType * field = tableStruct->getField(iIndex);
		if (field->getIsHaveHead() && flag == true)
		{
			int size = this->getValueSize(value);
			FieldHead *head = field->getFieldHead();
			int sizehead = this->getValueSize(head->headValue.head_int);

			int a = value - head->headValue.head_int * pow(10.0,(size - sizehead));

			a += 1 * pow(10.0, size - sizehead);
			//变长整型计算
			Fields[iIndex].fieldValue.VarInteger.nCount = 0;
			int count = 0;

			varIntWrite(a,Fields[iIndex].fieldValue.VarInteger.data,count);
			Fields[iIndex].fieldValue.VarInteger.nCount = count;

			setBit(iIndex);
			return true;
		}
		else
		{
			Fields[iIndex].fieldValue.VarInteger.nCount = 0;
			int count = 0;

			varIntWrite(value,Fields[iIndex].fieldValue.VarInteger.data,count);
			Fields[iIndex].fieldValue.VarInteger.nCount = count;

			setBit(iIndex);
			return true;
		}
	}
	return false;
}

bool Record::setFieldLong( int iIndex,long long value, bool flag /*= true*/ )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_LONG)
	{
		Fields[iIndex].fieldValue.Long = value;
		setBit(iIndex);
		return true;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_VAR_INT)
	{
		FieldType * field = tableStruct->getField(iIndex);
		if (field->getIsHaveHead() && flag == true)
		{
			int size = this->getValueSize(value);
			FieldHead *head = field->getFieldHead();
			int sizehead = this->getValueSize(head->headValue.head_int);

			int a = value - head->headValue.head_int * pow(10.0,(size - sizehead));

			a += 1 * pow(10.0, size - sizehead );
			//变长整型计算
			Fields[iIndex].fieldValue.VarInteger.nCount = 0;
			int count = 0;

			varIntWrite(a,Fields[iIndex].fieldValue.VarInteger.data,count);
			Fields[iIndex].fieldValue.VarInteger.nCount = count;

			setBit(iIndex);
			return true;
		}
		else
		{
			Fields[iIndex].fieldValue.VarInteger.nCount = 0;
			int count = 0;

			varIntWrite(value,Fields[iIndex].fieldValue.VarInteger.data,count);
			Fields[iIndex].fieldValue.VarInteger.nCount = count;

			setBit(iIndex);
			return true;
		}
	}
	return false;
}

bool Record::setFieldFloat( int iIndex,float value )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_FLOAT)
	{
		Fields[iIndex].fieldValue.Float = value;
		setBit(iIndex);
		return true;
	}
	return false;
}

bool Record::setFieldDouble( int iIndex,double value )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_DOUBLE)
	{
		Fields[iIndex].fieldValue.Double = value;
		setBit(iIndex);
		return true;
	}
	return false;
}

bool Record::setFieldFLDouble( int iIndex,double value)
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_FLDOUBLE)
	{
		int scale =  Fields[iIndex].fieldValue.FixedLengthDouble.nLength;
		//计算值
		long long value1;
		value1 = static_cast<long long>(value * pow(10.0,scale));

		//四舍五入
		long long num = static_cast<long long>(value * pow(10.0,scale+1));
		long long num1 = 10 * (num/10);
		if (num - num1 >4)
		{
			value1 += 1;
		}
		Fields[iIndex].fieldValue.FixedLengthDouble.FLDouble = value1;
		setBit(iIndex);
		return true;
	}

	return false;
}

bool Record::setFieldBinary( int iIndex,char * buffer,int size )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_TEXT)
	{
		int n = Fields[iIndex].nFieldSize;        
		char *tmp = Fields[iIndex].fieldValue.TextAndBinary.buffer;
		//拷贝length长度内的串
		while (n--&&((*tmp++ = *buffer++) != '\0'));   

		//设定长度与结束符号
		if (size < Fields[iIndex].nFieldSize)
			Fields[iIndex].fieldValue.TextAndBinary.nLength = size;
		else 
			Fields[iIndex].fieldValue.TextAndBinary.nLength = Fields[iIndex].nFieldSize;

		setBit(iIndex);
		return true;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_VAR_TEXT)
	{

		int n = size;    
		Fields[iIndex].nFieldSize = size;
		Fields[iIndex].fieldValue.TextAndBinary.nLength = size;
		if (Fields[iIndex].fieldValue.TextAndBinary.buffer != NULL)
		{
			free(Fields[iIndex].fieldValue.TextAndBinary.buffer);
		}
		Fields[iIndex].fieldValue.TextAndBinary.buffer = (char *)malloc(size);

		char *tmp = Fields[iIndex].fieldValue.TextAndBinary.buffer;
		//拷贝length长度内的串
		while (n--&&((*tmp++ = *buffer++) != '\0'));
		setBit(iIndex);
		return true;
	}

	if (Fields[iIndex].fieldType == FIELD_TYPE_BINARY)
	{      
		char *tmp = Fields[iIndex].fieldValue.TextAndBinary.buffer;
		memset(tmp, 0, Fields[iIndex].nFieldSize);

		if (size < Fields[iIndex].nFieldSize)
		{
			Fields[iIndex].fieldValue.TextAndBinary.nLength = size;
			memcpy(tmp,buffer,size);
		}
		else
		{
			Fields[iIndex].fieldValue.TextAndBinary.nLength = Fields[iIndex].nFieldSize;
			memcpy(tmp,buffer,Fields[iIndex].nFieldSize);
		}
		setBit(iIndex);
		return true;
	}

	if (Fields[iIndex].fieldType == FIELD_TYPE_VAR_BINARY)
	{      
		char *tmp = Fields[iIndex].fieldValue.TextAndBinary.buffer;
		if (tmp != NULL)
		{
			free(tmp);
		}
		Fields[iIndex].fieldValue.TextAndBinary.buffer = (char *) malloc(size);
		tmp = Fields[iIndex].fieldValue.TextAndBinary.buffer;
		
		Fields[iIndex].fieldValue.TextAndBinary.nLength = size;
		memcpy(tmp,buffer,size);
		setBit(iIndex);
		return true;
	}
	return false;
}

bool Record::IsOverFlow( int iIndex )
{
	if (iIndex < 0 || iIndex >= Fields.size())
	{
		return true;
	}
	return false;
}

unsigned int Record::Zig( int value )
{
	return (unsigned int)((value << 1) ^ (value >> 31));
}

int Record::Zag( unsigned int ziggedValue )
{
	int value = (int)ziggedValue;
	return (-(value & 0x01)) ^ ((value >> 1) & ~( 1<< 31));
}

unsigned char* Record::varIntWrite( int value1 ,unsigned char* data_byte,int & count )
{
	unsigned int value = Zig(value1);
	count = 0;
	do
	{
		data_byte[count] = (byte)((value & 0x7F) | 0x80);
		//data_byte.push_back(data);
		count++;
	} while ((value >>= 7) != 0);

	data_byte[count-1] &= 0x7F;
	return data_byte;
}

int Record::getVarInt(int iIndex )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return 0;
	}
	Field field = Fields[iIndex];
	if(field.fieldType == FIELD_TYPE_VAR_INT)
	{
		int nCount = field.fieldValue.VarInteger.nCount;
		if ( 0 == nCount)
		{
			return  0;
		}
		//可以重写为一个循环实现
		unsigned int value = field.fieldValue.VarInteger.data[0];

		if ((value & 0x80) == 0) 
		{
			int v = Zag(value);
			return v;
		}
		value &= 0x7F;

		unsigned int chunk = field.fieldValue.VarInteger.data[1];

		value |= (chunk & 0x7F) << 7;

		if ((chunk & 0x80) == 0)
		{
			int v = Zag(value);
			return v;
		}

		chunk = field.fieldValue.VarInteger.data[2];

		value |= (chunk & 0x7F) << 14;


		if ((chunk & 0x80) == 0) 
		{
			int v = Zag(value);
			return v;
		}

		chunk = field.fieldValue.VarInteger.data[3];

		value |= (chunk & 0x7F) << 21;


		if ((chunk & 0x80) == 0) 
		{
			int v = Zag(value);
			return v;
		}

		chunk = field.fieldValue.VarInteger.data[4];

		value |= chunk << 28;


		if ((chunk & 0x80) == 0) 
		{
			int v = Zag(value);
			return v;
		}
		return value;
	}
	return 0;
}

Field * Record::getField( int iIndex )  //dangerous!!!
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return NULL;
	}
	return &Fields[iIndex];
}

bool Record::write( SMDStream& stream )
{
    NaviGuard guard(m_lock);
    
	//stream.writeBytes(fieldFlag,size);
	//一次写入SMDStream
	for (int i=0;i<Fields.size();i++)
	{
		//if (readBit(i) == 0)
		//{
		//	continue;
		//}
		Enum_FieldType fieldType = Fields[i].fieldType;
		switch(fieldType)
		{
		case FIELD_TYPE_TEXT:
			stream.writeInt(Fields[i].fieldValue.TextAndBinary.nLength);
			stream.writeBytes(Fields[i].fieldValue.TextAndBinary.buffer,Fields[i].fieldValue.TextAndBinary.nLength);
			break;

		case FIELD_TYPE_VAR_TEXT:
			stream.writeString(Fields[i].fieldValue.TextAndBinary.buffer,Fields[i].fieldValue.TextAndBinary.nLength);
			break;
		case FIELD_TYPE_BINARY:
			stream.writeInt(Fields[i].fieldValue.TextAndBinary.nLength);
			stream.writeBytes(Fields[i].fieldValue.TextAndBinary.buffer,Fields[i].fieldValue.TextAndBinary.nLength);
			break;
		case FIELD_TYPE_VAR_BINARY:
			stream.writeInt(Fields[i].fieldValue.TextAndBinary.nLength);
			stream.writeBytes(Fields[i].fieldValue.TextAndBinary.buffer,Fields[i].fieldValue.TextAndBinary.nLength);
			break;
		case FIELD_TYPE_FLOAT:
			stream.writeFloat(Fields[i].fieldValue.Float);
			break;
		case FIELD_TYPE_DOUBLE:
			stream.writeDouble(Fields[i].fieldValue.Double);
			break;
		case FIELD_TYPE_FLDOUBLE:
			stream.writeLong(Fields[i].fieldValue.FixedLengthDouble.FLDouble);		
			break;
		case FIELD_TYPE_VAR_INT:
			stream.writeBytes(Fields[i].fieldValue.VarInteger.data,Fields[i].fieldValue.VarInteger.nCount);
			break;
		case FIELD_TYPE_SHORT:
			stream.writeShort(Fields[i].fieldValue.Short);
			break;
		case FIELD_TYPE_INT:
			stream.writeInt(Fields[i].fieldValue.Integer);
			break;
		case FIELD_TYPE_LONG:
			stream.writeLong(Fields[i].fieldValue.Long);
			break;
		}
	}
	return true;
}

bool Record::read( SMDStream& stream )
{
    NaviGuard guard(m_lock);
    
	if (!stream.size())
	{
		return false;
	}
	//标记为读入
	//stream.readBytes(fieldFlag,size);
	//一次写入SMDStream
	for (int i=0;i<Fields.size();i++)
	{
// 		if (readBit(i) == 0)
// 		{
// 			continue;
// 		}
		Enum_FieldType fieldType = Fields[i].fieldType;

		string str = "";
		int n = 0;
		int size =0;
		switch(fieldType)
		{
		case FIELD_TYPE_TEXT:
			memset(Fields[i].fieldValue.TextAndBinary.buffer,0,Fields[i].nFieldSize);
			stream.readString(str);
			n = Fields[i].nFieldSize;
			size = str.size();
			if (size < Fields[i].nFieldSize)
				Fields[i].fieldValue.TextAndBinary.nLength = size;
			else 
				Fields[i].fieldValue.TextAndBinary.nLength = Fields[i].nFieldSize;

			str.copy(Fields[i].fieldValue.TextAndBinary.buffer,min(n,size),0);
			setBit(i);
			break;

		case FIELD_TYPE_VAR_TEXT:
			stream.readString(str);
			if (Fields[i].fieldValue.TextAndBinary.buffer)
			{
				free(Fields[i].fieldValue.TextAndBinary.buffer);
			}
			Fields[i].fieldValue.TextAndBinary.buffer = (char * )malloc(str.size());
			Fields[i].nFieldSize = str.size();
			Fields[i].fieldValue.TextAndBinary.nLength = str.size();
			str.copy(Fields[i].fieldValue.TextAndBinary.buffer,str.size(),0);
			setBit(i);
			break;
		case FIELD_TYPE_BINARY:
			memset(Fields[i].fieldValue.TextAndBinary.buffer,0,Fields[i].nFieldSize);
			n = Fields[i].nFieldSize;
			stream.readInt(size);
			if (size < Fields[i].nFieldSize)
			{
				stream.readBytes(Fields[i].fieldValue.TextAndBinary.buffer,size);
				Fields[i].fieldValue.TextAndBinary.nLength = size;
			}
			else
			{
				stream.readBytes(Fields[i].fieldValue.TextAndBinary.buffer,Fields[i].nFieldSize);
				Fields[i].fieldValue.TextAndBinary.nLength = Fields[i].nFieldSize;
			}
			setBit(i);
			break;

		case FIELD_TYPE_VAR_BINARY:
			if (Fields[i].fieldValue.TextAndBinary.buffer)
			{
				free(Fields[i].fieldValue.TextAndBinary.buffer);
			}
			stream.readInt(Fields[i].fieldValue.TextAndBinary.nLength );
			Fields[i].fieldValue.TextAndBinary.buffer = (char * )malloc(Fields[i].fieldValue.TextAndBinary.nLength);
			stream.readBytes(Fields[i].fieldValue.TextAndBinary.buffer,Fields[i].fieldValue.TextAndBinary.nLength);
			Fields[i].nFieldSize = Fields[i].fieldValue.TextAndBinary.nLength;
			setBit(i);
			break;
		case FIELD_TYPE_FLOAT:
			stream.readFloat(Fields[i].fieldValue.Float);
			setBit(i);
			break;
		case FIELD_TYPE_DOUBLE:
			stream.readDouble(Fields[i].fieldValue.Double);
			setBit(i);
			break;
		case FIELD_TYPE_FLDOUBLE:
			stream.readLong(Fields[i].fieldValue.FixedLengthDouble.FLDouble);		
			setBit(i);
			break;
		case FIELD_TYPE_VAR_INT:
			readVarInt(stream,i);
			setBit(i);
			break;
		case FIELD_TYPE_SHORT:
			stream.readShort(Fields[i].fieldValue.Short);
			setBit(i);
			break;
		case FIELD_TYPE_INT:
			stream.readInt(Fields[i].fieldValue.Integer);
			setBit(i);
			break;
		case FIELD_TYPE_LONG:
			stream.readLong(Fields[i].fieldValue.Long);
			setBit(i);
			break;
		}
	}
	return true;
}

bool Record::setFieldString( int iIndex,const string & str )
{
	return setFieldBinary(iIndex,(char *)str.c_str(),str.size());
}

bool Record::readVarInt( SMDStream& stream,int iIndex )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	unsigned char  buffer[5]={0};
	for (int i =0 ;i<5;i++)
	{
		stream.readBytes(buffer+i,1);
		Fields[iIndex].fieldValue.VarInteger.data[i] = buffer[i];
		unsigned int value = Fields[iIndex].fieldValue.VarInteger.data[i];
		if (value < 128)
		{
			Fields[iIndex].fieldValue.VarInteger.nCount = i+1;
			return true;
		}
	}

	return false;
}

bool Record::getFieldAsShort( int iIndex, short &sval )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	Enum_FieldType fieldType = Fields[iIndex].fieldType;
	if ( fieldType == FIELD_TYPE_SHORT )
	{
		sval= Fields[iIndex].fieldValue.Short;
		return true;
	}
	return false;
}

bool Record::getFieldAsInteger( int iIndex, int & ival )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	Enum_FieldType fieldType = Fields[iIndex].fieldType;
	if ( fieldType == FIELD_TYPE_INT)
	{
		ival = Fields[iIndex].fieldValue.Integer;
		return true;
	}
	if ( fieldType == FIELD_TYPE_VAR_INT)
	{
		ival = getVarInt(iIndex);
		//FieldType * field = tableStruct->getField(iIndex);
		/*if (field->getIsHaveHead())
		{
			int size = this->getValueSize(ival);
			ival -=  1 * pow(10.0,size-1);
			FieldHead *head = field->getFieldHead();
			int sizehead = this->getValueSize(head->headValue.head_int);
			int a = head->headValue.head_int * pow(10.0,(size - 1)) + ival;
			ival = a;
		}*/
		return true;
	}
	return false;
}

bool Record::getFieldAsLong( int iIndex, long long & lval )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	Enum_FieldType fieldType = Fields[iIndex].fieldType;
	if ( fieldType == FIELD_TYPE_LONG)
	{
		lval = Fields[iIndex].fieldValue.Long;
		return true;
	}
	if ( fieldType == FIELD_TYPE_VAR_INT)
	{
		lval = getVarInt(iIndex);
		FieldType * field = tableStruct->getField(iIndex);
		if (field->getIsHaveHead())
		{
			int size = this->getValueSize(lval);
			lval -=  1 * pow(10.0,size-1);
			FieldHead *head = field->getFieldHead();
			//int sizehead = this->getValueSize(head->headValue.head_int);
			long long a = head->headValue.head_int * pow(10.0,(size - 1)) + lval;
			lval = a;
		}
		return true;
	}
	return false;
}

bool Record::getFieldAsFloat( int iIndex, float &fval )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	Enum_FieldType fieldType = Fields[iIndex].fieldType;
	if ( fieldType == FIELD_TYPE_FLOAT)
	{
		fval = Fields[iIndex].fieldValue.Float;
		return true;
	}
	return false;
}

bool Record::getFieldAsDouble( int iIndex, double &dval )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	Enum_FieldType fieldType = Fields[iIndex].fieldType;
	if ( fieldType == FIELD_TYPE_DOUBLE)
	{
		dval = Fields[iIndex].fieldValue.Double;
		return true;
	}
	if ( fieldType == FIELD_TYPE_FLDOUBLE)
	{
		dval = 1.0 * Fields[iIndex].fieldValue.FixedLengthDouble.FLDouble/pow(10.0,Fields[iIndex].fieldValue.FixedLengthDouble.nLength);
		return true;
	}
	return false;
}

bool Record::getFieldAsString( int iIndex,string & buffer )
{
    NaviGuard guard(m_lock);
    
	if (IsOverFlow(iIndex))
	{
		return false;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_TEXT || Fields[iIndex].fieldType == FIELD_TYPE_VAR_TEXT)
	{
		int n = Fields[iIndex].fieldValue.TextAndBinary.nLength;        
		char *tmp = Fields[iIndex].fieldValue.TextAndBinary.buffer;
		buffer.resize(n);
		char *tmp1 = (char *)buffer.c_str();
		//拷贝length长度内的串
		while (n--&&((*tmp1++ = *tmp++) != '\0'));   
		*tmp1 = '\0';
		return true;
	}
	if (Fields[iIndex].fieldType == FIELD_TYPE_BINARY || Fields[iIndex].fieldType == FIELD_TYPE_VAR_BINARY)
	{      
		int n = Fields[iIndex].fieldValue.TextAndBinary.nLength; 
		buffer.resize(n);
		char *tmp = Fields[iIndex].fieldValue.TextAndBinary.buffer;
		char *tmp1 = (char *)buffer.c_str();
		//memset(tmp1, 0, n);
		memcpy(tmp1,tmp,n);
		return true;
	}
	return false;
}


bool Record::getFieldAsBinary( int iIndex,char *& buffer,int &size )
{
//    NaviGuard guard(m_lock);
    
	string str = "";
	getFieldAsString(iIndex,str);
	buffer = (char *)malloc(str.size());
	memcpy(buffer,str.c_str(),str.size());
	size = str.size();
	if (size == 0)
	{
		return false;
	}
	return true;
}

int Record::getValueSize( long long value )
{
	long long v_tmp = value;
	int nSize= 0;
	while(v_tmp != 0)
	{
		v_tmp /=10;
		nSize++;
	}
	return nSize;
}

Record * Record::subRecord( Record * p )
{
	Record * result = new Record(tableStruct);

	for (int i = 0;i<tableStruct->getFieldCount();i++)
	{
		Enum_FieldType type = tableStruct->getFieldType(i);
		switch(type)
		{
		case FIELD_TYPE_TEXT:
		case FIELD_TYPE_VAR_TEXT:
			{
				string text;
				this->getFieldAsString(i,text);
				result->setFieldString(i,text);
				break;
			}
		case FIELD_TYPE_BINARY:
		case FIELD_TYPE_VAR_BINARY:
			{
				string text;
				this->getFieldAsString(i,text);
				result->setFieldBinary(i,(char *)text.c_str(),text.size());
				break;
			}
		case FIELD_TYPE_FLOAT:
			{
				float fval,pfval;
				this->getFieldAsFloat(i,fval);
				p->getFieldAsFloat(i,pfval);
				result->setFieldFloat(i,fval - pfval);
				break;
			}
		case FIELD_TYPE_DOUBLE:
		case FIELD_TYPE_FLDOUBLE:
			{
				double dval,pdval;
				this->getFieldAsDouble(i,dval);
				p->getFieldAsDouble(i,pdval);
				result->setFieldDouble(i,dval - pdval);
				break;
			}
		case FIELD_TYPE_VAR_INT:
			{
				int ival,pival;
				this->getFieldAsInteger(i,ival);
				p->getFieldAsInteger(i,pival);
				result->setFieldInteger(i,ival - pival,false);
				break;
			}
		case FIELD_TYPE_SHORT:
			{
				short sval,psval;
				this->getFieldAsShort(i,sval);
				p->getFieldAsShort(i,psval);
				result->setFieldShort(i,sval - psval);
				break;
			}
		case FIELD_TYPE_INT:
			{
				int ival,pival;
				this->getFieldAsInteger(i,ival);
				p->getFieldAsInteger(i,pival);
				result->setFieldInteger(i,ival - pival,false);
				break;
			}

		case FIELD_TYPE_LONG:
			{
				long long lval,plval;
				this->getFieldAsLong(i,lval);
				p->getFieldAsLong(i,plval);
				result->setFieldLong(i,lval - plval,false);
				break;
			}
		}
	}
	return result;
}

void Record::addRecord( Record * p )
{
	for (int i = 0;i<tableStruct->getFieldCount();i++)
	{
		Enum_FieldType type = tableStruct->getFieldType(i);
		switch(type)
		{
		case FIELD_TYPE_TEXT:
		case FIELD_TYPE_VAR_TEXT:
		case FIELD_TYPE_BINARY:
		case FIELD_TYPE_VAR_BINARY:
		case FIELD_TYPE_FLOAT:
			{
				float fval,pfval;
				this->getFieldAsFloat(i,fval);
				p->getFieldAsFloat(i,pfval);
				this->setFieldFloat(i,fval + pfval);
				break;
			}
		case FIELD_TYPE_DOUBLE:
		case FIELD_TYPE_FLDOUBLE:
			{
				double dval,pdval;
				this->getFieldAsDouble(i,dval);
				p->getFieldAsDouble(i,pdval);
				this->setFieldDouble(i,dval + pdval);
				break;
			}
		case FIELD_TYPE_VAR_INT:
			{
				int ival,pival;
				this->getFieldAsInteger(i,ival);
				p->getFieldAsInteger(i,pival);
				this->setFieldInteger(i,ival + pival,false);
				break;
			}
		case FIELD_TYPE_SHORT:
			{
				short sval,psval;
				this->getFieldAsShort(i,sval);
				p->getFieldAsShort(i,psval);
				this->setFieldShort(i,sval + psval);
				break;
			}
		case FIELD_TYPE_INT:
			{
				int ival,pival;
				this->getFieldAsInteger(i,ival);
				p->getFieldAsInteger(i,pival);
				this->setFieldInteger(i,ival + pival,false);
				break;
			}

		case FIELD_TYPE_LONG:
			{
				long long lval,plval;
				this->getFieldAsLong(i,lval);
				p->getFieldAsLong(i,plval);
				this->setFieldLong(i,lval + plval,false);
				break;
			}
		}
	}
}
}
