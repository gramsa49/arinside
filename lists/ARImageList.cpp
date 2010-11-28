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
#include "ARImageList.h"
#include "../ARInside.h"
#include "../output/WebUtil.h"
#include "../core/ARImage.h"

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

CARImageList::CARImageList(void)
{
	internalListState = CARImageList::EMPTY;
	reservedSize = 0;
	names.numItems = 0;
	sortedList = NULL;
}

CARImageList::~CARImageList()
{
	if (internalListState == CARImageList::INTERNAL_ALLOC)
	{
		try
		{
			delete[] names.nameList;
			delete[] types.stringList;
			delete[] changedTimes.timestampList;
			delete[] descriptions.stringList;
			delete[] helpTexts.stringList;
			delete[] owners.nameList;
			delete[] changedUsers.nameList;
			delete[] changeDiary.stringList;
			delete[] objProps.propsList;
			delete[] data.imageList;
		}
		catch (...)
		{
		}
	}
	else if (internalListState == CARImageList::ARAPI_ALLOC)
	{		
		try
		{
			FreeARNameList(&names,false);
			FreeARTextStringList(&types,false);
			FreeARTimestampList(&changedTimes,false);
			FreeARTextStringList(&descriptions,false);
			FreeARTextStringList(&helpTexts,false);
			FreeARAccessNameList(&owners,false);
			FreeARAccessNameList(&changedUsers,false);
			FreeARTextStringList(&changeDiary,false);
			FreeARPropListList(&objProps,false);
			FreeARImageDataList(&data,false);
		}
		catch (...)
		{

		}
	}
	if (sortedList != NULL)
		delete sortedList;
}

bool CARImageList::LoadFromServer()
{
	ARBooleanList   imgExists;
	ARStatusList    status;
	CARInside*      arIn = CARInside::GetInstance();

	ARNameList*     objectsToLoad = NULL;
	ARNameList      objectNames;
	unsigned int    originalObjectNameCount = 0;
	bool            funcResult = false;

	// if the blacklist contains images, we should first load all image names from 
	// the server and remove those that are contained in the blacklist. after that 
	// call ARGetMultipleImages to retrieve just the needed images.
	if (arIn->blackList.GetCountOf(ARREF_IMAGE) > 0)
	{
		memset(&objectNames, 0, sizeof(objectNames));

		if (ARGetListImage(&arIn->arControl, NULL, 0, NULL, &objectNames, &status) == AR_RETURN_OK)
		{
			originalObjectNameCount = objectNames.numItems;
			arIn->blackList.Exclude(ARREF_IMAGE, &objectNames);
			objectsToLoad = &objectNames;
		}
		else
			cerr << arIn->GetARStatusError(&status);
	}

	if (ARGetMultipleImages(&arIn->arControl,
		0,
		objectsToLoad,
		&imgExists,
		&names,
		&types,
		&changedTimes,
		&descriptions,
		&helpTexts,
		&owners,
		&changedUsers,
		&changeDiary,
		&objProps,
		NULL, // no checksum needed yet
		&data,
		&status) == AR_RETURN_OK)
	{
		FreeARBooleanList(&imgExists, false);
		internalListState = CARImageList::ARAPI_ALLOC;

		sortedList = new vector<int>();
		sortedList->reserve(names.numItems);
		for (unsigned int i=0; i<names.numItems; ++i)
		{
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

void CARImageList::Reserve(unsigned int size)
{
	if (internalListState != CARImageList::EMPTY) throw AppException("object isnt reusable!", "ImageList");

	sortedList = new vector<int>();
	sortedList->reserve(size);

	names.numItems = 0;
	names.nameList = new ARNameType[size];

	types.numItems = 0;
	types.stringList = new char*[size];

	changedTimes.numItems = 0;
	changedTimes.timestampList = new ARTimestamp[size];

	descriptions.numItems = 0;
	descriptions.stringList = new char*[size];

	helpTexts.numItems = 0;
	helpTexts.stringList = new char*[size];

	owners.numItems = 0;
	owners.nameList = new ARAccessNameType[size];

	changedUsers.numItems = 0;
	changedUsers.nameList = new ARAccessNameType[size];

	changeDiary.numItems = 0;
	changeDiary.stringList = new char*[size];

	objProps.numItems = 0;
	objProps.propsList = new ARPropList[size];

	data.numItems = 0;
	data.imageList = new ARImageDataStruct[size];

	reservedSize = size;
	internalListState = CARImageList::INTERNAL_ALLOC;
}

int CARImageList::AddImageFromXML(ARXMLParsedStream &stream, const char* imageName)
{
	if (internalListState != CARImageList::INTERNAL_ALLOC) throw AppException("illegal usage!", "ImageList");
	if (names.numItems >= reservedSize) return -1;
	
	CARInside* arIn = CARInside::GetInstance();
	ARNameType appBlockName; appBlockName[0] = 0;
	ARStatusList status;

	unsigned int dataSize = 0;
	unsigned int index = names.numItems;
	strncpy(names.nameList[index], imageName, 254);	// copy name over
	names.nameList[index][254] = 0;
	char *checkSum = NULL;

	if (ARGetImageFromXML(&arIn->arControl,
		&stream,
		names.nameList[index],
		appBlockName,
		&types.stringList[index],
		&data.imageList[index].numItems,
		&checkSum, // checksum isnt needed
		&changedTimes.timestampList[index],
		&descriptions.stringList[index],
		owners.nameList[index],
		changedUsers.nameList[index],
		&helpTexts.stringList[index],
		&changeDiary.stringList[index],
		&objProps.propsList[index],
		(char**)&data.imageList[index].bytes,
		&status) == AR_RETURN_OK)
	{
		++names.numItems;
		++types.numItems;
		++changedTimes.numItems;
		++descriptions.numItems;
		++helpTexts.numItems;
		++owners.numItems;
		++changedUsers.numItems;
		++changeDiary.numItems;
		++objProps.numItems;
		++data.numItems;

		sortedList->push_back(index);

		return index;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return -1;
	}
}

int CARImageList::FindImage(const char* name)
{
#ifdef ARINSIDE_USE_MAPS_FOR_LIST_ACCESS
	CMapType::const_iterator it = searchList.find(string(name));
	if (it == searchList.end()) return -1;
	return it->second;
#else
	for (unsigned int i = 0; i < GetCount(); ++i)
	{
		int result = strcoll(names.nameList[(*sortedList)[i]], name);
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
#endif
}

void CARImageList::Sort()
{
	if (GetCount() > 0)
		std::sort(sortedList->begin(),sortedList->end(), SortByName<CARImageList>(*this));

#ifdef ARINSIDE_USE_MAPS_FOR_LIST_ACCESS
	if (!searchList.empty()) searchList.clear();
	for (unsigned int i = 0; i < sortedList->size(); ++i)
	{
		searchList[string(names.nameList[(*sortedList)[i]])] = i;
	}
#endif
}

string CARImageList::ImageGetURL(unsigned int index, int rootLevel)
{
	CARImage img(index);
	CPageParams file(PAGE_DETAILS, &img);
	return CWebUtil::Link(ImageGetName(index), file, "image.gif", rootLevel); 
}

void CARImageList::AddReference(const CImageRefItem &referenceItem)
{
	referenceList.push_back(referenceItem);
}
#endif
