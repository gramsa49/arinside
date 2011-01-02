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
#include "../ARInside.h"
#include "../util/RefItem.h"

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
		std::sort(sortedList.begin(),sortedList.end(),SortByFieldNameXML(*this));
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

///////////////////////////////////////////////////////////////////////////////
// CARFieldListServer implementation
CARFieldListServer::CARFieldListServer(unsigned int schemaInsideId)
{
	schemaId = schemaInsideId;
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

bool CARFieldListServer::LoadFromServer()
{
	ARBooleanList   fldExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();
	ARNameType			schemaName; memcpy(schemaName, arIn->schemaList.SchemaGetName(schemaId), sizeof(ARNameType));

	memset(&fldExists, 0, sizeof(ARBooleanList));
	memset(&status, 0, sizeof(status));

	// ok, get all informations of the fields we need
	if (ARGetMultipleFields(&arIn->arControl,
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
		NULL, // objPropListList // TODO: support new prop list
#endif
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&fldExists, false);

		sortedList.reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
			sortedList.push_back(i);
		}

		workflowReferences.resize(names.numItems);

		return true;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return false;
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
		std::sort(sortedList.begin(),sortedList.end(),SortByName<CARFieldListServer>(*this));
}
