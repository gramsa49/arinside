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
#include "AREscalation.h"
#include "ARDayStructHelper.h"

CAREscalation::CAREscalation(string name, int insideId)
: CARServerObjectWithData(insideId)
{
	this->name = name;
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

string CAREscalation::GetURL(int rootLevel, bool showImage)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"escalation/"+this->FileID()+"/"+CWebUtil::DocName("index"), (showImage ? "escalation.gif" : ""), rootLevel);
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
			return CARDayStructHelper::DayStructToHTMLString(&escalationTm.u.date);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in Escalation GetTimeCriteria: " << e.what() << endl; 
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
