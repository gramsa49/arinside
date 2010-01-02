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

CContainerTable::CContainerTable(CARInside &arIn)
: CObjectTable("containerLIst", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn(40, "Container Name");
	tbl.AddColumn(20, "Type");
	tbl.AddColumn(20, "Modified");
	tbl.AddColumn(20, "By");
}

CContainerTable::~CContainerTable(void)
{
}

void CContainerTable::AddRow(CARContainer &cont, int rootLevel)
{
	CTableRow tblRow("");
	string cellNameValue = cont.GetURL(rootLevel);

	switch(cont.type)
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
	tblRow.AddCell( CTableCell(CAREnum::ContainerType(cont.type)));
	tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(cont.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(cont.lastChanged, rootLevel)));
	this->tbl.AddRow(tblRow);
}

int CContainerTable::NumRelatedActiveLinks(CARContainer &obj)
{
	int nResult = 0;
	try
	{
		unsigned int alCount = this->pInside->alList.GetCount();
		for (unsigned int alIndex = 0; alIndex < alCount; ++alIndex )
		{
			CARActiveLink al(alIndex);

			//Search if-actions
			for(unsigned int nAction = 0; nAction < al.GetIfActions().numItems; nAction++)
			{
				const ARActiveLinkActionStruct &action = al.GetIfActions().actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, obj.name.c_str())==0)
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
					if(strcmp(action.u.callGuide.guideName, obj.name.c_str())==0)
					{
						nResult++;
					}
				}				
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating NumRelatedActiveLinks() in container '" << obj.name << "': " << e.what() << endl;
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

			//Search if-actions
			for(unsigned int nAction = 0; nAction < filter.GetIfActions().numItems; nAction++)
			{
				ARFilterActionStruct &action = filter.GetIfActions().actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(obj.name.compare(action.u.callGuide.guideName)==0)
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
					if(strcmp(action.u.callGuide.guideName, obj.name.c_str())==0)
					{
						nResult++;
					}
				}				
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating NumRelatedFilters() in container '" << obj.name << "': " << e.what() << endl;
	}

	return nResult;
}
