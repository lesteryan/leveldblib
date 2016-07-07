package com.sogou.leveldblib;

public class LevelDB {
	
	public LevelDB()
	{
//		open();
//		insert();
	}
	
	public native boolean open();
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
