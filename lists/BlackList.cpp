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

#include "stdafx.h"
#include "BlackList.h"
#include "../ARInside.h"

CBlackList::CBlackList(void)
{
}

CBlackList::~CBlackList(void)
{
}

bool CBlackList::LoadFromServer(const string &packingListName)
{
	if (packingListName.empty()) return true;

	CARInside *pInside = CARInside::GetInstance();

	ARReferenceTypeList   refTypes;
	ARReferenceList       refItems;
	ARStatusList          status;

	int refTypeStorage = ARREF_ALL;
	refTypes.refType = &refTypeStorage;
	refTypes.numItems = 1;
	
	memset(&status, 0, sizeof(ARStatusList));
	memset(&refItems, 0, sizeof(ARStatusList));

	cout << "Loading blacklist from packinglist '" << packingListName << "'" << endl;

	if( ARGetContainer(&pInside->arControl,
		(char*)packingListName.c_str(),
		&refTypes,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&refItems,
		NULL,
		NULL,
		NULL,					
		NULL,
		NULL,
		NULL,
		&status) == AR_RETURN_OK)
	{
		containers.push_back(packingListName); // we add the black list too, so its not documented at all

		// now store the blacklisted items in the corresponding lists
		for(unsigned int i=0; i< refItems.numItems; ++i)
		{
			vector<string> *list = NULL;

			switch (refItems.referenceList[i].type)
			{
			case ARREF_SCHEMA: list = &schemas; break;
			case ARREF_ACTLINK:	list = &actlinks; break;
			case ARREF_FILTER: list = &filters; break;
			case ARREF_ESCALATION: list = &escalations; break;
			case ARREF_CONTAINER: list = &containers; break;
			case ARREF_CHAR_MENU: list = &menus; break;
			case ARREF_IMAGE: list = &images; break;
			default: continue;
			}
			
			list->push_back(refItems.referenceList[i].reference.u.name);
		}

		if (refItems.numItems > 0)
		{
			sort(schemas.begin(), schemas.end());
			sort(actlinks.begin(), actlinks.end());
			sort(filters.begin(), filters.end());
			sort(escalations.begin(), escalations.end());
			sort(containers.begin(), containers.end());
			sort(menus.begin(), menus.end());
			sort(images.begin(), images.end());
		}

		FreeARReferenceTypeList(&refTypes, false);
		FreeARReferenceList(&refItems, false);

		return true;
	}
	else
		cerr << "Failed loading the blacklist: " << CARInside::GetInstance()->GetARStatusError(&status);

	return false;
}

size_t CBlackList::GetCountOf(unsigned int nType)
{
	switch (nType)
	{
	case ARREF_SCHEMA: return schemas.size();
	case ARREF_ACTLINK: return actlinks.size();
	case ARREF_FILTER: return filters.size();
	case ARREF_ESCALATION: return escalations.size();
	case ARREF_CONTAINER: return containers.size();
	case ARREF_CHAR_MENU: return menus.size();
	case ARREF_IMAGE: return images.size();
	case ARREF_ALL:
		{
			size_t totalCount = schemas.size();
			totalCount += actlinks.size();
			totalCount += filters.size();
			totalCount += escalations.size();
			totalCount += containers.size();
			totalCount += menus.size();
			totalCount += images.size();
			return totalCount;
		}		
	default: return 0;
	}
}

// this function is just for compatibility for CARInside::InBlacklist
// we'll remove this later again.
bool CBlackList::Contains(unsigned int nType, const char *objName)
{
	vector<string> *list = NULL;

	switch (nType)
	{
	case ARREF_SCHEMA: list = &schemas; break;
	case ARREF_ACTLINK:	list = &actlinks; break;
	case ARREF_FILTER: list = &filters; break;
	case ARREF_ESCALATION: list = &escalations; break;
	case ARREF_CONTAINER: list = &containers; break;
	case ARREF_CHAR_MENU: list = &menus; break;
	case ARREF_IMAGE: list = &images; break;
	default: return false;
	}
	
	if (list != NULL)
	{
		vector<string>::iterator blEndIt = list->end();
		vector<string>::iterator blFindIt = lower_bound(list->begin(), blEndIt, objName);
		
		if (blFindIt != blEndIt && (*blFindIt).compare(objName) == 0)
			return true;
	}
	return false;
}
