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
#include "ARFieldList.h"
#include "ARListHelpers.h"
#include "../ARInside.h"
#include "../core/ARStatusList.h"
#include "../util/RefItem.h"
#include "support/IndexSorter.h"

///////////////////////////////////////////////////////////////////////////////
// CARFieldList implementation
bool CARFieldList::FieldReferenceExists(unsigned int index, const CRefItem& refItem)
{
	ReferenceList::iterator curIt = workflowReferences[index].begin();
	ReferenceList::iterator endIt = workflowReferences[index].end();

	for (; curIt != endIt; ++curIt)
	{
		if (*curIt == refItem)
		{
			return true;
		}
	}	
	return false;
}

bool CARFieldList::FieldReferenceAdd(unsigned int index, const CRefItem &refItem)
{
	if (refItem.GetObjectType() != AR_STRUCT_ITEM_XML_NONE)
	{
		workflowReferences[index].push_back(refItem);
		return true;
	}
	return false;
}

size_t CARFieldList::FieldReferenceCount(unsigned int index)
{
	return workflowReferences[index].size();
}

const CARFieldList::ReferenceList& CARFieldList::FieldReferenceList(unsigned int index)
{
	return workflowReferences[index];
}

///////////////////////////////////////////////////////////////////////////////
// CARFieldListXML implementation
void CARFieldListXML::FinishLoading()
{
	if (sortedList.empty() && fieldInfo.numItems > 0)
	{
		for (unsigned int i = 0; i < fieldInfo.numItems; ++i)
			sortedList.push_back(i);
	}

	workflowReferences.resize(fieldInfo.numItems);

	Sort();
}

void CARFieldListXML::Sort()
{
	if (GetCount() > 0)
	{
		IndexSorter indexSorter(CARInside::GetInstance()->appConfig.bUseUtf8);
		indexSorter.SortBy(this->fieldInfo).Sort(sortedList);
	}
}


int CARFieldListXML::Find(const char *name)
{
	unsigned int count = GetCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		int result = strcoll(fieldInfo.fieldList[sortedList[i]].fieldName, name);
		if (result == 0)
		{
			return i;
		}
		else if (result > 0)
			// the current string in the sorted list is greater as the string we are looking for.
			// stop searching here.
			break;
	}
	return -1;
}

int CARFieldListXML::Find(unsigned int fieldId)
{
	unsigned int count = GetCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		if (fieldInfo.fieldList[sortedList[i]].fieldId == fieldId)
			return i;
	}
	return -1;
}

const ARPropList& CARFieldListXML::FieldGetPropList(unsigned int index) const
{
#if AR_CURRENT_API_VERSION < AR_API_VERSION_763
	return emptyPropList;
#else
	return fieldInfo.fieldList[sortedList[index]].objPropList; 
#endif
}

///////////////////////////////////////////////////////////////////////////////
// CARFieldListServer implementation
CARFieldListServer::CARFieldListServer(unsigned int schemaInsideId)
{
	schemaId = schemaInsideId;
	ClearAllStructs();
}

CARFieldListServer::~CARFieldListServer()
{
	FreeARNameList(&names, false);
	FreeARInternalIdList(&fieldIds, false);
	FreeARFieldMappingList(&fieldMaps, false);
	FreeARUnsignedIntList(&dataTypes, false);
	FreeARUnsignedIntList(&options, false);
	FreeARUnsignedIntList(&createModes, false);
	FreeARUnsignedIntList(&fieldOptions, false);
	FreeARValueList(&defaultValues, false);
	FreeARPermissionListList(&permLists, false);
	FreeARFieldLimitList(&limits, false);
	FreeARDisplayInstanceListList(&dInstanceLists, false);
	FreeARTextStringList(&helpTexts, false);
	FreeARTimestampList(&changedTimes, false);
	FreeARAccessNameList(&owners, false);
	FreeARAccessNameList(&changedUsers, false);
	FreeARTextStringList(&changeDiary, false);
}

void CARFieldListServer::ClearAllStructs()
{
	ARZeroMemory(&names);
	ARZeroMemory(&fieldIds);
	ARZeroMemory(&fieldMaps);
	ARZeroMemory(&dataTypes);
	ARZeroMemory(&options);
	ARZeroMemory(&createModes);
	ARZeroMemory(&fieldOptions);
	ARZeroMemory(&defaultValues);
	ARZeroMemory(&permLists);
	ARZeroMemory(&limits);
	ARZeroMemory(&dInstanceLists);
	ARZeroMemory(&helpTexts);
	ARZeroMemory(&changedTimes);
	ARZeroMemory(&owners);
	ARZeroMemory(&changedUsers);
	ARZeroMemory(&changeDiary);
}

