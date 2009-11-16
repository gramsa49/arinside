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
#include "ARCharMenu.h"

CARCharMenu::CARCharMenu(string name, int insideId)
: CARServerObjectWithData(insideId)
{
	this->name = name;
	this->refreshCode = 0;
	this->objPropList.props = NULL;
}

CARCharMenu::~CARCharMenu(void)
{
	// TODO: move ARFree calls to separate method to allow copying of the object without 
	// copying the whole structure
	//try
	//{
	//	FreeARCharMenuStruct(&menuDefn, false);

	//	if(objPropList.props != NULL)
	//		FreeARPropList(&objPropList, false);
	//}
	//catch(...)
	//{
	//}
}

string CARCharMenu::GetURL(int rootLevel, bool showImage)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"menu/"+this->FileID()+"/"+CWebUtil::DocName("index"), (showImage ? "menu.gif" : ""), rootLevel);
}
