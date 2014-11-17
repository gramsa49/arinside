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

#pragma once

#include "InternalNameList.h"

class BasicStringList : public InternalNameList
{
public:
	BasicStringList() { list = NULL; }
	virtual ~BasicStringList();

	virtual void Allocate(unsigned int size);
	virtual void PushBack(ARNameType &value);
	virtual void PushBack(const std::string &value);

	bool operator()(int l, int r);

	ARNameList *list;

private:
	void MakeLower(ARNameType &value);
};
