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

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

class CARImageList : Uncopyable
{
public:
	CARImageList();
	~CARImageList();
	
	bool LoadFromServer();
	int AddImageFromXML(ARXMLParsedStream& stream, const char* imageName);

	// list functions
	inline unsigned int GetCount() { return names.numItems; }
	int FindImage(const char *name);
	void Reserve(unsigned int size);
	void Sort();

	// we dont use a separate CImage object yet. This avoids additional memory 
	// usage. the image object would need to store at least the index and
	// optinally a reference to the CImageList (but normally we dont need more
	// than one). The functions are inline to avoid additional strack frames.
	inline const ARNameType& ImageGetName(unsigned int index) { _ASSERT(index < names.numItems); return names.nameList[(*sortedList)[index].index]; }
	inline char* ImageGetType(unsigned int index) { _ASSERT(index < types.numItems); return types.stringList[(*sortedList)[index].index]; }
	inline const ARTimestamp& ImageGetTimestamp(unsigned int index) { return changedTimes.timestampList[(*sortedList)[index].index]; }
	inline char* ImageGetDescription(unsigned int index) { _ASSERT(index < descriptions.numItems); return descriptions.stringList[(*sortedList)[index].index]; }
	inline char* ImageGetHelptext(unsigned int index) { _ASSERT(index < helpTexts.numItems); return helpTexts.stringList[(*sortedList)[index].index]; }
	inline const ARAccessNameType& ImageGetOwner(unsigned int index) { _ASSERT(index < owners.numItems); return owners.nameList[(*sortedList)[index].index]; }
	inline const ARAccessNameType& ImageGetModifiedBy(unsigned int index) { _ASSERT(index < changedUsers.numItems); return changedUsers.nameList[(*sortedList)[index].index]; }
	inline char* ImageGetChangeDiary(unsigned int index) { _ASSERT(index < changeDiary.numItems); return changeDiary.stringList[(*sortedList)[index].index]; }
	inline const ARImageDataStruct& ImageGetData(unsigned int index) { _ASSERT(index < data.numItems); return data.imageList[(*sortedList)[index].index]; }
	string ImageGetURL(unsigned int index, int rootLevel);


private:
	// allocation state of internal structures
	enum ImageListState { EMPTY, ARAPI_ALLOC, INTERNAL_ALLOC };

	// for sorting a dummy object is used as a reference to an image (just the index in this case)
	// its sorted ascending by name
	class image_ref
	{
	public:
		CARImageList *list;
		unsigned int index;
		
		image_ref(CARImageList &list, int idx) : list(&list),index(idx) {}
		bool operator<(const image_ref &r) { return (strcmp(list->names.nameList[index], r.list->names.nameList[r.index]) < 0); }
		image_ref& operator=(const image_ref &r) { list = r.list; index = r.index; return *this; }
	};

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
	vector<image_ref> *sortedList;
};

#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
