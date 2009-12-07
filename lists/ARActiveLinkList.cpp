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
#include "ARActiveLinkList.h"
#include "../ARInside.h"
#include "../output/WebUtil.h"

CARActiveLinkList::CARActiveLinkList(void)
{
	internalListState = CARActiveLinkList::EMPTY;
	reservedSize = 0;
	names.numItems = 0;
	sortedList = NULL;
}

CARActiveLinkList::~CARActiveLinkList(void)
{
	if (internalListState == CARActiveLinkList::INTERNAL_ALLOC)
	{
		try
		{
			delete[] names.nameList;
			delete[] orders.intList;
			delete[] schemas.workflowConnectList;
			delete[] groups.internalIdListList;
			delete[] execMasks.intList;
			delete[] controlFields.internalIdList;
			delete[] focusFields.internalIdList;
			delete[] enabledObjects.intList;
			delete[] queries.qualifierList;
			delete[] ifActions.actionListList;
			delete[] elseActions.actionListList;
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
	else if (internalListState == CARActiveLinkList::ARAPI_ALLOC)
	{		
		try
		{
			FreeARNameList(&names,false);
			FreeARUnsignedIntList(&orders,false);
			FreeARWorkflowConnectList(&schemas,false);
			FreeARInternalIdListList(&groups,false);
			FreeARUnsignedIntList(&execMasks,false);
			FreeARInternalIdList(&controlFields,false);
			FreeARInternalIdList(&focusFields,false);
			FreeARUnsignedIntList(&enabledObjects,false);
			FreeARQualifierList(&queries,false);
			FreeARActiveLinkActionListList(&ifActions,false);
			FreeARActiveLinkActionListList(&elseActions,false);
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
	if (sortedList != NULL)
		delete sortedList;
}

bool CARActiveLinkList::LoadFromServer()
{
	ARBooleanList alExists;
	ARStatusList	status;
	CARInside*		arIn = CARInside::GetInstance();

	// if the blacklist contains active links, we should first load all active link names
	// from the server and remove those that are contained in the blacklist. after that 
	// call ARGetMultipleActiveLinks to retrieve just the needed active links.

	if (ARGetMultipleActiveLinks(&arIn->arControl,
		0, NULL, 
		&alExists,
		&names,
		&orders,
		&schemas,
		&groups,
		&execMasks,
		&controlFields,
		&focusFields,
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
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750 // Version 7.5 and higher
		NULL,NULL,  // as of version 7.5 this two parameters should be NULL; reserverd for future use
#endif
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&alExists, false);
		internalListState = CARActiveLinkList::ARAPI_ALLOC;

		sortedList = new vector<al_ref>();
		sortedList->reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
			appRefNames.push_back("");
			sortedList->push_back(al_ref(*this,i));
		}
		return true;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return false;
	}
}

void CARActiveLinkList::Reserve(unsigned int size)
{
	if (internalListState != CARActiveLinkList::EMPTY) throw AppException("object isnt reusable!", "ActiveLinkList");

	sortedList = new vector<al_ref>();
	sortedList->reserve(size);

	names.numItems = 0;
	names.nameList = new ARNameType[size];

	orders.numItems = 0;
	orders.intList = new unsigned int[size];

	schemas.numItems = 0;
	schemas.workflowConnectList = new ARWorkflowConnectStruct[size];

	groups.numItems = 0;
	groups.internalIdListList = new ARInternalIdList[size];

	execMasks.numItems = 0;
	execMasks.intList = new unsigned int[size];

	controlFields.numItems = 0;
	controlFields.internalIdList = new ARInternalId[size];

	focusFields.numItems = 0;
	focusFields.internalIdList= new ARInternalId[size];

	enabledObjects.numItems = 0;
	enabledObjects.intList = new unsigned int[size];

	queries.numItems = 0;
	queries.qualifierList = new ARQualifierStruct[size];

	ifActions.numItems = 0;
	ifActions.actionListList = new ARActiveLinkActionList[size];

	elseActions.numItems = 0;
	elseActions.actionListList = new ARActiveLinkActionList[size];

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
	internalListState = CARActiveLinkList::INTERNAL_ALLOC;
}

int CARActiveLinkList::AddActiveLinkFromXML(ARXMLParsedStream &stream, const char* actlinkName, unsigned int *outDocVersion)
{
	if (internalListState != CARActiveLinkList::INTERNAL_ALLOC) throw AppException("illegal usage!", "ActiveLinkList");
	if (names.numItems >= reservedSize) return -1;
	if (outDocVersion != NULL) *outDocVersion = 0;
	
	CARInside* arIn = CARInside::GetInstance();
	ARNameType appBlockName; appBlockName[0] = 0;
	ARStatusList status;

	unsigned int arDocVersion = 0;
	unsigned int index = names.numItems;
	strncpy(names.nameList[index], actlinkName, 254);	// copy name over
	names.nameList[index][254] = 0;

	if (ARGetActiveLinkFromXML(&arIn->arControl,
		&stream,
		names.nameList[index],
		appBlockName,
		&orders.intList[index],
		&schemas.workflowConnectList[index],
		&groups.internalIdListList[index],
		&execMasks.intList[index],
		&controlFields.internalIdList[index],
		&focusFields.internalIdList[index],
		&enabledObjects.intList[index],
		&queries.qualifierList[index],
		&ifActions.actionListList[index],
		&elseActions.actionListList[index],
		NULL, // what is support file info for?
		owners.nameList[index],
		changedUsers.nameList[index],
		&changedTimes.timestampList[index],
		&helpTexts.stringList[index],
		&changeDiary.stringList[index],
		&objProps.propsList[index],
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750 // Version 7.5 and higher
		NULL,NULL,  // as of version 7.5 this two parameters should be NULL; reserverd for future use
#endif
		&arDocVersion,
		&status) == AR_RETURN_OK)
	{
		++names.numItems;
		++orders.numItems;
		++schemas.numItems;
		++groups.numItems;
		++execMasks.numItems;
		++controlFields.numItems;
		++focusFields.numItems;
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

		sortedList->push_back(al_ref(*this, index));
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

int CARActiveLinkList::Find(const char* name)
{
	for (unsigned int i = 0; i < GetCount(); ++i)
	{
		if (strcmp(names.nameList[i], name) == 0)
		{
			size_t vectLen = sortedList->size();
			for (unsigned int k = 0; k < vectLen; ++k)
			{
				if ((*sortedList)[k].index == i) return k;
			}
		}
	}
	return -1;
}


void CARActiveLinkList::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList->begin(),sortedList->end());
}

string CARActiveLinkList::ActiveLinkGetURL(unsigned int index, int rootLevel)
{
	stringstream strmTmp;
	strmTmp << CWebUtil::RootPath(rootLevel) << "active_link/" << index << "/" << CWebUtil::DocName("index");
	return CWebUtil::Link(ActiveLinkGetName(index), strmTmp.str(), "active_link.gif", rootLevel); 
}

//void CARActiveLinkList::AddReference(const CImageRefItem &referenceItem)
//{
//	referenceList.push_back(referenceItem);
//}
