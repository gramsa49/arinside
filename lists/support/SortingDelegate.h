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

// a sorting delegate is needed, because the sort-algorithm copies the object multiple times
template<class S>
class SortingDelegate
{
public:
	SortingDelegate(S &SortRef) : theSortFn(SortRef) {}
	bool operator()(int l, int r) { return theSortFn(l,r); }
private:
	S &theSortFn;
};

