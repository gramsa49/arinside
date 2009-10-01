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
#include "../output/Table.h"
#include "../output/TableRow.h"

using namespace OUTPUT;

class CARProplistHelper
{
public:
	CARProplistHelper(ARPropList* propList);
	~CARProplistHelper(void);

	static string GetList(ARPropList &objPropList);
	static string GetLabel(ARULong32 nProp);
	static string GetValue(ARULong32 nProp, ARValueStruct &arV);

	ARValueStruct* GetAndUseValue(ARULong32 nProp);
	ARValueStruct* GetValue(ARULong32 nProp);
	string UnusedPropertiesToHTML();

private:
	class PropHelpData
	{
	public:
		PropHelpData(void);
		PropHelpData(ARULong32 propId, ARValueStruct* value);
		~PropHelpData(void);

		operator ARULong32() { return pId; }

		ARULong32       pId;    // property id
		ARValueStruct*  Value;  // value struct
		bool            isUsed; // used flag
	};
	std::vector<PropHelpData> properties;

};
