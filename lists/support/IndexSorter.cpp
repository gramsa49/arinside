//Copyright (C) 2014 John Luthgers | jls17
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

#include "BasicStringList.h"
#include "IndexSorter.h"
#include "SortingDelegate.h"
#include "UTF8StringList.h"

IndexSorter::IndexSorter(bool isUTF8)
{
	theList = NULL;
	isUtf8List = isUTF8;

	if (isUtf8List)
	{
		theList = new UTF8StringList();
	}
	else
		theList = new BasicStringList();
}

IndexSorter& IndexSorter::SortBy(ARNameList &list)
{
	theList->Allocate(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		theList->PushBack(list.nameList[i]);
	}
	return *this;
}

IndexSorter& IndexSorter::SortBy(ARFieldInfoList &list)
{
	theList->Allocate(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		theList->PushBack(list.fieldList[i].fieldName);
	}
	return *this;
}

IndexSorter& IndexSorter::SortBy(ARVuiInfoList &list)
{
	theList->Allocate(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		theList->PushBack(list.vuiList[i].vuiName);
	}
	return *this;
}

IndexSorter& IndexSorter::SortBy(vector<string> &list)
{
	size_t totalCount = list.size();
	theList->Allocate(static_cast<unsigned int>(totalCount));
	for (size_t index = 0; index < totalCount; ++index)
	{
		theList->PushBack(list[index]);
	}
	return *this;
}

IndexSorter::~IndexSorter()
{
	if (theList != NULL)
	{
		delete theList;
	}
}

void IndexSorter::Sort(std::vector<int> &indexList)
{
	if (isUtf8List)
	{
		UTF8StringList *sorting = static_cast<UTF8StringList*>(theList);
		std::sort(indexList.begin(), indexList.end(), SortingDelegate<UTF8StringList>(*sorting));
	}
	else
	{
		BasicStringList *sorting = static_cast<BasicStringList*>(theList);
		std::sort(indexList.begin(),indexList.end(),SortingDelegate<BasicStringList>(*sorting));
	}
}

