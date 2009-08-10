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
#include "ARObject.h"
#include "..\output\webpage.h"

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
	return _itoa(this->insideId, buffer, 10);
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