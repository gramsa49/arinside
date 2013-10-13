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

#include "stdafx.h"
#include "ARDayStructHelper.h"

string CARDayStructHelper::DayStructToHTMLString(const ARDayStruct* dayStruct)
{
	stringstream strm; strm.str("");
	try
	{
		char weekdays[][15] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

		strm << "Weekday(s):<br/> ";
		bool bWeekDay=false;
		for (unsigned int i = 0; i < 7; i++)
		{
			if (AR_DAY(dayStruct->weekday, i))
			{
				strm << weekdays[i] << ", ";
				bWeekDay=true;
			}				
		}
		if(!bWeekDay)
			strm << "None ";
		else
			CUtil::CleanUpStream(strm, 2);

		strm << "\n<br/><br/>Day of Month:<br/>\n";
		bool bDayOfMonth=false;
		for (unsigned int i = 0; i < 31; i++)
		{
			if (AR_DAY(dayStruct->monthday, i))
			{
				strm << i+1 << ", ";
				bDayOfMonth=true;
			}
		}
		if(!bDayOfMonth)
			strm << "None ";
		else
			CUtil::CleanUpStream(strm, 2);

		strm << "<br/><br/>Time:<br/>";
		bool bTime=false;
		for (unsigned int i = 0; i < 24; i++)
		{
			if (AR_HOUR(dayStruct->hourmask, i))
			{
				char tmp[10];
				sprintf(tmp, "%.02d:%.02d", i, dayStruct->minute);
				strm << tmp << ", ";
				bTime=true;
			}
		}
		if(!bTime)
			strm << "None";
		else
			CUtil::CleanUpStream(strm, 2);
	}
	catch (...)
	{
		cerr << "EXCEPTION in Escalation GetTimeCriteria" << endl; 
	}
	return strm.str();
}

void CARDayStructHelper::SplitInterval(unsigned int interval, unsigned int &days, unsigned int &hours, unsigned int &minutes, unsigned int &seconds)
{
	days = interval / (3600 * 24);
	if (days > 0)
		interval -= (days * (3600 * 24));
	hours = interval / 3600;
	if (hours > 0)
		interval -= (hours * 3600);
	minutes = interval / 60;
	if (minutes > 0)
		interval -= minutes * 60;
	seconds = interval;
}
