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
#define UTF8_SORT_LOCALE systemLocale
: systemLocale(""), coll(std::use_facet<std::collate<sort_char_t> >(UTF8_SORT_LOCALE))
#else
#define UTF8_SORT_LOCALE localeDetection.sortLocale
: coll(std::use_facet<std::collate<char> >(UTF8_SORT_LOCALE))
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
	std::wstring wideString;
	wideString.reserve(AR_MAX_NAME_SIZE + 1);
	utf8::utf8to16(&value[skippedSpaces], &value[strlen(value)], back_inserter(wideString));
	MakeLower(wideString);

	list.push_back(wideString);
#else
	list.push_back(&value[skippedSpaces]);
#endif
}

void UTF8StringList::PushBack(const std::string &value)
{
	unsigned int skippedSpaces = 0;
	while (value[skippedSpaces] == ' ') skippedSpaces++;

#ifdef WIN32
	std::wstring wideString;
	wideString.reserve(AR_MAX_NAME_SIZE + 1);
	utf8::utf8to16(value.begin() + skippedSpaces, value.end(), back_inserter(wideString));
	MakeLower(wideString);
	
	list.push_back(wideString);
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

UTF8StringList::sort_char_t UTF8StringList::operator () (sort_char_t c)
{
	return std::tolower(c, UTF8_SORT_LOCALE);
}

void UTF8StringList::MakeLower(list_string_t &str)
{
	std::transform(str.begin(), str.end(), str.begin(), *this);
}
