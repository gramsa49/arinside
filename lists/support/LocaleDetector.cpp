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
#include "LocaleDetector.h"

LocaleDetector::LocaleDetector()
{
	locale systemLocale("");
	string name = systemLocale.name();

	transform(name.begin(), name.end(), name.begin(), ::tolower);
	if (!CUtil::StrEndsWith(name, "utf8") && !CUtil::StrEndsWith(name, "utf-8"))
	{
		size_t index = name.find_first_not_of("abcdefghijklmnopqrstuvwxyz_");
		if (index != string::npos)
		{
			name = systemLocale.name().substr(0, index);
		}
		else
			name = systemLocale.name();

		name += ".utf8";

		try
		{
			sortLocale = std::locale(name.c_str());
		}
		catch (runtime_error &rterr)
		{
			cerr << "WARN: " << rterr.what() << " (" << name << ")" << endl;
			cerr << "Sorting might be incorrect! Please select a valid utf8 locale before running ARInside!";
			sortLocale = systemLocale;
		}
	}
	else
	{
		sortLocale = systemLocale;
	}
}
