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
#include "ARProplistHelper.h"

class CARActiveLink :
	public CARServerObject
{
public:
	CARActiveLink(string name, int insideId);
	~CARActiveLink(void);

	unsigned int order;
	ARWorkflowConnectStruct schemaList;
	ARInternalIdList groupList;
	unsigned int executeMask;
	ARInternalId controlField;
	ARInternalId focusField;
	unsigned int enable;
	ARQualifierStruct query;
	ARActiveLinkActionList actionList;
	ARActiveLinkActionList elseList;		
	ARPropList objPropList;

	string GetExecuteOn(bool singleLine=false, CARProplistHelper* props=NULL);
	string GetURL(int rootLevel);
};
