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
#include "ARMenuList.h"
#include "../ARInside.h"
#include "../output/WebUtil.h"
#include "BlackList.h"

CARMenuList::CARMenuList(void)
{
	internalListState = CARMenuList::EMPTY;
	reservedSize = 0;
	names.numItems = 0;
}

CARMenuList::~CARMenuList(void)
{
	if (internalListState == INTERNAL_ALLOC)
	{
		try
		{
			delete[] names.nameList;
			delete[] refreshCodes.intList;
			delete[] definitions.list;
			delete[] changedTimes.timestampList;
			delete[] helpTexts.stringList;
			delete[] owners.nameList;
			delete[] changedUsers.nameList;
			delete[] changeDiary.stringList;
			delete[] objProps.propsList;
		}
		catch (...)
		{
		}
	}
	else if (internalListState == ARAPI_ALLOC)
	{		
		try
		{
			FreeARNameList(&names,false);
			FreeARUnsignedIntList(&refreshCodes,false);
			FreeARCharMenuStructList(&definitions,false);
			FreeARTextStringList(&helpTexts,false);
			FreeARTimestampList(&changedTimes,false);
			FreeARAccessNameList(&owners,false);
			FreeARAccessNameList(&changedUsers,false);
			FreeARTextStringList(&changeDiary,false);
			FreeARPropListList(&objProps,false);
		}
		catch (...)
		{

		}
	}
}

bool CARMenuList::LoadFromServer()
{
	ARBooleanList   mnuExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();
	ARNameList*     objectsToLoad = NULL;
	ARNameList      objectNames;
	unsigned int    originalObjectNameCount = 0;
	bool            funcResult = false;

	memset(&mnuExists, 0, sizeof(mnuExists));
	memset(&status, 0, sizeof(status));

	// if the blacklist contains menus, we should first load all menu names
	// from the server and remove those that are contained in the blacklist.
	// after that call ARGetMultiple... to retrieve just the needed objects.
	if (arIn->blackList.GetCountOf(ARREF_CHAR_MENU) > 0)
	{
		memset(&objectNames, 0, sizeof(objectNames));

		if (ARGetListCharMenu(&arIn->arControl, NULL, 0, NULL, NULL, &objectNames, &status) == AR_RETURN_OK)
		{
			originalObjectNameCount = objectNames.numItems;
			arIn->blackList.Exclude(ARREF_CHAR_MENU, &objectNames);
			objectsToLoad = &objectNames;
		}
		else
			cerr << arIn->GetARStatusError(&status);
	}

	// ok, now retrieve all informations of the active links we need
	if (ARGetMultipleCharMenus(&arIn->arControl,
		0,
		objectsToLoad,
		&mnuExists,
		&names,
		&refreshCodes,
		&definitions,
		&helpTexts,
		&changedTimes,
		&owners,
		&changedUsers,
		&changeDiary,
		&objProps,
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&mnuExists, false);
		internalListState = CARMenuList::ARAPI_ALLOC;

		sortedList.reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
			appRefNames.push_back("");
			sortedList.push_back(i);
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

	return funcResult;
}

void CARMenuList::Reserve(unsigned int size)
{
	if (internalListState != CARMenuList::EMPTY) throw AppException("object isnt reusable!", "MenuList");

	sortedList.reserve(size);
	appRefNames.reserve(size);

	names.numItems = 0;
	names.nameList = new ARNameType[size];

	refreshCodes.numItems = 0;
	refreshCodes.intList = new unsigned int[size];

	definitions.numItems = 0;
	definitions.list = new ARCharMenuStruct[size];

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

	reservedSize = size;
	internalListState = CARMenuList::INTERNAL_ALLOC;
}

int CARMenuList::AddMenuFromXML(ARXMLParsedStream &stream, const char* menuName, unsigned int *outDocVersion)
{
	if (internalListState != CARMenuList::INTERNAL_ALLOC) throw AppException("illegal usage!", "MenuList");
	if (names.numItems >= reservedSize) return -1;
	if (outDocVersion != NULL) *outDocVersion = 0;
	
	CARInside* arIn = CARInside::GetInstance();
	ARNameType appBlockName; appBlockName[0] = 0;
	ARStatusList status;

	unsigned int arDocVersion = 0;
	unsigned int index = names.numItems;
	strncpy(names.nameList[index], menuName, 254);	// copy name over
	names.nameList[index][254] = 0;

	if (ARGetMenuFromXML(&arIn->arControl,
		&stream,
		names.nameList[index],
		appBlockName,
		&refreshCodes.intList[index],
		&definitions.list[index],
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
		++refreshCodes.numItems;
		++definitions.numItems;
		++helpTexts.numItems;
		++changedTimes.numItems;
		++owners.numItems;
		++changedUsers.numItems;
		++changeDiary.numItems;
		++objProps.numItems;

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

int CARMenuList::Find(const char* name)
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

void CARMenuList::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList.begin(),sortedList.end(),SortByName<CARMenuList>(*this));
}

