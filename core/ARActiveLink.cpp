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
#include "ARActiveLink.h"

CARActiveLink::CARActiveLink(string name, int insideId)
: CARServerObjectWithData(insideId)
{
	this->name = name;

	ARZeroMemory(&schemaList);
	ARZeroMemory(&groupList);
	ARZeroMemory(&query);
	ARZeroMemory(&actionList);
	ARZeroMemory(&elseList);
	ARZeroMemory(&objPropList);

	this->order = 0;
	this->executeMask = 0;
	this->controlField = 0;
	this->focusField = 0;
	this->enable = 0;	
}

string CARActiveLink::GetURL(int rootLevel, bool showImage)
{
	return CWebUtil::Link(this->name, CWebUtil::RootPath(rootLevel)+"active_link/"+this->FileID()+"/"+CWebUtil::DocName("index"), (showImage ? "active_link.gif" : ""), rootLevel);
}

CARActiveLink::~CARActiveLink(void)
{
	// TODO: move ARFree calls to separate method to allow copying of the object without 
	// copying the whole structure
	//try
	//{
	//	FreeARWorkflowConnectStruct(&schemaList, false);
	//	FreeARInternalIdList(&groupList, false);
	//	FreeARQualifierStruct(&query, false);
	//	FreeARActiveLinkActionList(&actionList, false);
	//	FreeARActiveLinkActionList(&elseList, false);

	//	if(objPropList.props != NULL)
	//		FreeARPropList(&objPropList, false);
	//}
	//catch(...)
	//{
	//}
}

/// Builds a ExecuteOn-string
string CARActiveLink::GetExecuteOn(bool singleLine, CARProplistHelper* props)
{		
	stringstream strm;
	strm.str("");
	bool strmHasContent = false; // use a local var to avoid use of 'strm.str().empty', because this make a copy of the string buffer evrytime

	try
	{		
		typedef	struct ARInsideExecOnStruct {
			unsigned int exec;
			char        *text;
		} ARInsideExecOnStruct;

		const ARInsideExecOnStruct executeText[] = {
			AR_EXECUTE_ON_BUTTON, "Button",
			AR_EXECUTE_ON_RETURN, "Return",
			AR_EXECUTE_ON_SUBMIT, "Submit",
			AR_EXECUTE_ON_MODIFY, "Modify",
			AR_EXECUTE_ON_DISPLAY, "Display",
			//AR_EXECUTE_ON_MODIFY_ALL, "Modify All", // its now a $OPERATION$
			//AR_EXECUTE_ON_MENU_OPEN, "OPEN",        // unsupported
			AR_EXECUTE_ON_MENU_CHOICE, "Menu Choice",
			AR_EXECUTE_ON_LOSE_FOCUS, "Loose Focus",
			AR_EXECUTE_ON_SET_DEFAULT, "Set Default",
			AR_EXECUTE_ON_QUERY, "Search",
			AR_EXECUTE_ON_AFTER_MODIFY, "After Modify",
			AR_EXECUTE_ON_AFTER_SUBMIT, "After Submit",
			AR_EXECUTE_ON_GAIN_FOCUS, "Gain Focus",
			AR_EXECUTE_ON_WINDOW_OPEN, "Window Open",
			AR_EXECUTE_ON_WINDOW_CLOSE, "Window Close",
			AR_EXECUTE_ON_UNDISPLAY, "Un-Display",
			AR_EXECUTE_ON_COPY_SUBMIT, "Copy To New",
			AR_EXECUTE_ON_LOADED, "Window Loaded",			
			// AR_EXECUTE_ON_INTERVAL is obsolete; 7.5 handle this only as object property; 
			// and older servers use this flag AND the object property. so its not needed 
			// to check this at all
			AR_EXECUTE_ON_EVENT, "Event",

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
			AR_EXECUTE_ON_TABLE_CONTENT_CHANGE, "Table Refresh",
			AR_EXECUTE_ON_HOVER_FIELD_LABEL, "Hover On Label",
			AR_EXECUTE_ON_HOVER_FIELD_DATA, "Hover On Data",
			AR_EXECUTE_ON_HOVER_FIELD, "Hover On Field",
			AR_EXECUTE_ON_PAGE_EXPAND, "Expand",
			AR_EXECUTE_ON_PAGE_COLLAPSE, "Collapse",
#endif
			0,NULL
		};

		for (unsigned int k = 0; true; ++k)
		{
			if (executeText[k].exec == 0) break;

			if ( (executeMask & executeText[k].exec) != 0)
			{
				if (!singleLine && strmHasContent) strm << "<br/>";
				if (singleLine && strmHasContent) strm << ", ";
				strm << executeText[k].text;
				strmHasContent = true;
			}
		}
		if (props != NULL)
		{
			// now check for special execution properties
			ARValueStruct* val = props->GetAndUseValue(AR_OPROP_INTERVAL_VALUE);
			if (val != NULL)
				if (val->dataType == AR_DATA_TYPE_INTEGER)
				{
					if (!singleLine && strmHasContent) strm << "<br/>";
					if (singleLine && strmHasContent) strm << ", ";
					strm << "Interval";
					if (!singleLine) strm << ": " << val->u.intVal;
					strmHasContent = true;
				}
		}
		if(!strmHasContent)
		{
			strm << "None";
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in ActiveLink GetExecuteOn: " << e.what() << endl;
	}
	return strm.str();
}
