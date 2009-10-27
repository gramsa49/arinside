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

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

class CARImageList
{
public:
	CARImageList();
	~CARImageList();
	
	bool LoadFromServer();
	int AddImageFromXML(ARXMLParsedStream& stream, const char* imageName);

	// list functions
	inline int GetCount() { return names.numItems; }
	int FindImage(const char *name);
	void Reserve(unsigned int size);

	// we dont use a separate CImage object yet. This avoids additional memory 
	// usage. the image object would need to store at least the index and
	// optinally a reference to the CImageList (but normally we dont need more
	// than one). The functions are inline to avoid additional strack frames.
	inline const ARNameType& ImageGetName(unsigned int index) { _ASSERT(index < names.numItems); return names.nameList[index]; }
	inline const char* ImageGetType(unsigned int index) { _ASSERT(index < types.numItems); return types.stringList[index]; }
	inline const ARTimestamp& ImageGetTimestamp(unsigned int index) { return changedTimes.timestampList[index]; }
	inline const char* ImageGetDescription(unsigned int index) { _ASSERT(index < descriptions.numItems); return descriptions.stringList[index]; }
	inline const char* ImageGetHelptext(unsigned int index) { _ASSERT(index < helpTexts.numItems); return helpTexts.stringList[index]; }
	inline const ARAccessNameType& ImageGetOwner(unsigned int index) { _ASSERT(index < owners.numItems); return owners.nameList[index]; }
	inline const ARAccessNameType& ImageGetModifiedBy(unsigned int index) { _ASSERT(index < changedUsers.numItems); return changedUsers.nameList[index]; }
	inline const char* ImageGetChangeDiary(unsigned int index) { _ASSERT(index < changeDiary.numItems); return changeDiary.stringList[index]; }
	inline const ARImageDataStruct& ImageGetData(unsigned int index) { _ASSERT(index < data.numItems); return data.imageList[index]; }

private:
	int reservedSize;    // -1 = memory not initialized; -2 = structures allocated by ARAPI; greater than -1 = allocated using new []
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
};
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
