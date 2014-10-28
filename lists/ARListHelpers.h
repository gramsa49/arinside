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
#pragma once

#include "../util/Uncopyable.h"

class IndexSorter : Uncopyable
{
public:
	IndexSorter(bool isUTF8);
	~IndexSorter();

	IndexSorter& SortBy(ARNameList &list);
	IndexSorter& SortBy(vector<string> &list);
	IndexSorter& SortBy(ARFieldInfoList &list);
	IndexSorter& SortBy(ARVuiInfoList &list);

	void Sort(vector<int> &indexList);
	bool operator()(int l, int r) { return (strcoll(theList->nameList[l], theList->nameList[r]) < 0); }

private:
	void AllocateList(unsigned int size);
	void PushBackTrimmed(ARNameType &value);
	void PushBackTrimmed(const std::string &value);

private:
	ARNameList* theList;
	bool isUtf8List;
};

// a sorting delegate is needed, because the sort-algorithm copies the object multiple times
class SortingDelegate
{
public:
	SortingDelegate(IndexSorter &SortRef) : theSortFn(SortRef) {}
	bool operator()(int l, int r) { return theSortFn(l,r); }
private:
	IndexSorter &theSortFn;
};


template<class C>
struct DeletePointer : unary_function<C*, void>
{
	void operator()(C* p) { delete p; }
};

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
void NormalizeNameListForSorting(ARNameList &names, ARPropListList &objProps);
void NormalizeNameListToRealNames(ARNameList &names, ARPropListList &objProps);
#endif

// the following prop list should be used in CARServerObject derived classes if
// they don't have their own propList available. It's initialized in CARInside.
extern ARPropList emptyPropList;
