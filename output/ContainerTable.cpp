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

#include "StdAfx.h"
#include ".\containertable.h"

using namespace OUTPUT;

CContainerTable::CContainerTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("containerLIst", "TblObjectList");
	tbl->AddColumn(40, "Container Name");
	tbl->AddColumn(20, "Type");
	tbl->AddColumn(20, "Modified");
	tbl->AddColumn(20, "By");
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
	this->tbl->AddRow(tblRow);
}

int CContainerTable::NumRelatedActiveLinks(CARContainer &obj)
{
	int nResult = 0;
	try
	{
		list<CARActiveLink>::iterator listIter;
		for ( listIter = this->pInside->alList.begin(); listIter != this->pInside->alList.end(); listIter++ )
		{
			CARActiveLink *al = &(*listIter);

			//Search if-actions
			for(unsigned int nAction = 0; nAction < al->actionList.numItems; nAction++)
			{
				ARActiveLinkActionStruct action = al->actionList.actionList[nAction];
				if(action.action == AR_ACTIVE_LINK_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, obj.name.c_str())==0)
					{
						nResult++;
					}
				}				
			}

			//Search else-actions
			for(unsigned int nAction = 0; nAction < al->elseList.numItems; nAction++)
			{
				ARActiveLinkActionStruct action = al->elseList.actionList[nAction];
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
	catch(...)
	{
		cout << "EXCEPTION enumerating NumRelatedActiveLinks() in container: " << obj.name << endl;
	}

	return nResult;
}

int CContainerTable::NumRelatedFilters(CARContainer &obj)
{
	int nResult = 0;
	try
	{
		list<CARFilter>::iterator listIter;
		for ( listIter = this->pInside->filterList.begin(); listIter != this->pInside->filterList.end(); listIter++ )
		{
			CARFilter *filter = &(*listIter);

			//Search if-actions
			for(unsigned int nAction = 0; nAction < filter->actionList.numItems; nAction++)
			{
				ARFilterActionStruct action = filter->actionList.actionList[nAction];
				if(action.action == AR_FILTER_ACTION_CALLGUIDE)
				{
					if(strcmp(action.u.callGuide.guideName, obj.name.c_str())==0)
					{
						nResult++;
					}
				}				
			}

			//Search else-actions
			for(unsigned int nAction = 0; nAction < filter->elseList.numItems; nAction++)
			{
				ARFilterActionStruct action = filter->elseList.actionList[nAction];
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
	catch(...)
	{
		cout << "EXCEPTION enumerating NumRelatedFilters() in container: " << obj.name << endl;
	}

	return nResult;
}
