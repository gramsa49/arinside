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
#include "ARVUIList.h"
#include "../ARInside.h"

///////////////////////////////////////////////////////////////////////////////
// CARVUIListXML - implementation
void CARVUIListXML::FinishLoading()
{
	if (sortedList.empty() && vuiList.numItems > 0)
	{
		for (unsigned int i = 0; i < vuiList.numItems; ++i)
			sortedList.push_back(i);
	}

	Sort();
}

void CARVUIListXML::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList.begin(),sortedList.end(),SortByVUINameXML(*this));
}

int CARVUIListXML::Find(unsigned int vuiId)
{
	unsigned int count = GetCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		if (vuiList.vuiList[sortedList[i]].vuiId == vuiId)
			return i;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
// CARVUIListServer - implementation
CARVUIListServer::CARVUIListServer(unsigned int schemaInsideId) 
{
	schemaId = schemaInsideId;
	ARZeroMemory(&ids);
	ARZeroMemory(&names);
	ARZeroMemory(&locales);
	ARZeroMemory(&types);
	ARZeroMemory(&props);
	ARZeroMemory(&helpTexts);
	ARZeroMemory(&changedTimes);
	ARZeroMemory(&owners);
	ARZeroMemory(&changedUsers);
	ARZeroMemory(&changeDiary);
}

CARVUIListServer::~CARVUIListServer()
{
	FreeARInternalIdList(&ids, false);
	FreeARNameList(&names, false);
	FreeARLocaleList(&locales, false);
	FreeARUnsignedIntList(&types, false);
	FreeARPropListList(&props, false);
	FreeARTextStringList(&helpTexts, false);
	FreeARTimestampList(&changedTimes, false);
	FreeARAccessNameList(&owners, false);
	FreeARAccessNameList(&changedUsers, false);
	FreeARTextStringList(&changeDiary, false);
}

bool CARVUIListServer::LoadFromServer()
{
	ARBooleanList   vuiExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();
	ARNameType			schemaName; memcpy(schemaName, arIn->schemaList.SchemaGetName(schemaId), sizeof(ARNameType));

	memset(&vuiExists, 0, sizeof(ARBooleanList));
	memset(&status, 0, sizeof(status));

	// ok, get all informations of the fields we need
	if (ARGetMultipleVUIs(&arIn->arControl,
		schemaName, 
		NULL, // all field ids
		0,    // no changed since restriction
		&vuiExists,
		&ids,
		&names,
		&locales,
		&types,
		&props,
		&helpTexts,
		&changedTimes,
		&owners,
		&changedUsers,
		&changeDiary,
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_763
		NULL, // objPropListList// TODO: support new prop list
#endif
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&vuiExists, false);

		sortedList.reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
			sortedList.push_back(i);
		}
		return true;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return false;
	}
}

int CARVUIListServer::Find(unsigned int vuiId)
{
	unsigned int vuiCount = GetCount();
	for (unsigned int i = 0; i < vuiCount; ++i)
	{
		if (ids.internalIdList[i] == vuiId)
			return i;
	}
	return -1;
}

void CARVUIListServer::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList.begin(),sortedList.end(),SortByName<CARVUIListServer>(*this));
}
