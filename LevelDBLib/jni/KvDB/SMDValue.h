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
		// ��ȡ2�ֽ�����
		static short getShort(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// �洢2�ֽ�����
		static void putShort(short shortValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// ��ȡ4�ֽ�����
		static int getInt(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// �洢4�ֽ�����
		static void putInt(int intValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// ��ȡ8�ֽ�����
		static long long getLong(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// �洢8�ֽ�����
		static void putLong(long long longValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// ��ȡ˫���ȸ���
		static double getDouble(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// ����˫���ȸ���
		static void putDouble(double doubleValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// ��ȡ�����ȸ���
		static float getFloat(const unsigned char *buf, int byteOrder = ENDIAN_BIG);

		// ���õ����ȸ���
		static void putFloat(float floatValue, unsigned char *buf, int byteOrder = ENDIAN_BIG);
	};
}

#endif

