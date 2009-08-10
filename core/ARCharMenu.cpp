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
#include ".\archarmenu.h"

CARCharMenu::CARCharMenu(string name, int insideId)
{
	this->name = name;
	this->insideId = insideId;

	this->refreshCode = 0;

	this->objPropList.props = NULL;
}

CARCharMenu::~CARCharMenu(void)
{
	try
	{
		FreeARCharMenuStruct(&menuDefn, false);

		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
	catch(...)
	{
	}
}

string CARCharMenu::GetURL(int rootLevel)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"menu/"+this->FileID()+"/"+CWebUtil::DocName("index"), "menu.gif", rootLevel);
}