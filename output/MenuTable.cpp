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
: CObjectTable("charmenuList", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn(40, "CharMenu Name");
	tbl.AddColumn(10, "Type");
	tbl.AddColumn(10, "Refresh On");
	tbl.AddColumn(20, "Changed");
	tbl.AddColumn(20, "By");
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
	tblRow.AddCell( CTableCell(CAREnum::MenuType(menu.menuDefn.menuType)));
	tblRow.AddCell( CTableCell(CAREnum::MenuRefresh(menu.refreshCode)));
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(menu.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(menu.lastChanged, rootLevel)));

	this->tbl.AddRow(tblRow);
}

int CMenuTable::NumRelatedFields(CARCharMenu &obj)
{	
	int nCnt = 0;

	unsigned int schemaCount = this->pInside->schemaList.GetCount();
	for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
	{			
		CARSchema schema(schemaIndex);
		
		unsigned int fieldCount = schema.GetFields()->GetCount();		
		for(unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schemaIndex, 0, fieldIndex);

			if(field.GetDataType() == AR_DATA_TYPE_CHAR)
			{
				if (field.GetLimits().u.charLimits.charMenu == obj.name)
				{
					nCnt++;
				}
			}
		}		
	}
	return nCnt;
}

int CMenuTable::NumRelatedActiveLinks(CARCharMenu &obj)
{
	int nCnt = 0;

	unsigned int alCount = this->pInside->alList.GetCount();
	for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex)
	{			
		CARActiveLink al(alIndex);
		for(unsigned int nAction = 0; nAction < al.GetIfActions().numItems; nAction++)
		{
			const ARActiveLinkActionStruct &action = al.GetIfActions().actionList[nAction];
			if(action.action == AR_ACTIVE_LINK_ACTION_SET_CHAR)
			{
				if(action.u.characteristics.charMenu != NULL)
				{
					if(strcmp(action.u.characteristics.charMenu, obj.name.c_str())==0)
					{
						nCnt++;
					}
				}
			}
		}

		for(unsigned int nAction = 0; nAction < al.GetElseActions().numItems; nAction++)
		{
			const ARActiveLinkActionStruct &action = al.GetElseActions().actionList[nAction];
			if(action.action == AR_ACTIVE_LINK_ACTION_SET_CHAR)
			{
				if(action.u.characteristics.charMenu != NULL)
				{
					if(strcmp(action.u.characteristics.charMenu, obj.name.c_str())==0)
					{
						nCnt++;
					}
				}
			}
		}		
	}
	return nCnt;
}