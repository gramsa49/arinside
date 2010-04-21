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
#include "ARServerObject.h"

class CARContainer :
	public CARServerObject
{
public:
	CARContainer(int insideId);
	CARContainer(const char* name);
	CARContainer(const string& name);
	~CARContainer(void);

	bool Exists();

	// implement functions inherited from CARObject
	string GetName();
	string GetName() const;
	string GetNameFirstChar();
	bool NameStandardFirstChar();

	// implement functions inherited from CARServerObject
	const char* GetHelpText() const;
	ARTimestamp GetTimestamp();
	const ARAccessNameType& GetOwner() const;
	const ARAccessNameType& GetLastChanged() const;
	const char* GetChangeDiary() const;

	const ARNameType& GetARName() const;
	const ARPermissionList& GetPermissions() const;
	const ARInternalIdList& GetSubadmins() const;
	const ARContainerOwnerObjList& GetOwnerObjects() const;
	char* GetLabel() const;
	char* GetDescription() const;
	unsigned int GetType() const;
	const ARReferenceList& GetReferences() const;
	const ARPropList& GetProps() const;

	const string& GetAppRefName() const;
	void SetAppRefName(const string& appName);

	string GetURL(int rootLevel, bool showImage = true);

	int GetServerObjectTypeXML() { return AR_STRUCT_ITEM_XML_CONTAINER; }
	bool IsClonable();
	CARServerObject* Clone();
private:
};
