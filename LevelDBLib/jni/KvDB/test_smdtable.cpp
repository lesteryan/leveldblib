#include "SMDIndexSegment.h"
#include "SMDSStorage.h"
#include "SMDDataSegment.h"
#include "TableDef.h"
#include "ShortField.h"
#include "LongField.h"
#include "FloatField.h"
#include "DoubleField.h"
#include "TextField.h"
#include "BinaryField.h"
#include "VarIntField.h"
#include "IntegerField.h"
#include "VarTextField.h"
#include "VarBinaryField.h"
#include "FLDoubleField.h"
#include "dev_buffer.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace kvdbEngine;

static const string PATH("D:\\vc_project\\my_smd_table\\Debug\\test.ssg");
void read_table(void){
	SMDTable storage;
	storage.open(PATH);

	Record* rd2 = storage.search("Table",3456);
	
	int ival=0;
	rd2->getFieldAsInteger(0,ival);
	if (ival == 653){
		cout<<"test ok"<<endl;
	}
	rd2->getFieldAsInteger(1,ival);
	if (ival == 35){
	    cout<<"test ok"<<endl;
	}
	delete rd2;

	rd2 = storage.search("Table",3456);
	
	ival=0;
	rd2->getFieldAsInteger(0,ival);
	if (ival == 653){
		cout<<"test ok"<<endl;
	}
	rd2->getFieldAsInteger(1,ival);
	if (ival == 35){
	    cout<<"test ok"<<endl;
	}
	delete rd2;
}

