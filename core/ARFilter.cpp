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
#include "ARFilter.h"

CARFilter::CARFilter(string name, int insideId)
: CARServerObjectWithData(insideId)
{
	this->order = 0;
	this->opSet = 0;
	this->enable = 0;
	this->name = name;

	this->objPropList.props = NULL;
}

CARFilter::~CARFilter(void)
{
	try
	{
		FreeARQualifierStruct(&query, false);
		FreeARFilterActionList(&actionList, false);
		FreeARFilterActionList(&elseList, false);
		FreeARWorkflowConnectStruct(&schemaList, false);	

		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
	catch(...)
	{
	}
}

string CARFilter::GetURL(int rootLevel)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"filter/"+this->FileID()+"/"+CWebUtil::DocName("index"), "filter.gif", rootLevel);
}

string CARFilter::GetExecuteOn()
{		
	stringstream strm;
	strm.str("");

	try
	{
		if(opSet == AR_OPERATION_NONE)
		{
			return "None";
		}
		else
		{
			const unsigned int opCount = 6;
			unsigned int bitmask[opCount] = { 
				AR_OPERATION_GET, 
				AR_OPERATION_SET, 
				AR_OPERATION_CREATE, 
				AR_OPERATION_DELETE, 
				AR_OPERATION_MERGE, 
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
				AR_OPERATION_SERVICE 
#endif
			};
			char executeText[opCount][30] = { 
				"Get Entry", 
				"Modify", 
				"Submit", 
				"Delete", 
				"Merge", 
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
				"Service"
#endif
			};

			for (unsigned int k= 0; k < opCount; k++)
			{
				if ( (opSet & bitmask[k]) != 0)
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
	catch(exception& e)
	{
		cout << "EXCEPTION in Filter GetExecuteOn: " << e.what() << endl; 
	}
	return strm.str();
}

string CARFilter::GetExecuteOnEx()
{		
	stringstream strm;
	strm.str("");

	try
	{
		if(opSet==NULL)
		{
			return "None";
		}
		else
		{
			const unsigned int opCount = 6;
			unsigned int bitmask[opCount] = 
			{ 
				AR_OPERATION_GET, 
				AR_OPERATION_SET, 
				AR_OPERATION_CREATE, 
				AR_OPERATION_DELETE, 
				AR_OPERATION_MERGE, 
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
				AR_OPERATION_SERVICE 
#endif
			};
			char executeText[opCount][30] = 
			{ 
				"Get Entry", 
				"Modify", 
				"Submit", 
				"Delete", 
				"Merge", 
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
				"Service"
#endif
			};

			for (unsigned int k= 0; k < opCount; k++)
			{
				if ( (opSet & bitmask[k]) != 0)
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
	catch(exception& e)
	{
		cout << "EXCEPTION in Filter GetExecuteOnEx: " << e.what() << endl; 
	}
	return strm.str();
}
