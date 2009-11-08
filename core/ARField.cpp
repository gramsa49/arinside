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
#include "ARField.h"

CARField::CARField(int insideId)
: CARServerObjectWithData(insideId)
{	
	this->fieldId = 0;
	this->dataType = 0;
	this->option = 0;
	this->createMode = 0;
}

CARField::~CARField(void)
{ 
	try
	{
		FreeARValueStruct (&defaultVal, false);			
		FreeARFieldLimitStruct (&limit, false);	
		FreeARPermissionList (&permissions, false);	
		FreeARDisplayInstanceList (&dInstanceList, false);
	}
	catch(...)
	{
	}
}

string CARField::GetURL(int rootLevel, bool showImage)
{
	stringstream tmp;
	tmp.str("");

	tmp << CWebUtil::RootPath(rootLevel) << "schema/" << this->schemaInsideId << "/" << CWebUtil::DocName(this->FileID());
	return CWebUtil::Link(this->name, tmp.str(), "", rootLevel);
}
