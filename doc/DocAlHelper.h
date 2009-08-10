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
#include "..\arinside.h"
#include "..\core\arqualification.h"

class CDocAlHelper
{
public:
	CDocAlHelper(CARInside &arInside, CARActiveLink &arActiveLink);
	~CDocAlHelper(void);

	void SetFieldsGetSecondaryForm(ARSetFieldsActionStruct sFieldStruct, string fromSchema, int rootLevel, stringstream &strmSchema, stringstream &strmSchemaDisplay, stringstream &strmServer, stringstream &strmQual);

private:
	CARInside *pInside;
	CARActiveLink *pAl;

	void FieldAssignement(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc);
	void CheckAssignment(ARAssignStruct &assignment, string fromSchema, int rootLevel, stringstream &assignSchema, stringstream &assignSchemaDisplay, stringstream &assignServer, stringstream &assignQual, string refItemDesc);
};
