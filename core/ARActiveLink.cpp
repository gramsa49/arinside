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
#include ".\aractivelink.h"

CARActiveLink::CARActiveLink(string name, int insideId)
{
	this->name = name;
	this->insideId = insideId;

	this->order = 0;
	this->executeMask = 0;
	this->controlField = 0;
	this->focusField = 0;
	this->enable = 0;	

	this->objPropList.props = NULL;
	this->query.operation = NULL;
}

string CARActiveLink::GetURL(int rootLevel)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"active_link/"+this->FileID()+"/"+CWebUtil::DocName("index"), "active_link.gif", rootLevel);
}

CARActiveLink::~CARActiveLink(void)
{
	try
	{
		FreeARWorkflowConnectStruct(&schemaList, false);
		FreeARInternalIdList(&groupList, false);
		FreeARQualifierStruct(&query, false);
		FreeARActiveLinkActionList(&actionList, false);
		FreeARActiveLinkActionList(&elseList, false);

		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
	catch(...)
	{
	}
}

string CARActiveLink::GetExecuteOn()
{		
	stringstream strm;
	strm.str("");

	try
	{		
		if(executeMask==NULL)
		{
			strm << "None";
		}
		else
		{
			unsigned int bitmask[19] = { 1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<7, 1<<8, 1<<9, 1<<10, 1<<11, 1<<12, 1<<13, 1<<14, 1<<15, 1<<16, 1<<17, 1<<18, 1<<19, 1<<20 };
			char executeText[19][30] = { "Button", "Return", "Submit", "Modify", "Display", "Menu Choice", "Loose Focus", "Set Default", "Search", "After Modify", "After Submit", "Gain Focus", "Window Open", "Window Close", "Undisplay", "Copy to New", "Loaded", "Interval", "Event"};

			for (unsigned int k= 0; k < 19; k++)
			{
				if ( (executeMask & bitmask[k]) != 0)
				{				
					strm << executeText[k] << "<br/>";			
				}
			}

			if(strm.str().size() > 5)
			{
				CUtil::CleanUpStream(strm, 5);
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in ActiveLink GetExecuteOn" << endl; 
	}
	return strm.str();
}

string CARActiveLink::GetExecuteOnEx()
{		
	stringstream strm;
	strm.str("");

	try
	{		
		if(executeMask==NULL)
		{
			strm << "None";
		}
		else
		{
			unsigned int bitmask[19] = { 1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<7, 1<<8, 1<<9, 1<<10, 1<<11, 1<<12, 1<<13, 1<<14, 1<<15, 1<<16, 1<<17, 1<<18, 1<<19, 1<<20 };
			char executeText[19][30] = { "Button", "Return", "Submit", "Modify", "Display", "Menu Choice", "Loose Focus", "Set Default", "Search", "After Modify", "After Submit", "Gain Focus", "Window Open", "Window Close", "Undisplay", "Copy to New", "Loaded", "Interval", "Event"};

			for (unsigned int k= 0; k < 19; k++)
			{
				if ( (executeMask & bitmask[k]) != 0)
				{				
					strm << executeText[k] << ", ";			
				}
			}

			if(strm.str().size() > 2)
			{
				CUtil::CleanUpStream(strm, 2);
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in ActiveLink GetExecuteOnEx" << endl; 
	}
	return strm.str();
}