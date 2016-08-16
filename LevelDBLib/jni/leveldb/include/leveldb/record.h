//
//  Record.h
//  libdataengine
//
//  Created by yanxi on 16/8/8.
//  Copyright © 2016年 sogou. All rights reserved.
//  使用LV结构进行序列化和反序列化

#ifndef Record_h
#define Record_h
#include <vector>

namespace leveldb_navi {
    
    class Record
    {
    public:
        Record()
        {
        }
        
        Record(const unsigned char *data, size_t length)
        {
            size_t index = 0;
            
            while(index < length)
            {
                size_t _length;
                
                if(data[index] & 0x80)
                {
                    //_length = arr2int(data + index + 1, data[index] & 0x7f);
                    //index += ((data[index] & 0x7f) + 1);
                    //旧数据有bug，暂时使用下面这种方式，等待新数据
                    _length = arr2int(data + index + 1, (data[index] & 0x7f) - 1);
                    index += ((data[index] & 0x7f));
                }
                else
                {
                    _length = data[index];
                    index ++;
                }
                
                if(index + _length > length)
                    break;
                
                records.push_back(std::string((char*)data + index, _length));
                index += _length;
            }
        }
        
        ~Record()
        {
            //        for(std::vector<std::string>::iterator iterator = records.begin() ; iterator != records.end()  ; iterator++)
            //        {
            //            std::string data(*iterator);
            //            printHexArray(data.data(), data.size());
            //        }
        }
        
        void put(const std::string& value)
        {
            records.push_back(value);
        }
        
        bool get(char index, std::string& value)
        {
            if(index >= records.size())
                return false;
            
            value = records[index];
            return true;
        }
        
        size_t size()
        {
            return records.size();
        }
        
        void serialized(std::string& buffer)
        {
            for(std::vector<std::string>::iterator iterator = records.begin() ; iterator != records.end()  ; iterator++)
            {
                std::string value = std::string(*iterator);
                size_t tlvlen = value.size();
                size_t lenlen, temp;
                
                if(value.size() < 0x80)
                    lenlen = 1;
                else if(value.size() < 0xff)
                    lenlen = 2;
                else if(value.size() < 0xffff)
                    lenlen = 3;
                else
                    lenlen = 4;
                
                tlvlen += lenlen;
                
                
                char *tlv = (char*)malloc(tlvlen);
                
                if(lenlen == 1)
                    tlv[0] = value.size();
                else
                {
                    temp = lenlen - 1;
                    tlv[0] = 0x80 | temp;
                    int2arr(value.size(), tlv + 1, temp);
                }
                
                memcpy(tlv + lenlen, value.data(), value.size());
                
                buffer.append(std::string(tlv, tlvlen));
                
                free(tlv);
            }
            
            //        printHexArray(buffer.data(), buffer.size());
        }
    private:
        std::vector<std::string> records;
        
        size_t arr2int(const unsigned char * data, int len)
        {
            size_t value = 0;
            for(int i = 0 ; i < len ; i++)
                value = (value << 8) | (data[i] & 0xff);
            
            return value;
        }
        
        void int2arr(size_t data, char* dest, size_t& len)
        {
            for(int i = 0 ; i < len ; i++)
                dest[i] = (char)(((data >> (len - i - 1) * 8)) & 0xff);
        }
        
        //    void printHexArray(const char * array, int len)
        //    {
        //        for(int i = 0 ; i < len ; i++)
        //            printf("%2x ",array[i]);
        
        //        printf("\n");
        //    }
    };
}


#endif /* Record_h */