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
#include "MenuTable.h"

using namespace OUTPUT;

CMenuTable::CMenuTable(CARInside &arIn)
: CObjectTable("menuList", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn(40, "CharMenu Name");
	tbl.AddColumn(10, "Type");
	tbl.AddColumn(10, "Refresh On");
	tbl.AddColumn(20, "Changed");
	tbl.AddColumn(20, "By");

	tbl.SetEmptyMessage("");
}

CMenuTable::~CMenuTable(void)
{
}

void CMenuTable::AddRow(CARCharMenu &menu, int rootLevel)
{
	CTableRow tblRow("");

	string cellNameValue = menu.GetURL(rootLevel);
	if(NumRelatedFields(menu) + NumRelatedActiveLinks(menu) ==0)
	{
		cellNameValue += " (<b>!</b>)";
	}

	tblRow.AddCell( CTableCell(cellNameValue));		
	tblRow.AddCell( CTableCell(CAREnum::MenuType(menu.GetDefinition().menuType)));
	tblRow.AddCell( CTableCell(CAREnum::MenuRefresh(menu.GetRefreshCode())));
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(menu.GetTimestamp())));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(menu.GetLastChanged(), rootLevel)));

	this->tbl.AddRow(tblRow);
}

int CMenuTable::NumRelatedFields(CARCharMenu &obj)
{	
	int count = 0;

	const CARCharMenu::ReferenceList& refs = obj.GetReferences();
	CARCharMenu::ReferenceList::const_iterator curIt = refs.begin();
	CARCharMenu::ReferenceList::const_iterator endIt = refs.end();

	for (; curIt != endIt; ++curIt)
	{
		if (curIt->GetObjectType() == AR_STRUCT_ITEM_XML_FIELD && curIt->GetMessageId() == REFM_FIELD_CHARMENU)
			count++;
	}

	return count;
}

int CMenuTable::NumRelatedActiveLinks(CARCharMenu &obj)
{
	int count = 0;

	const CARCharMenu::ReferenceList& refs = obj.GetReferences();
	CARCharMenu::ReferenceList::const_iterator curIt = refs.begin();
	CARCharMenu::ReferenceList::const_iterator endIt = refs.end();

	for (; curIt != endIt; ++curIt)
	{
		if (curIt->GetObjectType() == AR_STRUCT_ITEM_XML_ACTIVE_LINK)
			count++;
	}

	return count;
}
