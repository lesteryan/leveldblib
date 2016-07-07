package com.sogou.leveldblib;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

public class MainActivity extends Activity implements OnClickListener {
	
	LevelDB levelDB;
	String TAG = "MainActivity";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		levelDB = new LevelDB();

		findViewById(R.id.button_open).setOnClickListener(this);
		findViewById(R.id.button_insert).setOnClickListener(this);
		findViewById(R.id.button_clean).setOnClickListener(this);
		findViewById(R.id.button_query).setOnClickListener(this);
		findViewById(R.id.button_close).setOnClickListener(this);
		findViewById(R.id.button_reopen).setOnClickListener(this);
	}

	@Override
	public void onClick(View arg0) {
		// TODO Auto-generated method stub
		switch(arg0.getId())
		{
		case R.id.button_open:
		{
			if(levelDB.open())
				Toast.makeText(this, "open success", Toast.LENGTH_SHORT).show();
			else
				Toast.makeText(this, "open failed", Toast.LENGTH_SHORT).show();
			break;
		}
		
		case R.id.button_insert:
		{
			long time = levelDB.insert();
			if(time == -1)
				Toast.makeText(this, "insert failed", Toast.LENGTH_SHORT).show();
			else
				Toast.makeText(this, "insert cost " + time + "ms", Toast.LENGTH_SHORT).show();
			break;
		}
		
		case R.id.button_clean:
		{
			if(levelDB.clean())
				Toast.makeText(this, "clean success", Toast.LENGTH_SHORT).show();
			else
				Toast.makeText(this, "clean failed", Toast.LENGTH_SHORT).show();
			break;
		}
		
		case R.id.button_query:
		{
			long time = levelDB.query();
			if(time == -1)
				Toast.makeText(this, "query failed", Toast.LENGTH_SHORT).show();
			else
				Toast.makeText(this, "query cost " + time + "ms", Toast.LENGTH_SHORT).show();
			break;
		}
		
		case R.id.button_close:
		{
			if(levelDB.close())
				Toast.makeText(this, "open success", Toast.LENGTH_SHORT).show();
			else
				Toast.makeText(this, "open failed", Toast.LENGTH_SHORT).show();
			break;
		}
		
		case R.id.button_reopen:
		{
			if(levelDB.reopen())
				Toast.makeText(this, "reopen success", Toast.LENGTH_SHORT).show();
			else
				Toast.makeText(this, "reopen failed", Toast.LENGTH_SHORT).show();
			break;
		}
		}
	}
}

