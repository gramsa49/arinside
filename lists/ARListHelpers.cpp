//Copyright (C) 2011 John Luthgers | jls17
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
#include "ARListHelpers.h"

GenerateSortableList::GenerateSortableList(ARNameList &list)
{
	InitList(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		unsigned int curWritePos = 0;
		for (unsigned int j = 0; j < AR_MAX_NAME_SIZE; ++j)
		{
			if (list.nameList[i][j] == ' ' && curWritePos == 0) continue;	// skip space at the beginning
			theList->nameList[i][curWritePos++] = tolower(list.nameList[i][j]);
		}
	}
}

GenerateSortableList::GenerateSortableList(ARFieldInfoList &list)
{
	InitList(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		unsigned int curWritePos = 0;
		for (unsigned int j = 0; j < AR_MAX_NAME_SIZE; ++j)
		{
			if (list.fieldList[i].fieldName[j] == ' ' && curWritePos == 0) continue;	// skip space at the beginning
			theList->nameList[i][curWritePos++] = tolower(list.fieldList[i].fieldName[j]);
		}
	}
}

GenerateSortableList::GenerateSortableList(ARVuiInfoList &list)
{
	InitList(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		unsigned int curWritePos = 0;
		for (unsigned int j = 0; j < AR_MAX_NAME_SIZE; ++j)
		{
			if (list.vuiList[i].vuiName[j] == ' ' && curWritePos == 0) continue;	// skip space at the beginning
			theList->nameList[i][curWritePos++] = tolower(list.vuiList[i].vuiName[j]);
		}
	}
}

GenerateSortableList::GenerateSortableList(vector<string> &list)
{
	InitList(static_cast<unsigned int>(list.size()));
	for (unsigned int i = 0; i < theList->numItems; ++i)
	{
		unsigned int curWritePos = 0;
		size_t maxSourceLength = list[i].length();
		memset(theList->nameList[i], '\0', AR_MAX_NAME_SIZE + 1);
		
		for (unsigned int j = 0; j < AR_MAX_NAME_SIZE; ++j)
		{
			if (list[i][j] == ' ' && curWritePos == 0) continue;	// skip space at the beginning

			if (j < maxSourceLength)
				theList->nameList[i][j] = tolower(list[i][j]);
			else
				break; // copy only the length of the source .. the other bytes are already filled with nulls
		}
	}
}

GenerateSortableList::~GenerateSortableList()
{
	if (theList != NULL)
	{
		delete[] theList->nameList;
		delete theList;
	}
}

void GenerateSortableList::InitList(unsigned int size)
{
	theList = new ARNameList;
	theList->nameList = new ARNameType[size];
	theList->numItems = size;
}

ARNameList* GenerateSortableList::GetList()
{
	return theList;
}