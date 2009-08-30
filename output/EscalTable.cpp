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

#include "StdAfx.h"
#include "../core/arvalue.h"
#include ".\escaltable.h"

using namespace OUTPUT;

CEscalTable::CEscalTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("escalationList", "TblObjectList");
	tbl->AddColumn(30, "Escalation Name");
	tbl->AddColumn(10, "Enabled");
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
	if (pInside->CompareServerVersion(7, 1) >= 0)
		tbl->AddColumn(5, "Pool");
#endif
	tbl->AddColumn(10, "ExecuteOn");
	tbl->AddColumn(5, "If");
	tbl->AddColumn(5, "Else");
	tbl->AddColumn(20, "Changed");
	tbl->AddColumn(20, "By");
}

CEscalTable::~CEscalTable(void)
{
}

void CEscalTable::AddRow(CAREscalation &escal, int rootLevel)
{
	CTableRow tblRow("");
	tblRow.AddCell( CTableCell(escal.GetURL(rootLevel)));

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
	string escalPool;
	if (pInside->CompareServerVersion(7, 1) >= 0)
	{
		CARProplistHelper props(&escal.objPropList);
		ARValueStruct* val = props.GetValue(AR_OPROP_POOL_NUMBER);
		if (val != NULL)
			escalPool = CARValue::ValueToString(*val);
		else
			escalPool = "";
	}
#endif

	string tmpCssEnabled = "";
	if(escal.enable==0)
		tmpCssEnabled = "objStatusDisabled";
	tblRow.AddCell( CTableCell(CAREnum::ObjectEnable(escal.enable), tmpCssEnabled));
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
	if (pInside->CompareServerVersion(7, 1) >= 0)
		tblRow.AddCell( CTableCell(escalPool));
#endif
	tblRow.AddCell( CTableCell(escal.GetExecuteOn()));
	tblRow.AddCell( CTableCell(escal.actionList.numItems));
	tblRow.AddCell( CTableCell(escal.elseList.numItems));
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(escal.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(escal.lastChanged, rootLevel)));
	this->tbl->AddRow(tblRow);
}