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

#include "stdafx.h"
#include "ARSchemaList.h"
#include "../ARInside.h"
#include "../output/WebUtil.h"
#include "BlackList.h"

CARSchemaList::CARSchemaList(void)
{
	internalListState = CARSchemaList::EMPTY;
	reservedSize = 0;
	names.numItems = 0;
	sortedList = NULL;

	ARZeroMemory(&names);
	ARZeroMemory(&compounds);
	ARZeroMemory(&permissions);
	ARZeroMemory(&subAdmins);
	ARZeroMemory(&resultFields);
	ARZeroMemory(&sortings);
	ARZeroMemory(&indexes);
	ARZeroMemory(&archives);
	ARZeroMemory(&audits);
	ARZeroMemory(&defaultVUIs);
	ARZeroMemory(&helpTexts);
	ARZeroMemory(&changedTimes);
	ARZeroMemory(&owners);
	ARZeroMemory(&changedUsers);
	ARZeroMemory(&changeDiary);
	ARZeroMemory(&objProps);
}

CARSchemaList::~CARSchemaList(void)
{
	if (internalListState == CARSchemaList::INTERNAL_ALLOC)
	{
		try
		{
			delete[] names.nameList;
			delete[] compounds.compoundSchema;
			delete[] permissions.permissionList;
			delete[] subAdmins.internalIdListList;
			delete[] resultFields.listFieldList;
			delete[] sortings.sortListList;
			delete[] indexes.indexListList;
			delete[] archives.archiveInfoList;
			delete[] audits.auditInfoList;
			delete[] defaultVUIs.nameList;
			delete[] helpTexts.stringList;
			delete[] changedTimes.timestampList;
			delete[] owners.nameList;
			delete[] changedUsers.nameList;
			delete[] changeDiary.stringList;
			delete[] objProps.propsList;
		}
		catch (...)
		{
		}
	}
	else if (internalListState == CARSchemaList::ARAPI_ALLOC)
	{		
		try
		{
			FreeARNameList(&names,false);
			FreeARCompoundSchemaList(&compounds,false);
			FreeARPermissionListList(&permissions,false);
			FreeARInternalIdListList(&subAdmins,false);
			FreeAREntryListFieldListList(&resultFields,false);
			FreeARSortListList(&sortings,false);
			FreeARIndexListList(&indexes,false);
			FreeARArchiveInfoList(&archives,false);
			FreeARAuditInfoList(&audits,false);
			FreeARNameList(&defaultVUIs,false);
			FreeARTextStringList(&helpTexts,false);
			FreeARTimestampList(&changedTimes,false);
			FreeARAccessNameList(&owners,false);
			FreeARAccessNameList(&changedUsers,false);
			FreeARTextStringList(&changeDiary,false);
			FreeARPropListList(&objProps,false);

			for_each(fieldLists.begin(), fieldLists.end(), DeletePointer<CARFieldList>());
			for_each(vuiLists.begin(), vuiLists.end(), DeletePointer<CARVUIList>());
		}
		catch (...)
		{

		}
	}
	if (sortedList != NULL)
		delete sortedList;
}

