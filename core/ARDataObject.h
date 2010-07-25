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

#pragma once
#include "ARObject.h"
#include "../output/WebUtil.h"
#include "ARDataObjectType.h"

class CARDataObject :
	public CARObject
{
public:
	CARDataObject(int insideId);
	~CARDataObject(void);

	string GetName() { return name; }
	string GetName() const { return name; }
	string GetNameFirstChar() { return CARObject::GetNameFirstChar(name); }
	bool NameStandardFirstChar() { return CARObject::NameStandardFirstChar(name); }

	virtual unsigned int GetDataType() = 0;
	string name;
	string requestId;
	ARTimestamp created;
	string createdBy;
	ARTimestamp modified;
	string modifiedBy;
};
