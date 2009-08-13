
/****************************************************************************** 
 * 
 *  file:  ARActiveLink.cpp
 * 
 *  Copyright (c) 2007, Stefan Nerlich | stefan.nerlich@hotmail.com 
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/ 

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






