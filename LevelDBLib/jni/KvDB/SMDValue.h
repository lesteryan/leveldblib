#ifndef _SMD_VALUE_H
#define _SMD_VALUE_H

#include <string.h>
#include <assert.h>

namespace kvdbEngine
{
#define ENDIAN_BIG	  (0)
#define ENDIAN_LITTLE (1)

	class SMDValue
	{
	public:
		// 获取2字节整数
		static short getShort(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 存储2字节整数
		static void putShort(short shortValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 获取4字节整数
		static int getInt(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 存储4字节整数
		static void putInt(int intValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 获取8字节整数
		static long long getLong(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 存储8字节整数
		static void putLong(long long longValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 获取双精度浮点
		static double getDouble(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 设置双精度浮点
		static void putDouble(double doubleValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 获取单精度浮点
		static float getFloat(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// 设置单精度浮点
		static void putFloat(float floatValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);
	};
}

#endif

