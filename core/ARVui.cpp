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
#include "ARProplistHelper.h"

// TODO: write a description about how to use both types of calling this constructor
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

// this searches for a VUI with the specified label
CARVui::CARVui(unsigned int schemaInsideId, const string& vuiLabel)
: CARServerObject(-1), schema(schemaInsideId)
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

	for (unsigned int vuiPos = 0; vuiPos < vuiList->GetCount(); ++vuiPos)
	{
		ARValueStruct* val = CARProplistHelper::Find(vuiList->VUIGetProps(vuiPos), AR_DPROP_LABEL);
		if (val != NULL && val->dataType == AR_DATA_TYPE_CHAR && vuiLabel == val->u.charVal)
		{
			vuiIndex = vuiPos;
			break;
		}
	}	
}

CARVui::~CARVui(void)
{
}

bool CARVui::Exists()
{
	return (vuiIndex >= 0 && (unsigned int)vuiIndex < vuiList->GetCount());
}

bool CARVui::IsClonable() const
{
	return true;
}

CARServerObject* CARVui::Clone() const
{
	return new CARVui(*this);
}

string CARVui::GetURL(int rootLevel, bool showImage) const
{
	return CWebUtil::Link(this->GetName(), CPageParams(PAGE_DETAILS, this), "", rootLevel);
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

const CARSchema& CARVui::GetSchema() const
{
	return this->schema;
}
