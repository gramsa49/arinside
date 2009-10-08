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

string CARSchema::GetURL(int rootLevel, bool useImage)
{
	stringstream tmp;
	tmp << CWebUtil::RootPath(rootLevel) << "schema/" << this->insideId << "/" << CWebUtil::DocName("index");	
	if (useImage)
		return CWebUtil::Link(this->name, tmp.str(), "schema.gif", rootLevel);
	else
		return CWebUtil::Link(this->name, tmp.str(), "", rootLevel);
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
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating web alias: " << e.what() << endl;
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
