#ifndef _SMD_TABLE_CONFIG_H_
#define _SMD_TABLE_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#if defined(TARGET_IOS) || defined(TARGET_LINUX) 
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

// meta������洢��Ԫ�ظ���
#define MAX_META_INFO_COUNT (256)

// �ļ���ʶ
#define STORAGE_FLAG ("SMDTABLE")
#define STROAGE_FLAG_LENGTH (16)

#endif

#define k
