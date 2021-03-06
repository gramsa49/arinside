//Copyright (C) 2009 John Luthgers | jls17
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
#include "ARServerObject.h"
#include "ARProplistHelper.h"

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

class CARImageList; // forware declaration

class CARImage :
	public CARServerObject
{
public:
	CARImage(int insideId) : CARServerObject(insideId) { }
	~CARImage() { }

	// implement function inherited from CARObject
	string GetName();
	string GetName() const;
	string GetNameFirstChar();
	bool NameStandardFirstChar();

	// implement function inherited from CARServerObject
	const char* GetHelpText() const;
	ARTimestamp GetTimestamp();
	const ARAccessNameType& GetOwner() const;
	const ARAccessNameType& GetLastChanged() const;
	const char* GetChangeDiary() const;

	// implement access functions
	char* GetType() const;
	char* GetDescription() const;
	const ARImageDataStruct& GetData() const;
	
	string GetURL(int rootLevel, bool showImage = true);
	
	// class type support
	int GetServerObjectTypeXML() { return AR_STRUCT_ITEM_XML_IMAGE; }
	bool IsClonable();
	CARServerObject* Clone();
};

#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
