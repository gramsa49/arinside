//Copyright (C) 2010 John Luthgers | jls17
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
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.
#pragma once
#include "../ARApi.h"
#include "../util/Uncopyable.h"
#include "ARListHelpers.h"
#include "ARFieldList.h"
#include "ARVUIList.h"
#include <assert.h>

class CARSchemaList : Uncopyable
{
public:
	CARSchemaList(void);
	~CARSchemaList(void);

	// loading...
	bool LoadFromServer();
	int AddSchemaFromXML(ARXMLParsedStream& stream, const char* schemaName, unsigned int *arDocVersion = NULL);

	// list functions
	inline unsigned int GetCount() { return names.numItems; }
	int Find(const char *name);
	void Reserve(unsigned int size);
	void Sort();

	// data-access functions
	const ARNameType& SchemaGetName(unsigned int index) const { assert(index < names.numItems); return names.nameList[(*sortedList)[index]]; }
	const ARCompoundSchema& SchemaGetCompound(unsigned int index) const { assert(index < compounds.numItems); return compounds.compoundSchema[(*sortedList)[index]]; }
	const ARPermissionList& SchemaGetPermissions(unsigned int index) const { assert(index < permissions.numItems); return permissions.permissionList[(*sortedList)[index]]; }
	const ARInternalIdList& SchemaGetSubadmins(unsigned int index) const { assert(index < subAdmins.numItems); return subAdmins.internalIdListList[(*sortedList)[index]]; }
	const AREntryListFieldList& SchemaGetResultFields(unsigned int index) const { assert(index < resultFields.numItems); return resultFields.listFieldList[(*sortedList)[index]]; }
	const ARSortList& SchemaGetSortList(unsigned int index) const { assert(index < sortings.numItems); return sortings.sortListList[(*sortedList)[index]]; }
	const ARIndexList& SchemaGetIndexList(unsigned int index) const { assert(index < indexes.numItems); return indexes.indexListList[(*sortedList)[index]]; }
	const ARArchiveInfoStruct& SchemaGetArchiveInfo(unsigned int index) const { assert(index < archives.numItems); return archives.archiveInfoList[(*sortedList)[index]]; }
	const ARAuditInfoStruct& SchemaGetAuditInfo(unsigned int index) const { assert(index < audits.numItems); return audits.auditInfoList[(*sortedList)[index]]; }
	const ARNameType& SchemaGetDefaultVUI(unsigned int index) const { assert(index < defaultVUIs.numItems); return defaultVUIs.nameList[(*sortedList)[index]]; }
	char* SchemaGetHelptext(unsigned int index) const { assert(index < helpTexts.numItems); return helpTexts.stringList[(*sortedList)[index]]; }
	const ARTimestamp& SchemaGetTimestamp(unsigned int index) const { return changedTimes.timestampList[(*sortedList)[index]]; }
	const ARAccessNameType& SchemaGetOwner(unsigned int index) const { assert(index < owners.numItems); return owners.nameList[(*sortedList)[index]]; }
	const ARAccessNameType& SchemaGetModifiedBy(unsigned int index) const { assert(index < changedUsers.numItems); return changedUsers.nameList[(*sortedList)[index]]; }
	const char* SchemaGetChangeDiary(unsigned int index) const { assert(index < changeDiary.numItems); return changeDiary.stringList[(*sortedList)[index]]; }
	const ARPropList& SchemaGetPropList(unsigned int index) const { assert(index < objProps.numItems); return objProps.propsList[(*sortedList)[index]]; }
	string SchemaGetURL(unsigned int index, int rootLevel);

	const string& SchemaGetAppRefName(unsigned int index) const { assert(index < appRefNames.size()); return appRefNames[(*sortedList)[index]]; }
	void SchemaSetAppRefName(unsigned int index, const string& appName) { assert(index < appRefNames.size()); appRefNames[(*sortedList)[index]] = appName; }

	// access for contained lists
	CARFieldList* SchemaGetFields(unsigned int index) { assert(index < fieldLists.size()); return fieldLists[(*sortedList)[index]]; }
	CARVUIList* SchemaGetVUIs(unsigned int index) { assert(index < vuiLists.size()); return vuiLists[(*sortedList)[index]]; }

	// the sort class needs access the the "names" member variable
	friend class SortByName<CARSchemaList>;
private:
	// allocation state of internal structures
	enum SchemaListState { EMPTY, ARAPI_ALLOC, INTERNAL_ALLOC };

	unsigned int reservedSize;
	ARNameList names;
	ARCompoundSchemaList compounds;
	//ARSchemaInheritanceListList *schemaInheritanceListList; // reserved
	ARPermissionListList permissions;
	ARInternalIdListList subAdmins;
	AREntryListFieldListList resultFields;
	ARSortListList sortings;
	ARIndexListList indexes;
	ARArchiveInfoList archives;
	ARAuditInfoList audits;
	ARNameList defaultVUIs;
	ARTextStringList helpTexts;
	ARTimestampList changedTimes;
	ARAccessNameList owners;
	ARAccessNameList changedUsers;
	ARTextStringList changeDiary;
	ARPropListList objProps;
	vector<string> appRefNames;

	vector<CARFieldList*> fieldLists;
	vector<CARVUIList*> vuiLists;

	SchemaListState internalListState;
	vector<int> *sortedList;	// a index, sorted by schema names
};
