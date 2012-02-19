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
#include "../ARInside.h"
#include "AREscalation.h"
#include "ARDayStructHelper.h"

CAREscalation::CAREscalation()
: CARServerObject(-1)
{
}

CAREscalation::CAREscalation(int insideId)
: CARServerObject(insideId)
{
}

CAREscalation::CAREscalation(const string& name)
: CARServerObject(-1)
{
	insideId = CARInside::GetInstance()->escalationList.Find(name.c_str());
}

CAREscalation::~CAREscalation(void)
{
}

bool CAREscalation::IsClonable() const
{
	return true;
}

CARServerObject* CAREscalation::Clone() const
{
	return new CAREscalation(*this);
}

string CAREscalation::GetURL(int rootLevel, bool showImage) const
{
	CPageParams file(PAGE_DETAILS, this);
	return CWebUtil::Link(this->GetName(), file, (showImage ? "escalation.gif" : ""), rootLevel);
}

bool CAREscalation::Exists() const
{
	return (insideId >= 0 && (unsigned int)insideId < CARInside::GetInstance()->escalationList.GetCount());
}

string CAREscalation::GetTimeCriteria()
{
	stringstream strm;
	strm.str("");

	try
	{
		const AREscalationTmStruct& escalationTm = this->GetTimeStruct();
		if(escalationTm.escalationTmType == 0)
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
	if(GetTimeStruct().escalationTmType == AR_ESCALATION_TYPE_INTERVAL)
		return "Interval";
	else
		return "Time";
}

string CAREscalation::GetName()
{
	return CARInside::GetInstance()->escalationList.EscalationGetName(GetInsideId());
}

string CAREscalation::GetName() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetName(GetInsideId());
}

string CAREscalation::GetNameFirstChar()
{
	return CARObject::GetNameFirstChar(CARInside::GetInstance()->escalationList.EscalationGetName(GetInsideId()));
}

bool CAREscalation::NameStandardFirstChar()
{
	return CARObject::NameStandardFirstChar(GetNameFirstChar());
}

const AREscalationTmStruct& CAREscalation::GetTimeStruct() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetTime(GetInsideId());
}

const ARWorkflowConnectStruct& CAREscalation::GetSchemaList() const 
{ 
	return CARInside::GetInstance()->escalationList.EscalationGetSchemaList(GetInsideId());
}

unsigned int CAREscalation::GetEnabled()
{ 
	return CARInside::GetInstance()->escalationList.EscalationGetEnabled(GetInsideId());
}
	
const ARQualifierStruct& CAREscalation::GetRunIf() const 
{ 
	return CARInside::GetInstance()->escalationList.EscalationGetRunIf(GetInsideId()); 
}

const ARFilterActionList& CAREscalation::GetIfActions() const
{ 
	return CARInside::GetInstance()->escalationList.EscalationGetIfActions(GetInsideId());
}

const ARFilterActionList& CAREscalation::GetElseActions() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetElseActions(GetInsideId());
}

const ARPropList& CAREscalation::GetPropList() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetPropList(GetInsideId());
}

const string& CAREscalation::GetAppRefName() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetAppRefName(GetInsideId());
}

void CAREscalation::SetAppRefName(const string &appName)
{
	return CARInside::GetInstance()->escalationList.EscalationSetAppRefName(GetInsideId(), appName);
}

const char* CAREscalation::GetHelpText() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetHelptext(GetInsideId());
}

ARTimestamp CAREscalation::GetTimestamp()
{
	return CARInside::GetInstance()->escalationList.EscalationGetTimestamp(GetInsideId());
}

const ARAccessNameType& CAREscalation::GetOwner() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetOwner(GetInsideId());
}

const ARAccessNameType& CAREscalation::GetLastChanged() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetModifiedBy(GetInsideId());
}

const char* CAREscalation::GetChangeDiary() const
{
	return CARInside::GetInstance()->escalationList.EscalationGetChangeDiary(GetInsideId());
}
