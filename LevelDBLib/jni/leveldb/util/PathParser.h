/*
 * PathParser.h
 *
 *  Created on: 2016年8月1日
 *      Author: yanxi
 */

#ifndef PATHPARSER_H_
#define PATHPARSER_H_

class PathParser
{
public:
	static bool formatFilePath(const std::string &in, std::string& out)
	{
	    if(in.length() <= 1)
	        return false;

	    out = in;

	    for(int i = 0 ; i < out.length() ; i++)
	        if(out[i] == '\\')
	            out[i] = '/';

	    for(int i = 0 ; i < out.length() - 1 ; i++)
	    {
	        if(out[i] == '/' && out [i + 1] == '/')
	        {
	            out = out.substr(0, i) + out.substr(i + 1);
	            i--;
	        }
	    }

	    return true;
	}

	static bool formatDir(const std::string &in, std::string& out)
	{
	    bool ret = formatFilePath(in, out);
	    if(ret == false)
	        return false;

	    if(out[out.length() - 1] != '/')
	        out = out + "/";

	    return true;
	}

	static bool parseFilePath(const std::string& fname, std::string& filePath, std::string& fileName)
	{
	    bool ret = formatFilePath(fname, fileName);
	    if(ret == false)
	        return false;

	    int lastSep = fileName.find_last_of('/');
	    if(lastSep == std::string::npos)
	    {
	        return false;
	    }

	    filePath = fileName.substr(0, lastSep + 1);
	    fileName = fileName.substr(lastSep + 1, fname.length() - lastSep);

	    return true;
	}
};

#endif /* PATHPARSER_H_ */