bool CARSchemaList::LoadFromServer()
{
	ARBooleanList   schemaExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();
	ARNameList*     objectsToLoad = NULL;
	ARNameList      objectNames;
	unsigned int    originalObjectNameCount = 0;
	bool            funcResult = false;

	memset(&schemaExists, 0, sizeof(schemaExists));
	memset(&status, 0, sizeof(status));

	// if the blacklist contains schemas, we should first load all names from the
	// server and remove those that are contained in the blacklist. after that call
	// ARGetMultipleSchemas to retrieve just the needed objects.
	if (arIn->blackList.GetCountOf(ARREF_SCHEMA) > 0)
	{
		memset(&objectNames, 0, sizeof(objectNames));

		if (ARGetListSchema(&arIn->arControl, NULL, AR_LIST_SCHEMA_ALL | AR_HIDDEN_INCREMENT, NULL, NULL, NULL, &objectNames, &status) == AR_RETURN_OK)
		{
			originalObjectNameCount = objectNames.numItems;
			arIn->blackList.Exclude(ARREF_SCHEMA, &objectNames);
			objectsToLoad = &objectNames;
		}
		else
			cerr << arIn->GetARStatusError(&status);
	}

	// ok, now retrieve all informations of the schemas we need
	if (ARGetMultipleSchemas(&arIn->arControl,
		0, // changed since
		NULL, // schema type list
		objectsToLoad,
		NULL, // field id list
		&schemaExists,
		&names,
		&compounds,
		NULL, // schema inheritance list ... reserved for future use
		&permissions,
		&subAdmins,
		&resultFields,
		&sortings,
		&indexes,
		&archives,
		&audits,
		&defaultVUIs,
		&helpTexts,
		&changedTimes,
		&owners,
		&changedUsers,
		&changeDiary,
		&objProps,
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&schemaExists, false);
		internalListState = CARSchemaList::ARAPI_ALLOC;

		sortedList = new vector<int>();
		sortedList->reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
			appRefNames.push_back("");
			sortedList->push_back(i);
		}
		funcResult = true;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
	}

	// check if we have to clean up the name list
	if (originalObjectNameCount > 0)
	{
		objectNames.numItems = originalObjectNameCount;
		FreeARNameList(&objectNames, false);
	}

	// next, loading fields...	
	unsigned int count = names.numItems;
	fieldLists.reserve(count);

	for (unsigned int schemaIndex = 0; schemaIndex < count; ++schemaIndex)
	{
		CARFieldListServer *srvFieldList = new CARFieldListServer(schemaIndex);
		srvFieldList->LoadFromServer(); // TODO: maybe check if successfully loaded
		srvFieldList->Sort();
		fieldLists.push_back(srvFieldList);
	}

	// last, load vuis
	vuiLists.reserve(count);

	for (unsigned int schemaIndex = 0; schemaIndex < count; ++schemaIndex)
	{
		CARVUIListServer *srvVuiList = new CARVUIListServer(schemaIndex);
		srvVuiList->LoadFromServer(); // TODO: maybe check if successfully loaded
		srvVuiList->Sort();
		vuiLists.push_back(srvVuiList);
	}

	// Sort
	Sort();

	return funcResult;
}

void CARSchemaList::Reserve(unsigned int size)
{
	if (internalListState != CARSchemaList::EMPTY) throw AppException("object isnt reusable!", "SchemaList");

	sortedList = new vector<int>();
	sortedList->reserve(size);

	names.numItems = 0;
	names.nameList = new ARNameType[size];

	compounds.numItems = 0;
	compounds.compoundSchema = new ARCompoundSchema[size];

	permissions.numItems = 0;
	permissions.permissionList = new ARPermissionList[size];

	subAdmins.numItems = 0;
	subAdmins.internalIdListList = new ARInternalIdList[size];

	resultFields.numItems = 0;
	resultFields.listFieldList = new AREntryListFieldList[size];

	sortings.numItems = 0;
	sortings.sortListList = new ARSortList[size];

	indexes.numItems = 0;
	indexes.indexListList = new ARIndexList[size];

	archives.numItems = 0;
	archives.archiveInfoList = new ARArchiveInfoStruct[size];

	audits.numItems = 0;
	audits.auditInfoList = new ARAuditInfoStruct[size];

	defaultVUIs.numItems = 0;
	defaultVUIs.nameList = new ARNameType[size];

	helpTexts.numItems = 0;
	helpTexts.stringList = new char*[size];

	changedTimes.numItems = 0;
	changedTimes.timestampList = new ARTimestamp[size];

	owners.numItems = 0;
	owners.nameList = new ARAccessNameType[size];

	changedUsers.numItems = 0;
	changedUsers.nameList = new ARAccessNameType[size];

	changeDiary.numItems = 0;
	changeDiary.stringList = new char*[size];

	objProps.numItems = 0;
	objProps.propsList = new ARPropList[size];
	
	appRefNames.reserve(size);
	fieldLists.reserve(size);
	vuiLists.reserve(size);

	reservedSize = size;
	internalListState = CARSchemaList::INTERNAL_ALLOC;
}

