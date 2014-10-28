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

ARPropList emptyPropList;

IndexSorter::IndexSorter(ARNameList &list)
{
	InitList(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		PushBackTrimmed(list.nameList[i]);
	}
}

IndexSorter::IndexSorter(ARFieldInfoList &list)
{
	InitList(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		PushBackTrimmed(list.fieldList[i].fieldName);
	}
}

IndexSorter::IndexSorter(ARVuiInfoList &list)
{
	InitList(list.numItems);
	for (unsigned int i = 0; i < list.numItems; ++i)
	{
		PushBackTrimmed(list.vuiList[i].vuiName);
	}
}

IndexSorter::IndexSorter(vector<string> &list)
{
	size_t totalCount = list.size();
	InitList(static_cast<unsigned int>(totalCount));
	for (size_t index = 0; index < totalCount; ++index)
	{
		PushBackTrimmed(list[index]);
	}
}

IndexSorter::~IndexSorter()
{
	if (theList != NULL)
	{
		delete[] theList->nameList;
		delete theList;
	}
}

void IndexSorter::InitList(unsigned int size)
{
	theList = new ARNameList;
	theList->nameList = new ARNameType[size];
	theList->numItems = 0;
}

void IndexSorter::PushBackTrimmed(ARNameType &value)
{
	unsigned int skippedSpaces = 0;
	while (value[skippedSpaces] == ' ') skippedSpaces++;

	strncpy(theList->nameList[theList->numItems], &value[skippedSpaces], AR_MAX_NAME_SIZE);
	theList->nameList[theList->numItems][AR_MAX_NAME_SIZE] = 0;
	theList->numItems++;
}

void IndexSorter::PushBackTrimmed(const std::string &value)
{
	unsigned int skippedSpaces = 0;
	while (value[skippedSpaces] == ' ') skippedSpaces++;

	strncpy(theList->nameList[theList->numItems], (value.c_str() + skippedSpaces), AR_MAX_NAME_SIZE);
	theList->nameList[theList->numItems][AR_MAX_NAME_SIZE] = 0;
	theList->numItems++;
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
