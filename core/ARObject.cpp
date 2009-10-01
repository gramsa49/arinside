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
#include "ARObject.h"
#include "../output/WebPage.h"

using namespace OUTPUT;

CARObject::CARObject(void)
{
	this->insideId = 0;
	this->name = "";
}

CARObject::~CARObject(void)
{
}

string CARObject::FileID()
{
	char buffer[20];
	// use sprintf instead of _itoa to make it work under linux. 
	// Linux doesnt come with an implementation for itoa.
	sprintf(buffer,"%d", insideId);	
	//return _itoa(this->insideId, buffer, 10);
	return buffer;
}

string CARObject::GetNameFirstChar()
{
	return CUtil::String2Comp(std::string(1, name.at(0)));
}


bool CARObject::NameStandardFirstChar()
{
	string strValue = "abcdefghijklmnopqrstuvwxyz0123456789";
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{
		if(this->GetNameFirstChar() == std::string(1, strValue.at(i)))
			return true;
	}

	return false;
}
