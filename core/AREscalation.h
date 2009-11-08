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

#pragma once
#include "ARServerObject.h"

class CAREscalation :
	public CARServerObjectWithData
{
public:
	CAREscalation(string name, int insideId);
	~CAREscalation(void);

	AREscalationTmStruct escalationTm;
	ARWorkflowConnectStruct schemaList;
	unsigned int enable;
	ARQualifierStruct query;
	ARFilterActionList actionList;
	ARFilterActionList elseList;
	ARPropList objPropList;

	string GetTimeCriteria();
	string GetExecuteOn();
	string GetURL(int rootLevel, bool showImage = true);
	int GetServerObjectTypeXML() { return AR_STRUCT_ITEM_XML_ESCALATION; }
};
