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
};

class CARServerObjectWithData : public CARServerObject
{
public:
	CARServerObjectWithData(int insideId);
	~CARServerObjectWithData();

	// implement function inherited from CARServerObject
	const char* GetHelpText() const { return helptext; }
	ARTimestamp GetTimestamp() { return timestamp; }
	const ARAccessNameType& GetOwner() const { return owner; }
	const ARAccessNameType& GetLastChanged() const { return lastChanged; }
	const char* GetChangeDiary() const { return changeDiary; }

	// implement function inherited from CARObject
	string GetName() { return name; }
	string GetName() const { return name; }
	string GetNameFirstChar() { return CARObject::GetNameFirstChar(name); }
	bool NameStandardFirstChar() { return CARObject::NameStandardFirstChar(name); }

	string              name;
	char                *helptext;
	ARTimestamp         timestamp;
	ARAccessNameType    owner;
	ARAccessNameType    lastChanged;
	char                *changeDiary;		
	unsigned int        xmlDocVersion;
	string              appRefName;
};
