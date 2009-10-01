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
#include "ARVui.h"

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
