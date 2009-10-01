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

#include "stdafx.h"
#include "FilterTable.h"

using namespace OUTPUT;

CFilterTable::CFilterTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("filterList", "TblObjectList");
	tbl->AddColumn(30, "Filter Name");
	tbl->AddColumn(5, "Enabled");
	tbl->AddColumn(5, "Order");
	tbl->AddColumn(20, "Execute On");
	tbl->AddColumn(5, "If");
	tbl->AddColumn(5, "Else");
	tbl->AddColumn(15, "Changed");
	tbl->AddColumn(15, "By");
}

CFilterTable::~CFilterTable(void)
{
}

void CFilterTable::AddRow(CARFilter &filter, int rootLevel)
{
	CTableRow tblRow("");

	tblRow.AddCell( CTableCell(filter.GetURL(rootLevel)));

	string tmpCssEnabled = "";
	if(filter.enable==0)
		tmpCssEnabled = "objStatusDisabled";
	tblRow.AddCell( CTableCell(CAREnum::ObjectEnable(filter.enable), tmpCssEnabled));

	tblRow.AddCell( CTableCell(filter.order));
	tblRow.AddCell( CTableCell(filter.GetExecuteOnEx()));
	tblRow.AddCell( CTableCell(filter.actionList.numItems));
	tblRow.AddCell( CTableCell(filter.elseList.numItems));
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(filter.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(filter.lastChanged, rootLevel)));

	this->tbl->AddRow(tblRow);
}
