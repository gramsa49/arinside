//Copyright (C) 2014 John Luthgers | jls17
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
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "DocCharacterMenu.h"
#include "../../core/ARCharMenu.h"
#include "../../core/AREnum.h"
#include "../../output/Table.h"

using namespace OUTPUT;

CDocCharacterMenu::CDocCharacterMenu(CTable &table, CARCharMenu &menuObj)
: outputTable(table), menu(menuObj), itemTable(new OUTPUT::CTable("menuItems", "TblObjectList")), itemStrm(new stringstream()), valueStrm(new stringstream())
{
	itemTable->AddColumn(20, "Type");	
	itemTable->AddColumn(40, "Label");
	itemTable->AddColumn(40, "Value");
}

void CDocCharacterMenu::Documentation()
{
	try
	{
		const ARCharMenuStruct &menuDef = menu.GetDefinition();
		assert(menuDef.menuType == AR_CHAR_MENU_LIST);

		*itemStrm  << "<div id='menuItems'>";
		*valueStrm << "<div id='menuItems'>";
		CreateItemList(menuDef.u.menuList);
		*valueStrm << "</div>";
		*itemStrm  << "</div>";

		CreateItemTable();
		
		CTableRow row;
		row.AddCell("Menu Definition");
		row.AddCell(itemTable->ToXHtml());
		outputTable.AddRow(row);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION Menu details doc of '" << this->menu.GetName() << "': " << e.what() << endl;
	}
}

void CDocCharacterMenu::CreateItemTable()
{
	CTableRow row("cssStdRow");
	row.AddCell("&nbsp;");
	row.AddCell(itemStrm->str());
	row.AddCell(valueStrm->str());
	itemTable->AddRow(row);
}

void CDocCharacterMenu::CreateItemList(const ARCharMenuList &menu)
{
	*itemStrm << "<ul>";

	for(unsigned int i=0; i< menu.numItems; i++)
	{
		*itemStrm << "<li>" << menu.charMenuList[i].menuLabel << "</li>";

		switch (menu.charMenuList[i].menuType)
		{
		case AR_MENU_TYPE_VALUE:
			{
				*valueStrm << "<li>" << menu.charMenuList[i].u.menuValue << "</li>";
			}
			break;
		case AR_MENU_TYPE_MENU:
			{
				*valueStrm << "<li>" << "&nbsp;" << "</li>";
				ARCharMenuStruct *subMenu = menu.charMenuList[i].u.childMenu;
				if (subMenu != NULL && subMenu->menuType == AR_CHAR_MENU_LIST)
				{
					CreateItemList(subMenu->u.menuList);
				}
				else
				{
					assert(false);
				}
			}
			break;
		}
	}

	*itemStrm << "</ul>";	
}
