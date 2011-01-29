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
#include "GroupTable.h"
#include "../core/ARGroup.h"
#include "../core/ARRole.h"

using namespace OUTPUT;

CGroupTable::CGroupTable(CARInside &arIn)
: CObjectTable("groupList", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn(0, "Name");
	tbl.AddColumn(0, "ID");
	tbl.AddColumn(0, "Type");
	tbl.AddColumn(0, "Category");
	tbl.AddColumn(0, "Modified");
	tbl.AddColumn(0, "By");
}

CGroupTable::~CGroupTable(void)
{
}

void CGroupTable::AddRow(string appRefName, int id, int rootLevel)
{
	if(id < 0)
		this->AddRoleRow(appRefName, id, rootLevel);
	else
		this->AddGroupRow(appRefName, id, rootLevel);
}

void CGroupTable::AddRoleRow(string appRefName, int roleId, int rootLevel)
{
	CARRole role(roleId, appRefName);

	if(role.Exists())
	{
		CTableRow tblRow("");
		tblRow.AddCell( CTableCell("Role"));
		tblRow.AddCell( CTableCell(role.GetURL(rootLevel)));	
		tblRow.AddCell( CTableCell(role.GetRoleId()));
		tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(role.GetTimestamp())));
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(role.GetLastChanged(), rootLevel)));
		this->tbl.AddRow(tblRow);
	}
	else
	{
		CTableRow tblRow("");

		stringstream strmName, strmId;
		strmName << roleId << " (not loaded)";
		strmId << roleId;

		tblRow.AddCell( CTableCell("Role"));
		tblRow.AddCell( CTableCell(strmName.str()));	
		tblRow.AddCell( CTableCell(strmId.str()));
		tblRow.AddCell( CTableCell(EmptyValue)); // Timestamp
		tblRow.AddCell( CTableCell(EmptyValue)); // LastChangedBy
		this->tbl.AddRow(tblRow);
	}
}

// TODO: check calls to this function; maybe they have already found the group and this function searches again
void CGroupTable::AddGroupRow(string appRefName, int groupId, int rootLevel)
{
	// search group by id
	CARGroup insertGrp(-1, groupId);

	if(insertGrp.Exists())
	{
		CTableRow tblRow("");
		tblRow.AddCell( CTableCell(this->pInside->LinkToGroup(appRefName, insertGrp.GetGroupId(), rootLevel)));	
		tblRow.AddCell( CTableCell(insertGrp.GetGroupId()));
		tblRow.AddCell( CTableCell(CAREnum::GroupType(insertGrp.GetType())));
		tblRow.AddCell( CTableCell(CAREnum::GroupCategory(insertGrp.GetCategory())));
		tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(insertGrp.GetTimestamp())));
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(insertGrp.GetLastChanged(), rootLevel)));
		this->tbl.AddRow(tblRow);
	}
	else
	{
		CTableRow tblRow("");

		stringstream strmName, strmId;
		strmName << groupId << " (not loaded)";
		strmId << groupId;

		tblRow.AddCell( CTableCell(strmName.str()));// Name
		tblRow.AddCell( CTableCell(strmId.str()));  // Id
		tblRow.AddCell( CTableCell(EmptyValue));    // Type
		tblRow.AddCell( CTableCell(EmptyValue));    // Category
		tblRow.AddCell( CTableCell(EmptyValue));    // Timestamp
		tblRow.AddCell( CTableCell(EmptyValue));    // LastChangedBy
		this->tbl.AddRow(tblRow);
	}	
}
