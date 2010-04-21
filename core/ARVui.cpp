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
#include "../lists/ARVUIList.h"

CARVui::CARVui(unsigned int SchemaInsideId, unsigned int vuiId, int SchemaVuiIndex)
: CARServerObject(vuiId), schema(SchemaInsideId)
{
	if (!schema.Exists())
	{
		insideId = -1;
		vuiIndex = -1;
		vuiList = NULL;
		return;
	}

	// look up and store the vui index for faster access	
	vuiList = schema.GetVUIs();

	// if the vui is accessed via SchemaVuiIndex (and has a valid range) .. use it
	if (vuiId == 0 && SchemaVuiIndex >= 0 && (unsigned int)SchemaVuiIndex < vuiList->GetCount())
	{
		vuiIndex = SchemaVuiIndex;
		insideId = GetId();
	}
	// ok ... accessing by vuiId ... look up the index
	else if (vuiId > 0)
	{
		vuiIndex = vuiList->Find(vuiId);
	}
	// default .. this could be a out of bounds. the caller should know what to do!
	else
	{
		vuiIndex = SchemaVuiIndex;
	}
}

CARVui::~CARVui(void)
{
}

bool CARVui::Exists()
{
	return (vuiIndex >= 0 && (unsigned int)vuiIndex < vuiList->GetCount());
}

bool CARVui::IsClonable()
{
	return true;
}

CARServerObject* CARVui::Clone()
{
	return new CARVui(*this);
}

string CARVui::GetURL(int rootLevel, bool showImage)
{
	stringstream tmp;
	tmp.str("");

	tmp << CWebUtil::RootPath(rootLevel) << "schema/" << schema.GetInsideId() << "/" << CWebUtil::DocName("vui_" + this->FileID());	
	return CWebUtil::Link(this->GetName(), tmp.str(), "", rootLevel);
}

string CARVui::webAlias()
{
	try
	{
		const ARPropList& propList = GetProps();

		for(unsigned int i=0; i< propList.numItems; ++i)
		{
			if (propList.props[i].prop == AR_OPROP_VIEW_LABEL_WEB_ALIAS &&
			    propList.props[i].value.dataType == AR_DATA_TYPE_CHAR)
			{
				return propList.props[i].value.u.charVal;
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating view web alias: " << e.what() << endl;
	}
	return "";
}


string CARVui::Label()
{
	stringstream strm;
	strm.str("");

	try
	{
		const ARPropList& propList = GetProps();

		for(unsigned int i=0; i< propList.numItems; ++i)
		{
			if (propList.props[i].prop == AR_DPROP_LABEL &&
			    propList.props[i].value.dataType == AR_DATA_TYPE_CHAR)
			{
				return propList.props[i].value.u.charVal;
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating view label: " << e.what() << endl;
	}

	return strm.str();
}

string CARVui::GetName()
{
	return vuiList->VUIGetName(vuiIndex);
}

string CARVui::GetName() const
{
	return vuiList->VUIGetName(vuiIndex);
}

string CARVui::GetNameFirstChar()
{
	return CUtil::String2Comp(std::string(1, vuiList->VUIGetName(vuiIndex)[0]));
}

bool CARVui::NameStandardFirstChar()
{
	return CARObject::NameStandardFirstChar(GetNameFirstChar());
}

const char* CARVui::GetHelpText() const
{
	return vuiList->VUIGetHelptext(vuiIndex);
}

ARTimestamp CARVui::GetTimestamp()
{
	return vuiList->VUIGetTimestamp(vuiIndex);
}

const ARAccessNameType& CARVui::GetOwner() const
{
	return vuiList->VUIGetOwner(vuiIndex);
}

const ARAccessNameType& CARVui::GetLastChanged() const
{
	return vuiList->VUIGetModifiedBy(vuiIndex);
}

const char* CARVui::GetChangeDiary() const
{
	return vuiList->VUIGetChangeDiary(vuiIndex);
}

ARInternalId CARVui::GetId()
{
	return vuiList->VUIGetId(vuiIndex);
}

const ARLocaleType& CARVui::GetLocale() const
{
	return vuiList->VUIGetLocale(vuiIndex);
}

int CARVui::GetType()
{
	return vuiList->VUIGetType(vuiIndex);
}

const ARPropList& CARVui::GetProps() const
{
	return vuiList->VUIGetProps(vuiIndex);
}
