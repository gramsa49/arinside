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
#include "ARValue.h"
#include "../output/Table.h"
#include "../output/TableRow.h"
#include "../ARInside.h"

using namespace OUTPUT;

class CARAssignHelper
{
public:
	CARAssignHelper(CARInside &arIn, int rootLevel, const string& objName, int objType, const string& schemaName1, const string& schemaName2);
	~CARAssignHelper(void);

	bool pushFieldFlag;

	string PushFieldsAssignment(const ARPushFieldsActionStruct &action, int nAction, const string& ifElse);
	string SetFieldsAssignment(const ARSetFieldsActionStruct &action, int nAction, const string& ifElse);
	string OpenWindowAssignment(const ARFieldAssignList &action, int nAction, const string& ifElse, const string& openCloseInfo);
	string CloseWindowAssignment(const ARFieldAssignList &action, int nAction, const string& ifElse, const string& openCloseInfo);
	string ServiceAssignment(const ARFieldAssignList &action, int nAction, const string& ifElse, const string& serviceInfo);
	unsigned int CheckAssignment(int targetFieldId, ARAssignStruct* parentAssignment, const string& ifElse, int nAction, ARAssignStruct &assignment, stringstream &assignText, const string& refItemDesc);

	void AssignValue(int targetFieldId, const string& ifElse, ARValueStruct &v, stringstream &assignText, const string& refItemDesc);	
	void AssignField(const string& ifElse, int nAction, ARAssignFieldStruct &v, stringstream &assignText, const string& refItemDesc);
	void AssignProcess(const string& ifElse, char *v, stringstream &assignText, const string& refItemDesc);
	void AssignFunction(int targetFieldId, const string& ifElse, int nAction, ARFunctionAssignStruct &v, stringstream &assignText, const string& refItemDesc);	
	void AssignDDE(const string& ifElse, ARDDEStruct &v, stringstream &assignText, const string& refItemDesc);	
	void AssignSQL(const string& ifElse, ARAssignSQLStruct &v, stringstream &assignText, const string& refItemDesc);	
	void AssignFilterApi(const string& ifElse, ARAssignFilterApiStruct &v, stringstream &assignText, const string& refItemDesc);	

private:
	CARInside *arIn;
	string schemaName1;
	string schemaName2;
	int schemaInsideId1;
	int schemaInsideId2;
	int rootLevel;

	string objName;
	int objType;
};
