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
#include "AREscalationList.h"
#include "../ARInside.h"
#include "../output/WebUtil.h"
#include "BlackList.h"

CAREscalationList::CAREscalationList(void)
{
	internalListState = CAREscalationList::EMPTY;
	reservedSize = 0;
	names.numItems = 0;
}

CAREscalationList::~CAREscalationList(void)
{
	if (internalListState == CAREscalationList::INTERNAL_ALLOC)
	{
		try
		{
			delete[] names.nameList;
			delete[] times.escalationTmList;
			delete[] schemas.workflowConnectList;
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
		}
		catch (...)
		{
		}
	}
	else if (internalListState == CAREscalationList::ARAPI_ALLOC)
	{		
		try
		{
			FreeARNameList(&names,false);
			//FreeAREscalationTmList(&times,false); // hell, there is no free function for this struct?
			FreeARWorkflowConnectList(&schemas,false);
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
		}
		catch (...)
		{

		}
	}
}

bool CAREscalationList::LoadFromServer()
{
	ARBooleanList   escExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();
	ARNameList*     objectsToLoad = NULL;
	ARNameList      objectNames;
	unsigned int    originalObjectNameCount = 0;
	bool            funcResult = false;

	memset(&escExists, 0, sizeof(escExists));
	memset(&status, 0, sizeof(status));

	// if the blacklist contains escalations, we should first load all names from the
	// server and remove those that are contained in the blacklist. after that call
	// ARGetMultipleEscalations to retrieve just the needed objects.
	if (arIn->blackList.GetCountOf(ARREF_ESCALATION) > 0)
	{
		memset(&objectNames, 0, sizeof(objectNames));

		if (ARGetListEscalation(&arIn->arControl, NULL, 0, NULL, &objectNames, &status) == AR_RETURN_OK)
		{
			originalObjectNameCount = objectNames.numItems;
			arIn->blackList.Exclude(ARREF_ESCALATION, &objectNames);
			objectsToLoad = &objectNames;
		}
		else
			cerr << arIn->GetARStatusError(&status);
	}

	// ok, now retrieve all informations of the escalations we need
	if (ARGetMultipleEscalations(&arIn->arControl,
		0,
		objectsToLoad,
		&escExists,
		&names,
		&times,
		&schemas,
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
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&escExists, false);
		internalListState = CAREscalationList::ARAPI_ALLOC;

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

void CAREscalationList::Reserve(unsigned int size)
{
	if (internalListState != CAREscalationList::EMPTY) throw AppException("object isnt reusable!", "EscalationList");

	sortedList.reserve(size);

	names.numItems = 0;
	names.nameList = new ARNameType[size];

	times.numItems = 0;
	times.escalationTmList = new AREscalationTmStruct[size];

	schemas.numItems = 0;
	schemas.workflowConnectList = new ARWorkflowConnectStruct[size];

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
	
	appRefNames.reserve(size);

	reservedSize = size;
	internalListState = CAREscalationList::INTERNAL_ALLOC;
}

int CAREscalationList::AddEscalationFromXML(ARXMLParsedStream &stream, const char* escalationName, unsigned int *outDocVersion)
{
	if (internalListState != CAREscalationList::INTERNAL_ALLOC) throw AppException("illegal usage!", "EscalationList");
	if (names.numItems >= reservedSize) return -1;
	if (outDocVersion != NULL) *outDocVersion = 0;
	
	CARInside* arIn = CARInside::GetInstance();
	ARNameType appBlockName; appBlockName[0] = 0;
	ARStatusList status;

	unsigned int arDocVersion = 0;
	unsigned int index = names.numItems;
	strncpy(names.nameList[index], escalationName, AR_MAX_NAME_SIZE);	// copy name over
	names.nameList[index][AR_MAX_NAME_SIZE] = 0;

	if (ARGetEscalationFromXML(&arIn->arControl,
		&stream,
		names.nameList[index],
		appBlockName,
		&times.escalationTmList[index],
		&schemas.workflowConnectList[index],
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
		&arDocVersion,
		&status) == AR_RETURN_OK)
	{
		++names.numItems;
		++times.numItems;
		++schemas.numItems;
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

int CAREscalationList::Find(const char* name)
{
	CMapType::const_iterator it = searchList.find(string(name));
	if (it == searchList.end()) return -1;
	return it->second;
}

void CAREscalationList::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList.begin(),sortedList.end(),SortByName<CAREscalationList>(*this));

	// setup lookup map
	if (!searchList.empty()) searchList.clear();
	for (unsigned int i = 0; i < sortedList.size(); ++i)
	{
		searchList[string(names.nameList[sortedList[i]])] = i;
	}
}
