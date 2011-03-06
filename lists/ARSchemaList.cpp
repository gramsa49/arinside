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
			if (changedTimes.numItems > 0 && changedTimes.timestampList[0] != 0) // <APIBUG>
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
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
		NULL, // groupListList // TODO: support static inherited permissions
#endif
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

		sortedList.reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
			appRefNames.push_back("");
			sortedList.push_back(i);
		}
		funcResult = true;

		// <APIBUG>
		//
		// This is just a workarround for a bug in ARAPI 7.6.03/7.6.04
		// 
		// Notes: ClientAPI 7.6.04 (32bit) => Server 7.6.04 (64bit) working
		//        ClientAPI 7.5.00 (32bit) => Server 7.6.04 (64bit) failed
		//        ClientAPI 7.6.04 (32bit) => Server 7.5.00 (32bit) failed
		//        ClientAPI 7.6.04 (64bit) => Server 7.6.04 (64bit) working
		//        ClientAPI 7.6.04 (64bit) => Server 7.5.00 (32bit) failed
		// You can use the driver utility to reproduce the error. (Sequence: init, log, ssp, ver, gms => crash)
		if (changedTimes.numItems == 0 && names.numItems > 0)
		{
			changedTimes.timestampList = new ARTimestamp[names.numItems];
			changedTimes.numItems = names.numItems;
			memset(changedTimes.timestampList, 0, sizeof(ARTimestamp) * names.numItems);

			cout << "NOTE: ARAPI bug detected. Modify timestamps of schemas will not be available!" << endl;
		}
		// </APIBUG>
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

	missingFieldReferences.resize(count);
	memset(&missingFieldReferences[0], 0, sizeof(MissingReferenceList*) * count);

	// reserve reference lists
	activeLinks.resize(count);
	filters.resize(count);
	escalations.resize(count);
	alGuides.resize(count);
	fltGuides.resize(count);
	packLists.resize(count);
	webservices.resize(count);

	// Sort
	Sort();

	return funcResult;
}

void CARSchemaList::Reserve(unsigned int size)
{
	if (internalListState != CARSchemaList::EMPTY) throw AppException("object isnt reusable!", "SchemaList");

	sortedList.reserve(size);
	missingFieldReferences.resize(size);

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

	// reserve reference lists
	activeLinks.resize(size);
	filters.resize(size);
	escalations.resize(size);
	alGuides.resize(size);
	fltGuides.resize(size);
	packLists.resize(size);
	webservices.resize(size);

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

		sortedList.push_back(index);
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
	CMapType::const_iterator it = searchList.find(string(name));
	if (it == searchList.end()) return -1;
	return it->second;
}

void CARSchemaList::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList.begin(),sortedList.end(),SortByName<CARSchemaList>(*this));

	// setup lookup map
	if (!searchList.empty()) searchList.clear();
	for (unsigned int i = 0; i < sortedList.size(); ++i)
	{
		searchList[string(names.nameList[sortedList[i]])] = i;
	}
}

void CARSchemaList::SchemaAddMissingFieldReference(unsigned int index, int fieldId, const CRefItem &refItem)
{
	if (fieldId <= 0) return;	// dont add keyword references (like $-6$ ($SERVER$))
	MissingReferenceList* list = missingFieldReferences[sortedList[index]];
	if (list == NULL)
	{
		list = new MissingReferenceList();
		missingFieldReferences[sortedList[index]] = list;
	}

	if (list->size() > 0)
	{
		// check if reference is already there
		MissingReferenceList::iterator curIt = list->begin();
		MissingReferenceList::iterator endIt = list->end();
		for (; curIt != endIt; ++curIt)
			if (curIt->first == fieldId && 
				curIt->second == refItem) return;
	}
	list->push_back(MissingReferenceItem(fieldId, refItem));
}

const CARSchemaList::MissingReferenceList* CARSchemaList::SchemaGetMissingReferences(unsigned int index)
{
	return missingFieldReferences[sortedList[index]];
}

void CARSchemaList::SortReferences()
{
	size_t listCount = activeLinks.size();
	for (unsigned int schemaIndex = 0; schemaIndex < listCount; ++schemaIndex)
	{
		std::sort(activeLinks[schemaIndex].begin(), activeLinks[schemaIndex].end());
		std::sort(    filters[schemaIndex].begin(),     filters[schemaIndex].end());
		std::sort(escalations[schemaIndex].begin(), escalations[schemaIndex].end());
		std::sort(   alGuides[schemaIndex].begin(),    alGuides[schemaIndex].end());
		std::sort(  fltGuides[schemaIndex].begin(),   fltGuides[schemaIndex].end());
		std::sort(  packLists[schemaIndex].begin(),   packLists[schemaIndex].end());
		std::sort(webservices[schemaIndex].begin(), webservices[schemaIndex].end());
	}
}

void CARSchemaList::SchemaAddFilter(unsigned int index, const CARFilter &filter)
{
	filters[index].push_back(filter.GetInsideId());
}

void CARSchemaList::SchemaAddActiveLink(unsigned int index, const CARActiveLink &actlink)
{
	activeLinks[index].push_back(actlink.GetInsideId());
}

void CARSchemaList::SchemaAddEscalation(unsigned int index, const CAREscalation &escalation)
{
	escalations[index].push_back(escalation.GetInsideId());
}

void CARSchemaList::SchemaAddActLinkGuide(unsigned int index, const CARContainer &alGuide)
{
	alGuides[index].push_back(alGuide.GetInsideId());
}

void CARSchemaList::SchemaAddFilterGuide(unsigned int index, const CARContainer &fltGuide)
{
	fltGuides[index].push_back(fltGuide.GetInsideId());
}

void CARSchemaList::SchemaAddPackingList(unsigned int index, const CARContainer &packList)
{
	packLists[index].push_back(packList.GetInsideId());
}

void CARSchemaList::SchemaAddWebservice(unsigned int index, const CARContainer &webservice)
{
	webservices[index].push_back(webservice.GetInsideId());
}
