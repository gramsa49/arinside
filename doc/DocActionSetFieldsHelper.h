//Copyright (C) 2009 John Luthgers | jls17
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
#include "..\arinside.h"
#include "..\core\arqualification.h"

class CDocActionSetFieldsHelper
{
public:
	CDocActionSetFieldsHelper(CARInside &arInside, CARServerObject &arServerObject, ARSetFieldsActionStruct& sFieldStruct, int structItemType, string& ifElse, int numAction);
	~CDocActionSetFieldsHelper(void);

	void SetFieldsGetSecondaryForm(const string& fromSchema, int rootLevel, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual);

private:
	CARInside& arIn;
	CARServerObject& obj;
	ARSetFieldsActionStruct& setFieldsStruct;
	int arStructItemType;
	string& ifElse;
	int nAction;

	bool CheckAssignment(const ARAssignStruct &assignment, const string& fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual);
};
