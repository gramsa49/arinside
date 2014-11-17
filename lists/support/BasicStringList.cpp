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

BasicStringList::~BasicStringList()
{
	if (list != NULL)
	{
		delete[] list->nameList;
		delete list;
	}
}

void BasicStringList::Allocate(unsigned int size)
{
	list = new ARNameList;
	list->nameList = new ARNameType[size];
	list->numItems = 0;
}

void BasicStringList::PushBack(ARNameType &value)
{
	unsigned int skippedSpaces = 0;
	while (value[skippedSpaces] == ' ') skippedSpaces++;

	strncpy(list->nameList[list->numItems], &value[skippedSpaces], AR_MAX_NAME_SIZE);
	MakeLower(list->nameList[list->numItems]);
	list->nameList[list->numItems][AR_MAX_NAME_SIZE] = 0;
	list->numItems++;
}

void BasicStringList::PushBack(const std::string &value)
{
	unsigned int skippedSpaces = 0;
	while (value[skippedSpaces] == ' ') skippedSpaces++;

	strncpy(list->nameList[list->numItems], (value.c_str() + skippedSpaces), AR_MAX_NAME_SIZE);
	MakeLower(list->nameList[list->numItems]);
	list->nameList[list->numItems][AR_MAX_NAME_SIZE] = 0;
	list->numItems++;
}

bool BasicStringList::operator()(int l, int r) 
{
	return (strcoll(list->nameList[l], list->nameList[r]) < 0);
}

void BasicStringList::MakeLower(ARNameType &name)
{
	for (unsigned int idx = 0; idx < AR_MAX_NAME_SIZE; ++idx)
	{
		char c = name[idx];
		if (c == 0) break;
		name[idx] = tolower(c);
	}	
}
