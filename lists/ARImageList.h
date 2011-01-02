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
#include "../ARApi.h"
#include "../util/Uncopyable.h"
#include "../util/RefItem.h"
#include "ARListHelpers.h"
#include <assert.h>

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

class CARImageList : Uncopyable
{
public:
	CARImageList();
	~CARImageList();
	
	// loading...
	bool LoadFromServer();
	int AddImageFromXML(ARXMLParsedStream& stream, const char* imageName);

	// referencing...
	typedef vector<CRefItem> ReferenceItem;
	void AddReference(unsigned int index, const CRefItem &referenceItem);
	const ReferenceItem& GetReferences(unsigned int index);

	// list functions
	inline unsigned int GetCount() { return names.numItems; }
	int FindImage(const char *name);
	void Reserve(unsigned int size);
	void Sort();

	// The following functions give access to the data. But in most cases its easier
	// to use the CARImage object.
	inline const ARNameType& ImageGetName(unsigned int index) { assert(index < names.numItems); return names.nameList[sortedList[index]]; }
	inline char* ImageGetType(unsigned int index) { assert(index < types.numItems); return types.stringList[sortedList[index]]; }
	inline const ARTimestamp& ImageGetTimestamp(unsigned int index) { return changedTimes.timestampList[sortedList[index]]; }
	inline char* ImageGetDescription(unsigned int index) { assert(index < descriptions.numItems); return descriptions.stringList[sortedList[index]]; }
	inline char* ImageGetHelptext(unsigned int index) { assert(index < helpTexts.numItems); return helpTexts.stringList[sortedList[index]]; }
	inline const ARAccessNameType& ImageGetOwner(unsigned int index) { assert(index < owners.numItems); return owners.nameList[sortedList[index]]; }
	inline const ARAccessNameType& ImageGetModifiedBy(unsigned int index) { assert(index < changedUsers.numItems); return changedUsers.nameList[sortedList[index]]; }
	inline char* ImageGetChangeDiary(unsigned int index) { assert(index < changeDiary.numItems); return changeDiary.stringList[sortedList[index]]; }
	inline const ARImageDataStruct& ImageGetData(unsigned int index) { assert(index < data.numItems); return data.imageList[sortedList[index]]; }
	string ImageGetURL(unsigned int index, int rootLevel);

	// the sort class needs access the the "names" member variable
	friend class SortByName<CARImageList>;
private:
	// allocation state of internal structures
	enum ImageListState { EMPTY, ARAPI_ALLOC, INTERNAL_ALLOC };

private:
	unsigned int reservedSize;
	ARNameList names;
	ARTextStringList types;
	ARTimestampList changedTimes;
	ARTextStringList descriptions;
	ARTextStringList helpTexts;
	ARAccessNameList owners;
	ARAccessNameList changedUsers;
	ARTextStringList changeDiary;
	ARPropListList objProps;
	ARImageDataList data;
	ImageListState internalListState;
	vector<int> sortedList;
	vector<ReferenceItem> referenceList;
#ifdef ARINSIDE_USE_MAPS_FOR_LIST_ACCESS
	typedef map<string,int> CMapType;
	CMapType searchList;
#endif
};

#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
