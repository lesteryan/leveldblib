#include <iostream>
#include <string.h>

typedef enum{
    BIG_ORDER,
    SMALL_ORDER
}EnumByteOrder;

#define DECODE_ZIG_ZAG(n) (((n) >> 1) ^ -((n) & 1))
#define ENCODE_ZIG_ZAG(n) ((n)  << 1) ^ ((n) >> 31)

class MyStream{
public:
    MyStream(int nBufSize, EnumByteOrder enumOrder = BIG_ORDER){
        int nCpu = 1;
        _enumCPU = (*((char *)&nCpu) == 1) ? SMALL_ORDER : BIG_ORDER;
        _enumCfg = enumOrder;

        _pcBuf = new unsigned char[nBufSize];
        _nReadPos = 0;
        _nWritePos = 0;
        _nMaxBufSize = nBufSize;
    }
    
    MyStream(char* pcBuf, int nSize){
        _pcBuf = (unsigned char *)pcBuf;
        _nReadPos = 0;
        _nWritePos = nSize;
        _nMaxBufSize = nSize;
    }

    void writeInt(int nVal){
        memcpy(_pcBuf + _nWritePos, (char*)&nVal, sizeof(int));
        _nWritePos += sizeof(int);
    }

    int readInt(void){
        if (_nWritePos <= 0){
            return 0;
        }
        int nVal = 0;
        memcpy((char*)&nVal, _pcBuf + _nReadPos, sizeof(int));
        _nReadPos += sizeof(int);

        return nVal;
    }

    void writeVarInt(int nVal){
        nVal = ENCODE_ZIG_ZAG(nVal);
        while (true){
            if ((nVal & ~0x7F) == 0){
                _pcBuf[_nWritePos++] = nVal;
                return;
            } 
            else {
                _pcBuf[_nWritePos++] = (char)((nVal & 0x7F) | 0x80);
                nVal >>= 7;
	    }
        }
    }

    int readVarInt(void){
        int result=0;
        char b = _pcBuf[_nReadPos++];
        result  = (b & 0x7F)      ; if (!(b & 0x80)) goto done;
        b = _pcBuf[_nReadPos++];
        result |= (b & 0x7F) <<  7; if (!(b & 0x80)) goto done;
        b = _pcBuf[_nReadPos++];
        result |= (b & 0x7F) << 14; if (!(b & 0x80)) goto done;
        b = _pcBuf[_nReadPos++];
        result |= (b & 0x7F) << 21; if (!(b & 0x80)) goto done;
        b = _pcBuf[_nReadPos++];
        result |= b          << 28; if (!(b & 0x80)) goto done;

    done:
        result = DECODE_ZIG_ZAG(result);
        return result;
    }

    void clear(void) {
        _nReadPos = 0;
        _nWritePos = 0;
        memset(_pcBuf, 0, _nMaxBufSize);
    }

    int size() const{
       return _nWritePos;
    }

    int rest_size() const{
       return _nWritePos - _nReadPos;
    }

    char* toCString(void){
        return (char *)_pcBuf;
    }

private:
   unsigned char* _pcBuf;
   int   _nMaxBufSize;
   int   _nReadPos;
   int   _nWritePos;

   EnumByteOrder _enumCPU;
   EnumByteOrder _enumCfg;
};

