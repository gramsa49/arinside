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

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

CARImageList::CARImageList(void)
{
	reservedSize = -1;
}

CARImageList::~CARImageList()
{
	if (reservedSize>-1)
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
	else if (reservedSize == -2)
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
}

bool CARImageList::LoadFromServer()
{
	ARBooleanList imgExists;
	ARStatusList	status;
	CARInside*		arIn = CARInside::GetInstance();

	if (ARGetMultipleImages(&arIn->arControl,
		0, NULL, 
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
		reservedSize = -2;
		return true;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return false;
	}
}

void CARImageList::Reserve(unsigned int size)
{
	if (reservedSize != -1) throw exception("object isnt reusable!");

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
}

int CARImageList::AddImageFromXML(ARXMLParsedStream &stream, const char* imageName)
{
	if (reservedSize == -1) throw exception("call Reserve() first!");
	if (reservedSize < -1) throw exception("illegal usage!");
	if (int(names.numItems) >= reservedSize) return -1;
	
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

		return index;
	}
	else
	{
		cerr << arIn->GetARStatusError(&status);
		return -1;
	}
}
#endif
