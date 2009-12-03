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
#include "AlTable.h"

using namespace OUTPUT;

CAlTable::CAlTable(CARInside &arIn)
: CObjectTable("alList", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn(0, "Active Link Name");
	tbl.AddColumn(0, "Enabled");	
	tbl.AddColumn(0, "Groups");
	tbl.AddColumn(0, "Order");	
	tbl.AddColumn(0, "Execute On");
	tbl.AddColumn(0, "If");
	tbl.AddColumn(0, "Else");
	tbl.AddColumn(0, "Changed");
	tbl.AddColumn(0, "By");
}

CAlTable::~CAlTable(void)
{	
}

void CAlTable::AddRow(CARActiveLink &al, int rootLevel)
{
	CARProplistHelper props(&al.objPropList);
	CTableRow tblRow("");
	tblRow.AddCell( CTableCell(al.GetURL(rootLevel)));

	string tmpCssEnabled = "";
	if(al.enable==0)
		tmpCssEnabled = "objStatusDisabled";
	tblRow.AddCell( CTableCell(CAREnum::ObjectEnable(al.enable), tmpCssEnabled));

	stringstream strmNumGroup;
	strmNumGroup << al.groupList.numItems;
	tblRow.AddCell(	CTableCell(strmNumGroup.str(), ""));
	tblRow.AddCell(	CTableCell(al.order));
	tblRow.AddCell(	CTableCell(al.GetExecuteOn(true,&props)));
	tblRow.AddCell(	CTableCell(al.actionList.numItems));
	tblRow.AddCell(	CTableCell(al.elseList.numItems));
	tblRow.AddCell(	CTableCell(CUtil::DateTimeToHTMLString(al.timestamp)));
	tblRow.AddCell(	CTableCell(this->pInside->LinkToUser(al.lastChanged, rootLevel)));
	this->tbl.AddRow(tblRow);


}
