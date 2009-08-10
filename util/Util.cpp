//Copyright (C) 2009 Stefan Nerlich | stefan.nerlich@hotmail.com
//
//This file is part of ARInside.
//
//    ARInside is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    ARInside is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

#include "StdAfx.h"
#include "Util.h"

CUtil::CUtil(void)
{
}

CUtil::~CUtil(void)
{
}

string CUtil::TimeToString(ARTimestamp ts)
{
	string result = "";
	struct tm *stm;
	static char dest[100];

	time_t dtime = (time_t) ts;
	stm = localtime(&dtime);
	strftime(dest, 100, "%Y-%m-%d&nbsp;%H:%M:%S", stm);

	result = dest;
	return result;
}


string CUtil::TimeToTextString(ARTimestamp ts)
{
	string result = "";
	struct tm *stm;
	static char dest[100];

	time_t dtime = (time_t) ts;
	stm = localtime(&dtime);
	strftime(dest, 100, "%Y-%m-%d %H:%M:%S", stm);

	result = dest;
	return result;
}

string CUtil::ClearSpaces(string inStr)
{
	string result = "";
	remove_copy_if( inStr.begin(), inStr.end(), insert_iterator<string>(result, result.begin()), bind2nd(equal_to<char>(), ' '));    
	return result;
}

string CUtil::String2Comp(string inStr)
{
	inStr = ClearSpaces(inStr);

	for(unsigned i = 0; i < inStr.length(); i++)
		inStr[i] = tolower(inStr[i]);

	return inStr;
}

string CUtil::StrReplace(const std::string &search, const std::string &replace, std::string searchin)
{
	std::string::size_type pos = searchin.find(search, 0);
	unsigned int searchLength = (unsigned int)search.length();

	while(pos != std::string::npos)
	{
		searchin.replace(pos, searchLength, replace);
		pos = searchin.find(search, pos + searchLength);
	}

	return searchin;
}

int CUtil::SplitString(const std::string& input_record, std::vector<std::string>& colv)
{
	std::istringstream s(input_record);
	std::string temp;

	colv.clear();
	while (std::getline(s, temp, ';')) 
	{
		colv.push_back(temp);
	}

	return static_cast<int>(colv.size());
}

//Remove last n chars from string
void CUtil::CleanUpStream(stringstream &strm, unsigned int nLength)
{
	if(strm != NULL
		&& strm.str().size() > nLength)
	{
		string tmp = strm.str();
		tmp.erase(tmp.size()-nLength, nLength);		
		strm.str("");
		strm << tmp;
	}
}