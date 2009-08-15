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
#include ".\roletable.h"

using namespace OUTPUT;

CRoleTable::CRoleTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("roleList", "TblObjectList");
	tbl->AddColumn(25, "Role Name");
	tbl->AddColumn(5, "RoleID");
	tbl->AddColumn(30, "Application");	
	tbl->AddColumn(20, "Modified");
	tbl->AddColumn(20, "By");
}

CRoleTable::~CRoleTable(void)
{
}

void CRoleTable::AddRow(CARRole &role, int rootLevel)
{
	CTableRow tblRow("");
	tblRow.AddCell( CTableCell(role.GetURL(rootLevel)));
	tblRow.AddCell( CTableCell(role.roleId));
	tblRow.AddCell( CTableCell(this->pInside->LinkToContainer(role.applicationName, rootLevel)));
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(role.modified)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(role.modifiedBy, rootLevel)));
	this->tbl->AddRow(tblRow);
}