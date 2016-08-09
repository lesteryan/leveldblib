#include "LevelDB.h"
#include "util/PathParser.h"

LevelDB::LevelDB():db(NULL),isAtomic(false)
{
}

LevelDB::~LevelDB()
{
	close();
}

Status LevelDB::open(const std::string& _dbPath, const std::string& _dbName, bool _readOnly, bool _writeOnly)
{
	option.create_if_missing = _readOnly ? false : true;
	option.read_only = _readOnly;
	option.write_only = _writeOnly;

	Status statue = DB::Open(option, _dbPath, _dbName, &db);

	if(statue.ok())
	{
		dbPath = _dbPath;
		dbName = _dbName;
	}
	else
	{
		db = NULL;
	}

	return statue;
}

Status LevelDB::open(const std::string& _dbPath)
{
	std::string dbPath, dbName;
	if(!PathParser::parseFilePath(_dbPath, dbPath, dbName))
		return Status::InvalidArgument("path is invalid");

	return open(dbPath, dbName, true, false);
}

bool LevelDB::isOpen()
{
	return db != NULL;
}

Status LevelDB::insert(const Slice& key, const Slice& value)
{
	if(db == NULL)
		return Status::IOError("database is not open");
//    else if(key.size() != KEY_LENGTH)
//		return Status::InvalidArgument("key length invalid");

    if(isAtomic)
    {
        batch.Put(key, value);
        return Status::OK();
    }
    else
    {
        return db->Put(WriteOptions(), key, value);
    }
}

Status LevelDB::insert(const unsigned short type, const unsigned int id, const Slice& value)
{
    char tmpArray[6];
    makeKey(type, id, tmpArray);

    return insert(Slice(tmpArray, sizeof(tmpArray)/sizeof(char)), value);
}

//Status LevelDB::insert(const unsigned short type, const unsigned int id, const int index, const Slice& value)
//{
//    char tmpArray[7];
//    makeKey(type, id, index, tmpArray);
//
//    return insert(Slice(tmpArray, sizeof(tmpArray)/sizeof(char)), value);
//}

Status LevelDB::atomReady()
{
	if(db == NULL)
		return Status::IOError("database is not open");

	batch.Clear();
    isAtomic = true;

	return Status::OK();
}

Status LevelDB::atomCommit()
{
	if(db == NULL)
		return Status::IOError("database is not open");
    else if(isAtomic == false)
        return Status::IOError("is not execute atomReady");

    isAtomic = false;
	return db->Write(WriteOptions(), &batch);
}

Status LevelDB::query(const Slice& key, std::string& value)
{
	if(db == NULL)
		return Status::IOError("database is not open");

	return db->Get(ReadOptions(), key, &value);
}

Status LevelDB::query(const unsigned short type, const unsigned int id, std::string& value)
{
    char tmpArray[6];
    makeKey(type, id, tmpArray);

    return query(Slice(tmpArray, sizeof(tmpArray)/sizeof(char)), value);
}

//Status LevelDB::query(const unsigned short type, const unsigned int id, const int index, std::string& value)
//{
//    char tmpArray[7];
//    makeKey(type, id, index, tmpArray);
//
//    return query(Slice(tmpArray, sizeof(tmpArray)/sizeof(char)), value);
//}

Status LevelDB::make()
{
	if(dbPath.length() == 0 || dbName.length() == 0)
		return Status::NotSupported("dbpath or dbname invalid");
    
    if(isOpen())
        close();

	Status statue;
	statue = this->open(dbPath, dbName, true, false);
	if(statue.ok() == false)
		return statue;

	this->close();

	return Status::OK();
}

void LevelDB::close()
{
	if(db != NULL)
	{
		db->~DB();
		db = NULL;
	}
}

Status LevelDB::destory()
{
	if(db == NULL)
		return Status::IOError("database is not open");

	batch.Clear();

	return DestroyDB(dbPath, dbName, option);
}
void LevelDB::makeKey(const short type, const int id, char* key)
{
	integer2array(type, key, 2);
	integer2array(id, key + 2, 4);
}

//void LevelDB::makeKey(const short type, const int id, int index, char * key)
//{
//    integer2array(type, key, 2);
//    integer2array(id, key + 2, 4);
//    integer2array(index, key + 6, 1);
//}

void LevelDB::integer2array(const long long value, char * dest, const int len)
{
	if(dest == NULL)
		return ;

    for(int i = 0 ; i < len ; i++)
    {
        dest[i] = (unsigned char)(((value >> (len - i - 1) * 8)) & 0xff);
    }
}

#ifdef BUILD_HADOOP
Status LevelDB::getVirtualFiles(std::vector<VirtualMemFile *>& _files)
{
	_files.clear();

	if(db == NULL)
	{
		return Status::IOError("database is not open");
	}

	_files = db->getFiles();

	return Status::OK();
}
#endif
