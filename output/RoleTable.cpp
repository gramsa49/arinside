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
#include "RoleTable.h"

using namespace OUTPUT;

CRoleTable::CRoleTable(CARInside &arIn)
: CObjectTable("roleList", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn(25, "Role Name");
	tbl.AddColumn(5, "RoleID");
	tbl.AddColumn(30, "Application");	
	tbl.AddColumn(20, "Modified");
	tbl.AddColumn(20, "By");
}

CRoleTable::~CRoleTable(void)
{
}

void CRoleTable::AddRow(CARRole &role, int rootLevel)
{
	CTableRow tblRow("");
	tblRow.AddCell( CTableCell(role.GetURL(rootLevel)));
	tblRow.AddCell( CTableCell(role.GetRoleId()));
	tblRow.AddCell( CTableCell(this->pInside->LinkToContainer(role.GetApplicationName(), rootLevel)));
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(role.GetTimestamp())));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(role.GetLastChanged(), rootLevel)));
	this->tbl.AddRow(tblRow);
}
