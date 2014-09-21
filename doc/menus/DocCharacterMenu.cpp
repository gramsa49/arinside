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

void CDocCharacterMenu::Documentation()
{
	try
	{
		CTable tbl("menuItems", "TblObjectList");
		tbl.AddColumn(20, "Type");	
		tbl.AddColumn(40, "Label");
		tbl.AddColumn(40, "Value");

		const ARCharMenuList& menu = this->menu.GetDefinition().u.menuList;
		for(unsigned int i=0; i< menu.numItems; i++)
		{
			CTableRow row("cssStdRow");		
			CTableCell cellItemType(CAREnum::MenuItemType(menu.charMenuList[i].menuType), "");				
			CTableCell cellItemLabel(menu.charMenuList[i].menuLabel, "");

			string mValue = "";
			if(menu.charMenuList[i].menuType == AR_MENU_TYPE_VALUE)
				mValue = menu.charMenuList[i]	.u.menuValue;
			CTableCell cellItemValue(mValue, "");

			row.AddCell(cellItemType);
			row.AddCell(cellItemLabel);
			row.AddCell(cellItemValue);
			tbl.AddRow(row);		
		}
		
		CTableRow row;
		row.AddCell("Menu Definition");
		row.AddCell(tbl.ToXHtml());
		table.AddRow(row);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION Menu details doc of '" << this->menu.GetName() << "': " << e.what() << endl;
	}
}
