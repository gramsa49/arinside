
/****************************************************************************** 
 * 
 *  file:  ARFilter.cpp
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
#include ".\arfilter.h"

CARFilter::CARFilter(string name, int insideId)
{
	this->order = 0;
	this->opSet = 0;
	this->enable = 0;
	this->name = name;
	this->insideId = insideId;

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
		if(opSet == NULL)
		{
			return "None";
		}
		else
		{
			unsigned int bitmask[5] = { 1, 1<<1, 1<<2, 1<<3, 1<<4 };
			char executeText[5][30] = { "Get Entry", "Modify", "Submit", "Delete", "Merge"};

			for (unsigned int k= 0; k < 5; k++)
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
	catch(...)
	{
		cout << "EXCEPTION in Filter GetExecuteOn" << endl; 
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
			unsigned int bitmask[5] = { 1, 1<<1, 1<<2, 1<<3, 1<<4 };
			char executeText[5][30] = { "Get Entry", "Modify", "Submit", "Delete", "Merge"};

			for (unsigned int k= 0; k < 5; k++)
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
	catch(...)
	{
		cout << "EXCEPTION in Filter GetExecuteOnEx" << endl; 
	}
	return strm.str();
}


