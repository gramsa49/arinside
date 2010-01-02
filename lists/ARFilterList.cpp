//Copyright (C) 2009 John Luthgers | jls17
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

#include "stdafx.h"
#include "ARFilterList.h"
#include "../ARInside.h"
#include "../output/WebUtil.h"
#include "BlackList.h"

CARFilterList::CARFilterList(void)
{
	internalListState = CARFilterList::EMPTY;
	reservedSize = 0;
	names.numItems = 0;
	sortedList = NULL;
}

CARFilterList::~CARFilterList(void)
{
	if (internalListState == CARFilterList::INTERNAL_ALLOC)
	{
		try
		{
			delete[] names.nameList;
			delete[] orders.intList;
			delete[] schemas.workflowConnectList;
			delete[] operationSets.intList;
			delete[] enabledObjects.intList;
			delete[] queries.qualifierList;
			delete[] ifActions.actionListList;
			delete[] elseActions.actionListList;
			delete[] helpTexts.stringList;
			delete[] changedTimes.timestampList;
			delete[] owners.nameList;
			delete[] changedUsers.nameList;
			delete[] changeDiary.stringList;
			delete[] objProps.propsList;
			delete[] errorOptions.intList;
			delete[] errorHandlers.nameList;
		}
		catch (...)
		{
		}
	}
	else if (internalListState == CARFilterList::ARAPI_ALLOC)
	{		
		try
		{
			FreeARNameList(&names,false);
			FreeARUnsignedIntList(&orders,false);
			FreeARWorkflowConnectList(&schemas,false);
			FreeARUnsignedIntList(&operationSets,false);
			FreeARUnsignedIntList(&enabledObjects,false);
			FreeARQualifierList(&queries,false);
			FreeARFilterActionListList(&ifActions,false);
			FreeARFilterActionListList(&elseActions,false);
			FreeARTextStringList(&helpTexts,false);
			FreeARTimestampList(&changedTimes,false);
			FreeARAccessNameList(&owners,false);
			FreeARAccessNameList(&changedUsers,false);
			FreeARTextStringList(&changeDiary,false);
			FreeARPropListList(&objProps,false);
			FreeARUnsignedIntList(&errorOptions,false);
			FreeARNameList(&errorHandlers,false);
		}
		catch (...)
		{

		}
	}
	if (sortedList != NULL)
		delete sortedList;
}

