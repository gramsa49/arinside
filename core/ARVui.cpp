
/****************************************************************************** 
 * 
 *  file:  ARVui.cpp
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
#include ".\arvui.h"

CARVui::CARVui(int insideId)
{	
	this->insideId = insideId;
	this->vuiType = 0;

	this->objPropList.props = NULL;
	bARFree = true;
}

CARVui::~CARVui(void)
{
	if(bARFree)
	{
		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
}

string CARVui::GetURL(int rootLevel)
{
	stringstream tmp;
	tmp.str("");

	tmp << CWebUtil::RootPath(rootLevel) << "schema/" << this->schemaInsideId << "/" << CWebUtil::DocName("vui_" + this->FileID());	
	return CWebUtil::Link(this->name, tmp.str(), "", rootLevel);
}

string CARVui::webAlias()
{
	stringstream strm;
	strm.str("");

	try
	{
		if(objPropList.props != NULL)
		{
			for(unsigned int i=0; i< this->objPropList.numItems; i++)
			{
				switch(this->objPropList.props[i].prop)
				{
					case AR_OPROP_VIEW_LABEL_WEB_ALIAS:
					{
						strm.str("");
						if(this->objPropList.props[i].value.u.charVal != NULL)
						{
							strm.str("");
							strm << this->objPropList.props[i].value.u.charVal;
						}
					}
					break;
				}				
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating view web alias" << endl;
	}

	return strm.str();
}


string CARVui::Label()
{
	stringstream strm;
	strm.str("");

	try
	{
		if(objPropList.props != NULL)
		{
			for(unsigned int i=0; i< this->objPropList.numItems; i++)
			{
				switch(this->objPropList.props[i].prop)
				{
					case AR_DPROP_LABEL:
					{
						strm.str("");
						if(this->objPropList.props[i].value.u.charVal != NULL)
						{
							strm.str("");
							strm << this->objPropList.props[i].value.u.charVal;
						}
					}
					break;
				}				
			}
		}
	}
	catch(...)
	{
		cout << "EXCEPTION enumerating view label" << endl;
	}

	return strm.str();
}