bool CARFieldListServer::LoadFromServer()
{
	ARBooleanList   fldExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();
	ARNameType			schemaName; memcpy(schemaName, arIn->schemaList.SchemaGetName(schemaId), sizeof(ARNameType));

	memset(&fldExists, 0, sizeof(ARBooleanList));
	memset(&status, 0, sizeof(status));	

	if (!arIn->appConfig.slowObjectLoading && ARGetMultipleFields(&arIn->arControl,
		schemaName, 
		NULL, // all field ids
		&fldExists,
		&fieldIds,
		&names,
		&fieldMaps,
		&dataTypes,
		&options,
		&createModes,
		&fieldOptions,
		&defaultValues,
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
		NULL, // groupListList // TODO: support static inherited permissions
#endif
		&permLists,
		&limits,
		&dInstanceLists,
		&helpTexts,
		&changedTimes,
		&owners,
		&changedUsers,
		&changeDiary,
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
		&objProps,
#endif
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&fldExists, false);
		BuildIndex();
		return true;
	}
	else
	{
		cerr << BuildMessageAndFreeStatus(status);

		// ok, fallback to slow data retrieval
		// this could be necessaray if there is a corrupt field that keeps us from getting all fields at once
		if (!arIn->appConfig.slowObjectLoading)
			cout << "WARN: switching to slow field loading for: " << schemaName << endl;

		ARInternalIdList fieldIdList;
		ARZeroMemory(&fieldIdList);

		if (ARGetListField(&arIn->arControl, schemaName, AR_FIELD_TYPE_ALL, 0, 
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
			NULL, 
#endif
			&fieldIdList, &status) == AR_RETURN_OK)
		{
			Reserve(fieldIdList.numItems);

			unsigned int currentWriteIndex = 0;
			for (unsigned int idx = 0; idx < fieldIdList.numItems; ++idx)
			{
				if (ARGetField(&arIn->arControl, schemaName, fieldIdList.internalIdList[idx],
					names.nameList[currentWriteIndex],
					&fieldMaps.mappingList[currentWriteIndex],
					&dataTypes.intList[currentWriteIndex],
					&options.intList[currentWriteIndex],
					&createModes.intList[currentWriteIndex],
					&fieldOptions.intList[currentWriteIndex],
					&defaultValues.valueList[currentWriteIndex],
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
					NULL,
#endif
					&permLists.permissionList[currentWriteIndex],
					&limits.fieldLimitList[currentWriteIndex],
					&dInstanceLists.dInstanceList[currentWriteIndex],
					&helpTexts.stringList[currentWriteIndex],
					&changedTimes.timestampList[currentWriteIndex],
					owners.nameList[currentWriteIndex],
					changedUsers.nameList[currentWriteIndex],
					&changeDiary.stringList[currentWriteIndex],
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
					&objProps.propsList[currentWriteIndex],
#endif
					&status) == AR_RETURN_OK)
				{
					fieldIds.internalIdList[currentWriteIndex++] = fieldIdList.internalIdList[idx];
				}
				else
					cerr << "Schema: " << schemaName << " -- Failed to load field " << fieldIdList.internalIdList[idx] << ": " << BuildMessageAndFreeStatus(status);
			}
			SetNumItems(currentWriteIndex);
			BuildIndex();
			return true;
		}
		else
		{
			cerr << BuildMessageAndFreeStatus(status);
			return false;
		}
	}
}

int CARFieldListServer::Find(const char *name)
{
	unsigned int count = GetCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		int result = strcoll(names.nameList[sortedList[i]], name);
		if (result == 0)
		{
			return i;
		}
		else if (result > 0)
			// the current string in the sorted list is greater as the string we are looking for.
			// stop searching here.
			break;
	}
	return -1;
}

int CARFieldListServer::Find(unsigned int fieldId)
{
	unsigned int count = GetCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		if (fieldIds.internalIdList[sortedList[i]] == fieldId)
			return i;
	}
	return -1;
}

void CARFieldListServer::Sort()
{
	if (GetCount() > 0)
	{
		IndexSorter indexSorter(CARInside::GetInstance()->appConfig.bUseUtf8);
		indexSorter.SortBy(names).Sort(sortedList);
	}
}

const ARPropList& CARFieldListServer::FieldGetPropList(unsigned int index) const
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	if (objProps.numItems == 0)
		return emptyPropList;
	return objProps.propsList[sortedList[index]];
#else
	return emptyPropList;
#endif
}

