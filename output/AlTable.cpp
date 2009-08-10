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
#include ".\altable.h"

using namespace OUTPUT;

CAlTable::CAlTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("alList", "TblObjectList");
	tbl->AddColumn(30, "Active Link Name");	
	tbl->AddColumn(5, "Enabled");	
	tbl->AddColumn(5, "Groups");
	tbl->AddColumn(5, "Order");	
	tbl->AddColumn(15, "Execute On");
	tbl->AddColumn(5, "If");
	tbl->AddColumn(5, "Else");
	tbl->AddColumn(15, "Changed");
	tbl->AddColumn(15, "By");
}

CAlTable::~CAlTable(void)
{	
}

void CAlTable::AddRow(CARActiveLink &al, int rootLevel)
{
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
	tblRow.AddCell(	CTableCell(al.GetExecuteOnEx()));
	tblRow.AddCell(	CTableCell(al.actionList.numItems));
	tblRow.AddCell(	CTableCell(al.elseList.numItems));
	tblRow.AddCell(	CTableCell(CUtil::TimeToString(al.timestamp)));
	tblRow.AddCell(	CTableCell(this->pInside->LinkToUser(al.lastChanged, rootLevel)));
	this->tbl->AddRow(tblRow);


}