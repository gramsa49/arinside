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

class CARFilter :
	public CARServerObjectWithData
{
public:
	CARFilter(string name, int insideId);
	~CARFilter(void);

	// filter base informations from api
	unsigned int order;
	unsigned int opSet;
	unsigned int enable;
	unsigned int errorOptions;
	ARNameType errorFilterName;
	ARQualifierStruct query;
	ARFilterActionList actionList;
	ARFilterActionList elseList;
	ARWorkflowConnectStruct schemaList;
	ARPropList objPropList;

	// filter enhanced informations
	list<string> errorCallers;

	// some helper methods
	string GetExecuteOn();
	string GetExecuteOnEx();
	string GetURL(int rootLevel, bool showImage = true);
	int GetServerObjectTypeXML() { return AR_STRUCT_ITEM_XML_FILTER; }
};
