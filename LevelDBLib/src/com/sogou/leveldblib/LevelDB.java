package com.sogou.leveldblib;

import android.util.Log;

public class LevelDB {
	
	public static final String TAG = "LevelDB Java";
	public LevelDB()
	{
		test();
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
	
//	public native boolean open(String dbname, boolean read_only, boolean write_only);
//	public native long insert();
//	public native boolean clean();
//	public native long query();
//	public native boolean close();
//	public native boolean reopen();
	public native void test();
	
	static
	{
		System.loadLibrary("LevelDB");
	}
}
