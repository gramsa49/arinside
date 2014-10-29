//Copyright (C) 2014 John Luthgers | jls17
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
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "UTF8StringList.h"
#include "thirdparty/utf8cpp/source/utf8.h"
#include <locale>

UTF8StringList::UTF8StringList()
#ifdef WIN32
: systemLocale(""), coll(std::use_facet<std::collate<sort_char_t> >(systemLocale))
#else
: coll(std::use_facet<std::collate<char> >(localeDetection.sortLocale))
#endif
{
}

void UTF8StringList::Allocate(unsigned int size)
{
	list.reserve(size);
}

void UTF8StringList::PushBack(ARNameType &value)
{
	unsigned int skippedSpaces = 0;
	while (value[skippedSpaces] == ' ') skippedSpaces++;

#ifdef WIN32
	vector<wchar_t> wideString;
	utf8::utf8to16(&value[skippedSpaces], &value[strlen(value)], back_inserter(wideString));
	wideString.push_back(0);
	
	wchar_t *uStr = &wideString[0];
	list.push_back(uStr);
#else
	list.push_back(&value[skippedSpaces]);
#endif
}

void UTF8StringList::PushBack(const std::string &value)
{
	unsigned int skippedSpaces = 0;
	while (value[skippedSpaces] == ' ') skippedSpaces++;

#ifdef WIN32
	vector<wchar_t> wideString;
	utf8::utf8to16(value.begin() + skippedSpaces, value.end(), back_inserter(wideString));
	wideString.push_back(0);
	
	wchar_t *uStr = &wideString[0];
	list.push_back(uStr);
#else
	list.push_back(value.c_str() + skippedSpaces);
#endif
}

bool UTF8StringList::operator() (int l, int r)
{
	return (
		coll.compare(list[l].c_str(), list[l].c_str() + list[l].size(),
		             list[r].c_str(), list[r].c_str() + list[r].size()
		            ) < 0);
}
