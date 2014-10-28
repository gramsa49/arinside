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
#include "../ARInside.h"
#include "../util/Util.h"
#include "thirdparty/utf8cpp/source/utf8.h"
#include <locale>

ARPropList emptyPropList;

class InternalNameList
{
public:
	virtual ~InternalNameList() {}
	virtual void Allocate(unsigned int size) = 0;
	virtual void PushBack(ARNameType &item) = 0;
	virtual void PushBack(const std::string &item) = 0;
};

class BasicStringList : public InternalNameList
{
public:
	BasicStringList() 
	{
		list = NULL;
	}
	virtual ~BasicStringList()
	{
		if (list != NULL)
		{
			delete[] list->nameList;
			delete list;
		}
	}

	virtual void Allocate(unsigned int size)
	{
		list = new ARNameList;
		list->nameList = new ARNameType[size];
		list->numItems = 0;
	}
	virtual void PushBack(ARNameType &value)
	{
		unsigned int skippedSpaces = 0;
		while (value[skippedSpaces] == ' ') skippedSpaces++;

		strncpy(list->nameList[list->numItems], &value[skippedSpaces], AR_MAX_NAME_SIZE);
		list->nameList[list->numItems][AR_MAX_NAME_SIZE] = 0;
		list->numItems++;
	}
	virtual void PushBack(const std::string &value)
	{
		unsigned int skippedSpaces = 0;
		while (value[skippedSpaces] == ' ') skippedSpaces++;

		strncpy(list->nameList[list->numItems], (value.c_str() + skippedSpaces), AR_MAX_NAME_SIZE);
		list->nameList[list->numItems][AR_MAX_NAME_SIZE] = 0;
		list->numItems++;
	}
	bool operator()(int l, int r) { return (strcoll(list->nameList[l], list->nameList[r]) < 0); }

	ARNameList *list;
};

class LocaleDetector
{
public:
	std::locale sortLocale;

	LocaleDetector()
	{
		locale systemLocale("");
		string name = systemLocale.name();

		transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (!CUtil::StrEndsWith(name, "utf8") && !CUtil::StrEndsWith(name, "utf-8"))
		{
			size_t index = name.find_first_not_of("abcdefghijklmnopqrstuvwxyz_");
			if (index != string::npos)
			{
				name = systemLocale.name().substr(0, index);
			}
			else
				name = systemLocale.name();

			name += ".utf8";

			try
			{
				sortLocale = std::locale(name.c_str());
			}
			catch (runtime_error &rterr)
			{
				cerr << "WARN: " << rterr.what() << " (" << name << ")" << endl;
				cerr << "Sorting might be incorrect! Please select a valid utf8 locale before running ARInside!";
				sortLocale = systemLocale;
			}
		}
		else
		{
			sortLocale = systemLocale;
		}
	}
};

#ifdef WIN32
class UTF8StringList : public InternalNameList
{
private:
	std::locale sysLoc;
	const std::collate<wchar_t> &coll;
public:
	UTF8StringList()
		: sysLoc(""), coll(std::use_facet<std::collate<wchar_t> >(sysLoc))
	{
	}

	virtual void Allocate(unsigned int size)
	{
		list.reserve(size);
	}
	virtual void PushBack(ARNameType &value)
	{
		unsigned int skippedSpaces = 0;
		while (value[skippedSpaces] == ' ') skippedSpaces++;

		vector<wchar_t> wideString;
		utf8::utf8to16(&value[skippedSpaces], &value[strlen(value)], back_inserter(wideString));
		wideString.push_back(0);
		
		wchar_t *uStr = &wideString[0];
		list.push_back(uStr);
	}
	virtual void PushBack(const std::string &value)
	{
		unsigned int skippedSpaces = 0;
		while (value[skippedSpaces] == ' ') skippedSpaces++;

		vector<wchar_t> wideString;
		utf8::utf8to16(value.begin() + skippedSpaces, value.end(), back_inserter(wideString));
		wideString.push_back(0);
		
		wchar_t *uStr = &wideString[0];
		list.push_back(uStr);
	}
	bool operator()(int l, int r) { return (coll.compare(list[l].c_str(), list[l].c_str() + list[l].size(), list[r].c_str(), list[r].c_str() + list[r].size()) < 0); }

	vector<std::wstring> list;
};
#else
class UTF8StringList : public InternalNameList
{
private:
	LocaleDetector localeDetection;
	const std::collate<char> &coll;
public:
	UTF8StringList()
		: coll(std::use_facet<std::collate<char> >(localeDetection.sortLocale))
	{
	}

