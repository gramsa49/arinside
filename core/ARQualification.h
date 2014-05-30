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
#include "../ARInside.h"

class CARQualification
{
public:
	CARQualification(CARInside &arIn);
	~CARQualification(void);

	void CheckQuery(const ARQualifierStruct *query, const CRefItem &refItem, int depth, int pFormId, int sformId, stringstream &qText, int rootLevel);

	int arsStructItemType;;

private:
	CARInside *arIn;
	void CheckOperand(ARFieldValueOrArithStruct *operand, ARFieldValueOrArithStruct *parent, const CRefItem &refItem, int pFormId, int sFormId, stringstream &qText, int rootLevel);
	int FindCurrentEnumFieldId(int pFormId, int sFormId);
	char* getFieldPrefix(ARFieldValueOrArithStruct *operand);

	int tmpFormId;
	vector<const ARQualifierStruct*> qualLevels;
};
