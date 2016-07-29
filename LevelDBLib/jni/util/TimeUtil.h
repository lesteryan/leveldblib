/*
 * TimeUtil.h
 *
 *  Created on: 2016?6?29?
 *      Author: lt_dan
 */

#ifndef UTIL_TIMEUTIL_H_
#define UTIL_TIMEUTIL_H_

#include <time.h>
#include <string>

class TimeUtil
{
public:
	void start()
	{
		startTime = getCurrentTime();
	}

	void stop()
	{
		stopTime = getCurrentTime();
	}

	jlong getTime()
	{
		return stopTime - startTime;
	}

	long getCurrentTime()
	{
	   struct timeval tv;
	   gettimeofday(&tv,NULL);
	   return  tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}

private:
	jlong startTime, stopTime;
};


#endif /* UTIL_TIMEUTIL_H_ */
