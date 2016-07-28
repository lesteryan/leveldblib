package com.sogou.leveldblib;

import android.util.Log;

public class LevelDB {
	
	public static final String TAG = "LevelDB Java";
	public LevelDB()
	{
		if(open("beijing", false, true) &&
		insert() > 0&&
		close() &&
		open("beijing", false, true) &&
		close() &&
		open("beijing", true, false)&&
		query() > 0 &&
		close())
		{}
//		
//		open("tianjin", false, true);
//		insert();
//		close();
//		open("tianjin", false, true);
//		close();
//		open("tianjin", true, false);
//		query();
//		close();
//		
//		open("beijing", true, false);
//		query();
//		close();
//		
//		open("tianjin", true, false);
//		query();
//		close();
	}
	
	public native boolean open(String dbname, boolean read_only, boolean write_only);
	public native long insert();
	public native boolean clean();
	public native long query();
	public native boolean close();
	public native boolean reopen();
	
	static
	{
		System.loadLibrary("LevelDB");
	}
}
