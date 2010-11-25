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

class CARFieldList;
class CARVUIList;

class CARSchema :
	public CARServerObject
{
public:
	CARSchema();
	CARSchema(int insideId);
	CARSchema(const string& schemaName);
	~CARSchema(void);

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

	// other schema data access functions
	const ARNameType& GetARName() const;
	const ARCompoundSchema& GetCompound() const;
	const ARPermissionList& GetPermissions() const;
	const ARInternalIdList& GetSubadmins() const;
	const AREntryListFieldList& GetResultFields() const;
	const ARSortList& GetSortList() const;
	const ARIndexList& GetIndexList() const;
	const ARArchiveInfoStruct& GetArchiveInfo() const;
	const ARAuditInfoStruct& GetAuditInfo() const;
	const ARNameType& GetDefaultVUI() const;
	const ARPropList& GetProps() const;

	const string& GetAppRefName() const;
	void SetAppRefName(const string& appName);

	CARFieldList* GetFields();
	CARVUIList* GetVUIs();

	// some helpers
	string WebAlias();
	string GetURL(int rootLevel, bool useImage = true);

	string LinkToVui(int vuiId, int fromRootLevel);
	string LinkToVui(string vuiName, int fromRootLevel);
	string VuiGetLabel(int vuiId);

	// class type support
	int GetServerObjectTypeXML() { return AR_STRUCT_ITEM_XML_SCHEMA; }
	bool IsClonable();
	CARServerObject* Clone();
};
