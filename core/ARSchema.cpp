
/****************************************************************************** 
 * 
 *  file:  ARSchema.cpp
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
#include "ARSchema.h"

CARSchema::CARSchema()
{	
	this->fieldList.clear();
	this->vuiList.clear();

	this->alias = "";
	this->fieldList.clear();
	this->vuiList.clear();	

	this->objPropList.props = NULL;
}

CARSchema::CARSchema(string name, int insideId)
{
	try
	{
		this->name = name;	
		this->insideId = insideId;

		this->alias = "";
		this->fieldList.clear();
		this->vuiList.clear();	

		this->objPropList.props = NULL;
	}
	catch(...)
	{
	}
}

CARSchema::~CARSchema(void)
{
	FreeARCompoundSchema(&schema, false);		
	FreeARPermissionList(&groupList, false);
	FreeARInternalIdList(&admingrpList, false);
	FreeAREntryListFieldList(&getListFields, false);
	FreeARSortList(&sortList, false);
	FreeARIndexList(&indexList, false);
	FreeARArchiveInfoStruct(&archiveInfo, false);
	FreeARAuditInfoStruct(&auditInfo, false);	
	
	if(objPropList.props != NULL)
		FreeARPropList(&objPropList, false);
}

string CARSchema::GetURL(int rootLevel)
{
	stringstream tmp;
	tmp << CWebUtil::RootPath(rootLevel) << "schema/" << this->insideId << "/" << CWebUtil::DocName("index");	
	return CWebUtil::Link(this->name, tmp.str(), "schema.gif", rootLevel);
}

string CARSchema::WebAlias()
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
					case AR_OPROP_FORM_NAME_WEB_ALIAS:
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
		cout << "EXCEPTION enumerating web alias" << endl;
	}

	return strm.str();
}


string CARSchema::LinkToVui(int vuiId, int fromRootLevel)
{	
	list<CARVui>::iterator vuiIter;			
	for(vuiIter = this->vuiList.begin(); vuiIter != this->vuiList.end(); vuiIter++)
	{
		CARVui *vui = &(*vuiIter);
		if(vui->insideId == vuiId)
		{
			return vui->GetURL(fromRootLevel);
		}
	}
	return EmptyValue;
}


string CARSchema::LinkToVui(string vuiLabel, int fromRootLevel)
{	
	list<CARVui>::iterator vuiIter;				
	for(vuiIter = this->vuiList.begin(); vuiIter != this->vuiList.end(); vuiIter++)
	{
		CARVui *vui = &(*vuiIter);

		for(unsigned int i=0; i<vui->objPropList.numItems; i++)
		{
			switch(vui->objPropList.props[i].prop)
			{
				case AR_DPROP_LABEL:
					{
						if(vui->objPropList.props[i].value.u.charVal != NULL)
						{
							if(strcmp(vuiLabel.c_str(), vui->objPropList.props[i].value.u.charVal)==0)
							{
								stringstream tmp;
								tmp << CWebUtil::RootPath(fromRootLevel) << "schema/" << this->insideId << "/" << CWebUtil::DocName("vui_" + vui->FileID());
								
								return CWebUtil::Link(vui->objPropList.props[i].value.u.charVal, tmp.str(), "", fromRootLevel);
							}
						}
					}
				break;
			}
		}
	}

	return vuiLabel;
}

string CARSchema::VuiGetLabel(int vuiId)
{
	stringstream strmTmp;
	strmTmp.str("");

	strmTmp << vuiId;    
	
	list<CARVui>::iterator vuiIter;			
	for(vuiIter = this->vuiList.begin(); vuiIter != this->vuiList.end(); vuiIter++)
	{
		CARVui *vui = &(*vuiIter);
		if(vui->insideId == vuiId)
		{
			return vui->Label();
		}
	}

	return strmTmp.str();
}
