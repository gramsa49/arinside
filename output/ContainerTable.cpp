//Copyright (C) 2009 Stefan Nerlich | stefan.nerlich@hotmail.com
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
#include "ContainerTable.h"

using namespace OUTPUT;

CContainerTable::CContainerTable(CARInside &arIn, bool includeObjTypeColumn)
: CObjectTable("containerList", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn((includeObjTypeColumn?40:60), "Container Name");
	if (includeObjTypeColumn)
	{
		tbl.AddColumn(20, "Type");
	}
	tbl.AddColumn(20, "Modified");
	tbl.AddColumn(20, "By");

	hasObjTypeColumn = includeObjTypeColumn;
}

CContainerTable::~CContainerTable(void)
{
}

void CContainerTable::AddRow(CARContainer &cont, int rootLevel)
{
	CTableRow tblRow("");
	string cellNameValue = cont.GetURL(rootLevel);

	switch(cont.GetType())
	{
	case ARCON_FILTER_GUIDE:
		{
			if(NumRelatedFilters(cont)==0)
			{
				cellNameValue += " (<b>!</b>)";
			}
		}
		break;
	case ARCON_GUIDE:
		{
			if(NumRelatedActiveLinks(cont)==0)
			{
				cellNameValue += " (<b>!</b>)";
			}
		}
		break;
	}

	tblRow.AddCell( CTableCell(cellNameValue));
	if (this->hasObjTypeColumn) { tblRow.AddCell( CTableCell(CAREnum::ContainerType(cont.GetType()))); }
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(cont.GetTimestamp())));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(cont.GetLastChanged(), rootLevel)));
	this->tbl.AddRow(tblRow);
}

// TODO: optimize related workflow scanning
// The following two methods scan for activelinks and filters calling the current container, just to add a sign
// to unused containers. The same scanning is done during the container documentation to show all objects calling
// this guide. The code should be moved to the scanning phase to add all callers to the container. Later we can
// check if there are no callers an put the sign in front of the object link.

int CContainerTable::NumRelatedActiveLinks(CARContainer &obj)
{
	int nResult = 0;
	try
	{
		unsigned int alCount = this->pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex )
		{
			CARActiveLink al(alIndex);

			// skip this object in case it's overlaid (hidden)
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(al))
				continue;

			//Search if-actions
			for(unsigned int nAction = 0; nAction < al.GetIfActions().numItems; nAction++)
			{
				const ARActiveLinkActionStruct &action = al.GetIfActions().actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, obj.GetARName())==0)
					{
						nResult++;
					}
				}				
			}

			//Search else-actions
			for(unsigned int nAction = 0; nAction < al.GetElseActions().numItems; nAction++)
			{
				const ARActiveLinkActionStruct &action = al.GetElseActions().actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, obj.GetARName())==0)
					{
						nResult++;
					}
				}				
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating NumRelatedActiveLinks() in container '" << obj.GetName() << "': " << e.what() << endl;
	}

	return nResult;
}

int CContainerTable::NumRelatedFilters(CARContainer &obj)
{
	int nResult = 0;
	try
	{
		unsigned int filterCount = pInside->filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; ++filterIndex )
		{
			CARFilter filter(filterIndex);

			// skip this object in case it's overlaid (hidden)
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(filter))
				continue;

			//Search if-actions
			for(unsigned int nAction = 0; nAction < filter.GetIfActions().numItems; nAction++)
			{
				ARFilterActionStruct &action = filter.GetIfActions().actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, obj.GetARName())==0)
					{
						nResult++;
					}
				}				
			}

			//Search else-actions
			for(unsigned int nAction = 0; nAction < filter.GetElseActions().numItems; nAction++)
			{
				ARFilterActionStruct action = filter.GetElseActions().actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, obj.GetARName())==0)
					{
						nResult++;
					}
				}				
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating NumRelatedFilters() in container '" << obj.GetName() << "': " << e.what() << endl;
	}

	return nResult;
}