bool CARFilterList::LoadFromServer()
{
	ARBooleanList   fltExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();
	ARNameList*     objectsToLoad = NULL;
	ARNameList      objectNames;
	unsigned int    originalObjectNameCount = 0;
	bool            funcResult = false;

	memset(&fltExists, 0, sizeof(fltExists));
	memset(&status, 0, sizeof(status));

	// if the blacklist contains filters, we should first load all filter names from the
	// server and remove those that are contained in the blacklist. after that call
	// ARGetMultipleFilters to retrieve just the needed objects.
	if (arIn->blackList.GetCountOf(ARREF_FILTER) > 0)
	{
		memset(&objectNames, 0, sizeof(objectNames));

		if (ARGetListFilter(&arIn->arControl, NULL, 0, NULL, &objectNames, &status) == AR_RETURN_OK)
		{
			originalObjectNameCount = objectNames.numItems;
			arIn->blackList.Exclude(ARREF_FILTER, &objectNames);
			objectsToLoad = &objectNames;
		}
		else
			cerr << arIn->GetARStatusError(&status);
	}

	// ok, now retrieve all informations of the filters we need
	if (ARGetMultipleFilters(&arIn->arControl,
		0,
		objectsToLoad,
		&fltExists,
		&names,
		&orders,
		&schemas,
		&operationSets,
		&enabledObjects,
		&queries,
		&ifActions,
		&elseActions,
		&helpTexts,
		&changedTimes,
		&owners,
		&changedUsers,
		&changeDiary,
		&objProps,
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
		&errorOptions,
		&errorHandlers,
#endif
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&fltExists, false);
		internalListState = CARFilterList::ARAPI_ALLOC;

		sortedList = new vector<int>();
		sortedList->reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
			appRefNames.push_back("");
			errorCallers.push_back(vector<string>());
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

	return funcResult;
}

void CARFilterList::Reserve(unsigned int size)
{
	if (internalListState != CARFilterList::EMPTY) throw AppException("object isnt reusable!", "FilterList");

	sortedList = new vector<int>();
	sortedList->reserve(size);

	names.numItems = 0;
	names.nameList = new ARNameType[size];

	orders.numItems = 0;
	orders.intList = new unsigned int[size];

	schemas.numItems = 0;
	schemas.workflowConnectList = new ARWorkflowConnectStruct[size];

	operationSets.numItems = 0;
	operationSets.intList = new unsigned int[size];

	enabledObjects.numItems = 0;
	enabledObjects.intList = new unsigned int[size];

	queries.numItems = 0;
	queries.qualifierList = new ARQualifierStruct[size];

	ifActions.numItems = 0;
	ifActions.actionListList = new ARFilterActionList[size];

	elseActions.numItems = 0;
	elseActions.actionListList = new ARFilterActionList[size];

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
	
	errorOptions.numItems = 0;
	errorOptions.intList = new unsigned int[size];

	errorHandlers.numItems = 0;
	errorHandlers.nameList = new ARNameType[size];

	appRefNames.reserve(size);
	errorCallers.reserve(size);

	reservedSize = size;
	internalListState = CARFilterList::INTERNAL_ALLOC;
}

int CARFilterList::AddFilterFromXML(ARXMLParsedStream &stream, const char* filterName, unsigned int *outDocVersion)
{
	if (internalListState != CARFilterList::INTERNAL_ALLOC) throw AppException("illegal usage!", "FilterList");
	if (names.numItems >= reservedSize) return -1;
	if (outDocVersion != NULL) *outDocVersion = 0;
	
	CARInside* arIn = CARInside::GetInstance();
	ARNameType appBlockName; appBlockName[0] = 0;
	ARStatusList status;

	unsigned int arDocVersion = 0;
	unsigned int index = names.numItems;
	strncpy(names.nameList[index], filterName, 254);	// copy name over
	names.nameList[index][254] = 0;

	if (ARGetFilterFromXML(&arIn->arControl,
		&stream,
		names.nameList[index],
		appBlockName,
		&orders.intList[index],
		&schemas.workflowConnectList[index],
		&operationSets.intList[index],
		&enabledObjects.intList[index],
		&queries.qualifierList[index],
		&ifActions.actionListList[index],
		&elseActions.actionListList[index],
		owners.nameList[index],
		changedUsers.nameList[index],
		&changedTimes.timestampList[index],
		&helpTexts.stringList[index],
		&changeDiary.stringList[index],
		&objProps.propsList[index],
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
		&errorOptions.intList[index],
		errorHandlers.nameList[index],
#endif
		&arDocVersion,
		&status) == AR_RETURN_OK)
	{
		++names.numItems;
		++orders.numItems;
		++schemas.numItems;
		++operationSets.numItems;
		++enabledObjects.numItems;
		++queries.numItems;
		++ifActions.numItems;
		++elseActions.numItems;
		++helpTexts.numItems;
		++changedTimes.numItems;
		++owners.numItems;
		++changedUsers.numItems;
		++changeDiary.numItems;
		++objProps.numItems;
		++errorOptions.numItems;
		++errorHandlers.numItems;

		sortedList->push_back(index);
		appRefNames.push_back("");
		errorCallers.push_back(vector<string>());

		if (outDocVersion != NULL) *outDocVersion = arDocVersion;

		return index;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return -1;
	}
}

int CARFilterList::Find(const char* name)
{
	for (unsigned int i = 0; i < GetCount(); ++i)
	{
		int result = strcmp(names.nameList[(*sortedList)[i]], name);
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

void CARFilterList::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList->begin(),sortedList->end(),SortByName<CARFilterList>(*this));
}

string CARFilterList::FilterGetURL(unsigned int index, int rootLevel)
{
	stringstream strmTmp;
	strmTmp << CWebUtil::RootPath(rootLevel) << "filter/" << index << "/" << CWebUtil::DocName("index");
	return CWebUtil::Link(FilterGetName(index), strmTmp.str(), "filter.gif", rootLevel); 
}
