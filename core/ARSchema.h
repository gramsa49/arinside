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
#include "ARField.h"
#include "ARVui.h"

class CARSchema :
	public CARServerObject
{
public:
	CARSchema();
	CARSchema(string name, int insideId);
public:
	~CARSchema(void);

	string alias;
	ARCompoundSchema  schema;
	ARPermissionList	groupList;
	ARInternalIdList	admingrpList;
	AREntryListFieldList getListFields;
	ARSortList	sortList;
	ARIndexList	indexList;
	ARArchiveInfoStruct	archiveInfo;
	ARAuditInfoStruct auditInfo;
	ARNameType defaultVui;
	ARPropList objPropList;
	string WebAlias();

	list<CARField> fieldList;
	list<CARVui> vuiList;

	string GetURL(int rootLevel);
	string GetURL(int rootLevel, bool useImage);

	string LinkToVui(int vuiId, int fromRootLevel);
	string LinkToVui(string vuiName, int fromRootLevel);
	string VuiGetLabel(int vuiId);
};