	virtual void Allocate(unsigned int size)
	{
		list.reserve(size);
	}
	virtual void PushBack(ARNameType &value)
	{
		unsigned int skippedSpaces = 0;
		while (value[skippedSpaces] == ' ') skippedSpaces++;

		list.push_back(&value[skippedSpaces]);
	}
	virtual void PushBack(const std::string &value)
	{
		unsigned int skippedSpaces = 0;
		while (value[skippedSpaces] == ' ') skippedSpaces++;

		list.push_back(value.c_str() + skippedSpaces);
	}
	bool operator()(int l, int r) { return (coll.compare(list[l].c_str(), list[l].c_str() + list[l].size(), list[r].c_str(), list[r].c_str() + list[r].size()) < 0); }

	vector<std::string> list;
};
#endif

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

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
/////// additional functions ///////
void NormalizeNameListForSorting(ARNameList &names, ARPropListList &objProps)
{
	// Since ARS 7.6.4 we have two workflow layers, like an object inherited from another. It
	// uses layer-numbering, where 0 is the base layer mostly used for out-of-the-box stuff and
	// 1 is the custom layer. You can switch the active layer on the server. If layer 0 is
	// active only out of the box workflow is executed and visible to clients (WUT/midtier).
	// If layer 1 is active (this is the default) all workflow is inherited from layer 0 and
	// extended by custom workflow or overlaid ones contained in layer 1. ARInside should
	// always document the current workflow execution model, so we have to take the Overlay-
	// Mode into account.
	// overlayMode 0 = only origin workflow should be visible (no custom, no overlay)
	// overlayMode 1 = origin, custom and overlaid workflow should be visible
	// In later version of ARSystem there might be more layers available.

	// for overlays we need to rename the object names accordingly
	unsigned int count = min(names.numItems, objProps.numItems);
	for (unsigned int idx = 0; idx < count; ++idx)
	{
		ARValueStruct* val = CARProplistHelper::Find(objProps.propsList[idx], AR_SMOPROP_OVERLAY_PROPERTY);

		if (val != NULL && val->dataType == AR_DATA_TYPE_INTEGER)
		{
			switch (val->u.intVal)
			{
			case AR_OVERLAID_OBJECT:
				{
					if (CARInside::GetInstance()->overlayMode == 1)
					{
						// add the AR_RESERVED_OVERLAY_STRING to the end of the name .. this becomes the original object: "__o" ;-)
						strncat(names.nameList[idx], AR_RESERV_OVERLAY_STRING, AR_MAX_NAME_SIZE);
						names.nameList[idx][AR_MAX_NAME_SIZE] = 0;
					}
				}
				break;
			case AR_OVERLAY_OBJECT:
				{
					if (CARInside::GetInstance()->overlayMode == 1)
					{
						// strip the AR_RESERVED_OVERLAY_STRING from end of the name, so it gets the real object name
						size_t nameLen = strlen(names.nameList[idx]);
						if (nameLen > 3)
						{
							nameLen -= 3;
							if (strcmp(&names.nameList[idx][nameLen], AR_RESERV_OVERLAY_STRING) == 0)
								names.nameList[idx][nameLen] = 0;
						}
					}
				}
			case AR_CUSTOM_OBJECT:
				{
					// strip the AR_RESERV_OVERLAY_CUSTOM_STRING from end of the name, so it gets the real object name
					size_t nameLen = strlen(names.nameList[idx]);
					if (nameLen > 3)
					{
						nameLen -= 3;
						if (strcmp(&names.nameList[idx][nameLen], AR_RESERV_OVERLAY_CUSTOM_STRING) == 0)
							names.nameList[idx][nameLen] = 0;
					}
				}
				break;
			}
		}
	}
}

void NormalizeNameListToRealNames(ARNameList &names, ARPropListList &objProps)
{
	// now make sure the real object names are used
	unsigned int count = min(names.numItems, objProps.numItems);
	for (unsigned int idx = 0; idx < count; ++idx)
	{
		ARValueStruct* val = CARProplistHelper::Find(objProps.propsList[idx], AR_SMOPROP_OVERLAY_PROPERTY);

		if (val != NULL && val->dataType == AR_DATA_TYPE_INTEGER)
		{
			switch (val->u.intVal)
			{
			case AR_OVERLAID_OBJECT:  // this occurs if overlayMode = 1
			case AR_OVERLAY_OBJECT:   // this occurs if overlayMode = 0
				{
					// strip the AR_RESERVED_OVERLAY_STRING again, so it gets the real object name
					size_t nameLen = strlen(names.nameList[idx]);
					if (nameLen > 3)
					{
						nameLen -= 3;
						if (strcmp(&names.nameList[idx][nameLen], AR_RESERV_OVERLAY_STRING) == 0)
							names.nameList[idx][nameLen] = 0;
					}
				}
				break;
			}
		}
	}
}
#endif
