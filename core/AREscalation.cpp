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
#include ".\arescalation.h"

CAREscalation::CAREscalation(string name, int insideId)
{
	this->name = name;
	this->insideId = insideId;
	this->enable = 0;

	this->objPropList.props = NULL;
}

CAREscalation::~CAREscalation(void)
{
	try
	{
		FreeARWorkflowConnectStruct (&schemaList, false);
		FreeARQualifierStruct (&query, false);
		FreeARFilterActionList (&actionList, false);
		FreeARFilterActionList (&elseList, false);	

		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
	catch(...)
	{
	}
}

string CAREscalation::GetURL(int rootLevel)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"escalation/"+this->FileID()+"/"+CWebUtil::DocName("index"), "escalation.gif", rootLevel);
}

string CAREscalation::GetTimeCriteria()
{
	stringstream strm;
	strm.str("");

	try
	{
		if(escalationTm.escalationTmType == NULL)
		{
			return "Unknown Escalation type";
		}

		//Check if the Escalation runs On Intervall		
		if (escalationTm.escalationTmType == AR_ESCALATION_TYPE_INTERVAL)
		{
			unsigned int interval, days, hours, minutes;
			interval = escalationTm.u.interval;
			days = interval / (3600 * 24);
			if (days > 0)
				interval -= (days * (3600 * 24));
			hours = interval / 3600;
			if (hours > 0)
				interval -= (hours * 3600);
			minutes = interval / 60;
			strm << days << " Days " << hours << " Hours " << minutes << " Minutes";
		}
		else
		{			
			char weekdays[][15] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

			strm << "Weekday(s):<br/> ";
			bool bWeekDay=false;
			for (unsigned int i = 0; i < 7; i++)
			{
				if (AR_DAY(escalationTm.u.date.weekday, i))
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
				if (AR_DAY(escalationTm.u.date.monthday, i))
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
				if (AR_HOUR(escalationTm.u.date.hourmask, i))
				{
					char tmp[10];
					sprintf(tmp, "%.02d:%.02d", i, escalationTm.u.date.minute);
					strm << tmp << ", ";
					bTime=true;
				}
			}
			if(!bTime)
				strm << "None";
			else
				CUtil::CleanUpStream(strm, 2);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in Escalation GetTimeCriteria" << endl; 
	}

	return strm.str();
}

string CAREscalation::GetExecuteOn()
{
	if(escalationTm.escalationTmType != NULL
		&& escalationTm.escalationTmType == AR_ESCALATION_TYPE_INTERVAL)
	{
		return "Interval";
	}

	return "Time";
}