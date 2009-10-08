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
#include "UserTable.h"

using namespace OUTPUT;

CUserTable::CUserTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("userList", "TblObjectList");
	tbl->AddColumn(40, "Login Name");
	tbl->AddColumn(20, "License");
	tbl->AddColumn(20, "Modified");
	tbl->AddColumn(20, "By");
}

CUserTable::~CUserTable(void)
{
}

void CUserTable::AddRow(CARUser &user, int rootLevel)
{	
	try
	{
		CTableRow tblRow("");
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(user.loginName, rootLevel)));
		tblRow.AddCell( CTableCell(CAREnum::UserGetLicType(user.licenseType)));
		tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(user.modified)));
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(user.modifiedBy, rootLevel)));

		this->tbl->AddRow(tblRow);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION adding user '" << user.loginName << "': " << e.what()<< endl;
	}
}
