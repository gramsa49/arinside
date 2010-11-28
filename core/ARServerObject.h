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

using namespace OUTPUT;

class CARServerObject : public CARObject
{
public:
	CARServerObject(int insideId) : CARObject(insideId) {}

	virtual const char* GetHelpText() const = 0;
	virtual ARTimestamp GetTimestamp() = 0;
	virtual const ARAccessNameType& GetOwner() const = 0;
	virtual const ARAccessNameType& GetLastChanged() const = 0;
	virtual const char* GetChangeDiary() const = 0;
	virtual string GetURL(int rootLevel, bool showImage = true) = 0;
	virtual int GetServerObjectTypeXML() = 0;

	virtual CARServerObject* Clone() = 0;
	virtual bool IsClonable() = 0;
};
