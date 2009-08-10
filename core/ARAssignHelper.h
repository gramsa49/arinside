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
#include ".\arvalue.h"
#include "..\output\table.h"
#include "..\output\tablerow.h"
#include "..\arinside.h"

using namespace OUTPUT;

class CARAssignHelper
{
public:
	CARAssignHelper(CARInside &arIn, string dir, int rootLevel, string objName, int objType, string schemaName1, string schemaName2);
	~CARAssignHelper(void);

	bool pushFieldFlag;
	bool openWindowFlag;

	string PushFieldsAssignment(ARPushFieldsActionStruct &action, int nAction, string ifElse);
	string SetFieldsAssignment(ARSetFieldsActionStruct &action, int nAction, string ifElse);
	string OpenWindowAssignment(ARFieldAssignList &action, int nAction, string ifElse, string openCloseInfo);
	string CloseWindowAssignment(ARFieldAssignList &action, int nAction, string ifElse, string openCloseInfo);
	string ServiceAssignment(ARFieldAssignList &action, int nAction, string ifElse, string serviceInfo);
	void CheckAssignment(int targetFieldId, string ifElse, int nAction, ARAssignStruct &assignment, stringstream &assignText, string refItemDesc);

	void AssignValue(int targetFieldId, string ifElse, ARValueStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignField(string ifElse, int nAction, ARAssignFieldStruct &v, stringstream &assignText, string refItemDesc);
	void AssignProcess(string ifElse, char *v, stringstream &assignText, string refItemDesc);
	void AssignFunction(int targetFieldId, string ifElse, int nAction, ARFunctionAssignStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignDDE(string ifElse, ARDDEStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignSQL(string ifElse, ARAssignSQLStruct &v, stringstream &assignText, string refItemDesc);	
	void AssignFilterApi(string ifElse, ARAssignFilterApiStruct &v, stringstream &assignText, string refItemDesc);	

private:
	CARInside *arIn;
	string dir;
	string schemaName1;
	string schemaName2;
	int schemaInsideId1;
	int schemaInsideId2;
	int rootLevel;

	string objName;
	int objType;
};