void CARFieldListServer::Reserve(unsigned int amount)
{
	unsigned int objAllocIndex = 0;
	try
	{
		names.nameList = new ARNameType[amount];
		ARZeroMemory(names.nameList, sizeof(ARNameType) * amount);
		objAllocIndex++;

		fieldIds.internalIdList = new ARInternalId[amount];
		ARZeroMemory(fieldIds.internalIdList, sizeof(ARInternalId) * amount);
		objAllocIndex++;

		fieldMaps.mappingList = new ARFieldMappingStruct[amount];
		ARZeroMemory(fieldMaps.mappingList, sizeof(ARFieldMappingStruct) * amount);
		objAllocIndex++;

		dataTypes.intList = new unsigned int[amount];
		ARZeroMemory(dataTypes.intList, sizeof(unsigned int) * amount);
		objAllocIndex++;

		options.intList = new unsigned int[amount];
		ARZeroMemory(options.intList, sizeof(unsigned int) * amount);
		objAllocIndex++;

		createModes.intList = new unsigned int[amount];
		ARZeroMemory(createModes.intList, sizeof(unsigned int) * amount);
		objAllocIndex++;
		
		fieldOptions.intList = new unsigned int[amount];
		ARZeroMemory(fieldOptions.intList, sizeof(unsigned int) * amount);
		objAllocIndex++;

		defaultValues.valueList = new ARValueStruct[amount];
		ARZeroMemory(defaultValues.valueList, sizeof(ARValueStruct) * amount);
		objAllocIndex++;

		permLists.permissionList = new ARPermissionList[amount];
		ARZeroMemory(permLists.permissionList, sizeof(ARPermissionList) * amount);
		objAllocIndex++;

		limits.fieldLimitList = new ARFieldLimitStruct[amount];
		ARZeroMemory(limits.fieldLimitList, sizeof(ARFieldLimitStruct) * amount);
		objAllocIndex++;

		dInstanceLists.dInstanceList = new ARDisplayInstanceList[amount];
		ARZeroMemory(dInstanceLists.dInstanceList, sizeof(ARDisplayInstanceList) * amount);
		objAllocIndex++;

		helpTexts.stringList = new char*[amount];
		ARZeroMemory(helpTexts.stringList, sizeof(char*) * amount);
		objAllocIndex++;

		changedTimes.timestampList = new ARTimestamp[amount];
		ARZeroMemory(changedTimes.timestampList, sizeof(ARTimestamp) * amount);
		objAllocIndex++;

		owners.nameList = new ARAccessNameType[amount];
		ARZeroMemory(owners.nameList, sizeof(ARAccessNameType) * amount);
		objAllocIndex++;

		changedUsers.nameList = new ARAccessNameType[amount];
		ARZeroMemory(changedUsers.nameList, sizeof(ARAccessNameType) * amount);
		objAllocIndex++;

		changeDiary.stringList = new char*[amount];
		ARZeroMemory(changeDiary.stringList, sizeof(char*) * amount);
		objAllocIndex++;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
		objProps.propsList = new ARPropList[amount];
		ARZeroMemory(objProps.propsList, sizeof(ARPropList) * amount);
		objAllocIndex++;
#endif
	}
	catch (exception &e)
	{
		cout << "Failed to allocate memory (" << e.what() << ") for " << CARInside::GetInstance()->schemaList.SchemaGetName(schemaId) << " [" << objAllocIndex << ", " << amount << "]" << endl;
		delete[] objProps.propsList;
		delete[] changeDiary.stringList;
		delete[] changedUsers.nameList;
		delete[] owners.nameList;
		delete[] changedTimes.timestampList;
		delete[] helpTexts.stringList;
		delete[] dInstanceLists.dInstanceList;
		delete[] limits.fieldLimitList;
		delete[] permLists.permissionList;
		delete[] defaultValues.valueList;
		delete[] fieldOptions.intList;
		delete[] createModes.intList;
		delete[] options.intList;
		delete[] dataTypes.intList;
		delete[] fieldMaps.mappingList;
		delete[] fieldIds.internalIdList;
		delete[] names.nameList;
		ClearAllStructs();
		throw;
	}
}

void CARFieldListServer::SetNumItems(unsigned int amount)
{
	names.numItems = amount;
	fieldIds.numItems = amount;
	fieldMaps.numItems = amount;
	dataTypes.numItems = amount;
	options.numItems = amount;
	createModes.numItems = amount;
	fieldOptions.numItems = amount;
	defaultValues.numItems = amount;
	permLists.numItems = amount;
	limits.numItems = amount;
	dInstanceLists.numItems = amount;
	helpTexts.numItems = amount;
	changedTimes.numItems = amount;
	owners.numItems = amount;
	changedUsers.numItems = amount;
	changeDiary.numItems = amount;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
	objProps.numItems = amount;
#endif
}

void CARFieldListServer::BuildIndex()
{
	sortedList.reserve(names.numItems);
	for (unsigned int i=0; i<names.numItems; ++i)
	{
		sortedList.push_back(i);
	}
	workflowReferences.resize(names.numItems);
}