int CARSchemaList::AddSchemaFromXML(ARXMLParsedStream &stream, const char* schemaName, unsigned int *outDocVersion)
{
	if (internalListState != CARSchemaList::INTERNAL_ALLOC) throw AppException("illegal usage!", "SchemaList");
	if (names.numItems >= reservedSize) return -1;
	if (outDocVersion != NULL) *outDocVersion = 0;
	
	CARInside* arIn = CARInside::GetInstance();
	ARNameType appBlockName; appBlockName[0] = 0;
	ARStatusList status;

	unsigned int arDocVersion = 0;
	unsigned int index = names.numItems;
	strncpy(names.nameList[index], schemaName, AR_MAX_NAME_SIZE);	// copy name over
	names.nameList[index][AR_MAX_NAME_SIZE] = 0;

	// create the sub-lists
	CARFieldListXML* fldList = new CARFieldListXML();
	CARVUIListXML* vuiList = new CARVUIListXML();

	if (ARGetSchemaFromXML(&arIn->arControl,
		&stream,
		names.nameList[index],
		appBlockName,
		&compounds.compoundSchema[index],
		&permissions.permissionList[index],
		&subAdmins.internalIdListList[index],
		&resultFields.listFieldList[index],
		&sortings.sortListList[index],
		&indexes.indexListList[index],
		&archives.archiveInfoList[index],
		&audits.auditInfoList[index],
		defaultVUIs.nameList[index],
		NULL, // next fieldid
		NULL,	// core version
		NULL,	// upgrade version
		&fldList->fieldInfo,
		&vuiList->vuiList,
		owners.nameList[index],
		changedUsers.nameList[index],
		&changedTimes.timestampList[index],
		&helpTexts.stringList[index],
		&changeDiary.stringList[index],
		&objProps.propsList[index],
		&arDocVersion,
		&status) == AR_RETURN_OK)
	{
		++names.numItems;
		++compounds.numItems;
		++permissions.numItems;
		++subAdmins.numItems;
		++resultFields.numItems;
		++sortings.numItems;
		++indexes.numItems;
		++archives.numItems;
		++audits.numItems;
		++defaultVUIs.numItems;
		++helpTexts.numItems;
		++changedTimes.numItems;
		++owners.numItems;
		++changedUsers.numItems;
		++changeDiary.numItems;
		++objProps.numItems;

		// signal finished loading to sub-objects 
		fldList->FinishLoading();
		vuiList->FinishLoading();

		fieldLists.push_back(fldList);
		vuiLists.push_back(vuiList);

		sortedList->push_back(index);
		appRefNames.push_back("");

		if (outDocVersion != NULL) *outDocVersion = arDocVersion;

		return index;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return -1;
	}
}

int CARSchemaList::Find(const char* name)
{
	unsigned int count = GetCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		int result = strcoll(names.nameList[(*sortedList)[i]], name);
		if (result == 0)
		{
			return result;
		}
		else if (result > 0)	
			// the current string in the sorted list is greater as the string we are looking for.
			// stop searching here.
			break;
	}
	return -1;
}

void CARSchemaList::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList->begin(),sortedList->end(),SortByName<CARSchemaList>(*this));
}

string CARSchemaList::SchemaGetURL(unsigned int index, int rootLevel)
{
	stringstream strmTmp;
	strmTmp << CWebUtil::RootPath(rootLevel) << "escalation/" << index << "/" << CWebUtil::DocName("index");
	return CWebUtil::Link(SchemaGetName(index), strmTmp.str(), "escalation.gif", rootLevel); 
}
