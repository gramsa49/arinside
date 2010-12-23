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
#include "FieldRefItem.h"
#include "../ARInside.h"
#include "../core/ARActiveLink.h"
#include "../core/ARFilter.h"
#include "../core/AREscalation.h"

CFieldRefItem::CFieldRefItem()
:CRefItem(0), arsStructItemType(0), fromFieldId(0), fieldInsideId(0), schemaInsideId(0)
{
}

CFieldRefItem::CFieldRefItem(int arsStructItemType, string fromName, string description, int fieldInsideId, int schemaInsideId)
: CRefItem(0)
{
	this->arsStructItemType = arsStructItemType;
	this->fromName = fromName;
	this->fromFieldId = 0;
	this->description = description;
	this->fieldInsideId = fieldInsideId;
	this->schemaInsideId = schemaInsideId;
}

CFieldRefItem::~CFieldRefItem(void)
{
}

string CFieldRefItem::GetObjectName()
{
	return fromName;
}

unsigned int CFieldRefItem::GetObjectEnabled(bool &supportsEnabled)
{
	switch (this->arsStructItemType)
	{
	case AR_STRUCT_ITEM_XML_FILTER:
		{
			CARActiveLink al(fromName);
			if (al.Exists())
			{
				supportsEnabled = true;
				return al.GetEnabled();
			}
		}
		break;		
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
		{
			int fltID = CARInside::GetInstance()->filterList.Find(fromName.c_str());
			if (fltID > -1)
			{
				CARFilter flt(fltID);
				supportsEnabled = true;
				return flt.GetEnabled();
			}
		}
		break;
	case AR_STRUCT_ITEM_XML_ESCALATION:
		{
			int escID = CARInside::GetInstance()->escalationList.Find(fromName.c_str());
			if (escID > -1)
			{
				CAREscalation esc(escID);
				supportsEnabled = true;
				return esc.GetEnabled();
			}
		}
		break;		
	}
	supportsEnabled = false;
	return false;
}

string CFieldRefItem::GetDescription()
{
	return description;
}
