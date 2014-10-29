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

#pragma once

#include "InternalNameList.h"

class UTF8StringList : public InternalNameList
{
public:
#ifdef WIN32
	typedef wchar_t sort_char_t;
	typedef std::wstring list_string_t;
#else
	typedef char sort_char_t;
	typedef std::string list_string_t;
#endif

private:
#ifdef WIN32
	std::locale systemLocale;
#else
	LocaleDetector localeDetection;
#endif
	const std::collate<sort_char_t> &coll;

public:
	UTF8StringList();

	virtual void Allocate(unsigned int size);
	virtual void PushBack(ARNameType &value);
	virtual void PushBack(const std::string &value);

	bool operator()(int l, int r);

	vector<list_string_t> list;
};
