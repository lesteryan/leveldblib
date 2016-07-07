#ifndef _SMD_SSTORAGE_H_
#define _SMD_SSTORAGE_H_

#include <stdio.h>
#include <string>
#include <vector>

#include "SMDMetaSegment.h"
#include "SMDDataSegment.h"
#include "SMDIndexSegment.h"
#include "SMDTableSegment.h"
#include "SMDPageManagement.h"
#include "SMDIndexSegment.h"

namespace kvdbEngine
{
	class SMDTable
	{
	public:
		friend class SMDPageManagement;

		// 构造
		SMDTable();

		// 析构
		~SMDTable();

		// 打开Table文件
		// @path 文件路径,不存在会创建
		// @return 返回打开table是否成功
		bool open(const std::string& path,unsigned short pageSize=4096);

		// 打开Table文件
		// @path 文件路径,不存在会创建
		// @return 返回打开table是否成功
		bool open(FILE* file,int offset,unsigned short pageSize=4096,bool exists = true);

		// 关闭Table文件
		// @return 关闭是否成功
		bool close();

		// 保存Table文件
		// @return 返回保存table是否成功
		bool save();

		// 添加Meta信息
		bool addMetaInfo(const std::string& key,const std::string& val);

		// 获取Meta信息
		std::string getMetaInfo(const std::string& key) const;

		// 添加表结构信息
		bool addTableStruct(TableDef *p);

		//获取表结构信息 
		TableDef * getTableDef(const string & name);
		TableDef * getTableDef(int iIndex);

		//获取记录 
		bool addOneGroupRecords(vector<Record *> records, const string & name);


		vector<Record *> getOneGroupRecords( unsigned int offset,const string &tableName); //获取一组record	


		bool add(const string & name,Record * r,int key);
		bool add(const string & name,Record * r,const std::string& key);
		//添加一个Key的范围
		bool addGroup(const string & name,std::vector<Record*> r,int keyS,int KeyE);


		Record * search(const string & name,int key);
		Record * search(const string & name,const std::string& key);

		std::vector<Record*> searchGroup(const string & name,int key);

		// 获取所有记录
		vector<Record* > getRecordAll(const string& tableName);

	protected:
		bool save(const void* buffer,unsigned int sz,unsigned int offset);

		bool read(void* buffer,unsigned int sz,unsigned int offset);

	private:
		//增加一条记录到表中
		Record * getRecord( unsigned int offset,const string &tableName ); //获取一个record

		// 保存文件头标识
		// @return 保存是否成功
		bool saveFlag(unsigned short pageSize);

		// 验证文件头标识
		// @return 验证是否成功
		bool checkFlag();

	private:
		// 移动文件到指定位置
		bool seekFileTo(int position);

	private:
		// 文件指针
		FILE* file;

		// 文件偏移量,-1表示使用path打开
		int offset;

		// 页面管理类
		SMDPageManagement* pageMgr;

		// 元信息
		SMDMetaSegment* segMeta;

		// 数据
		SMDDataSegment* segData;

		// 表结构
		SMDTableSegment* segTable;

		// 索引
		SMDIndexSegment* segIndex;
	};
}

#endif
