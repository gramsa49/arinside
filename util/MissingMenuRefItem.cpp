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

#include "StdAfx.h"
#include ".\missingmenurefitem.h"

CMissingMenuRefItem::CMissingMenuRefItem()
{
}

CMissingMenuRefItem::CMissingMenuRefItem(const string& menuName, int arsStructItemType, int schemaInsideId, int fieldInsideId)
{
	this->menuName = menuName;
	this->arsStructItemType = arsStructItemType;
	this->schemaInsideId = schemaInsideId;
	this->fieldInsideId = fieldInsideId;
}

/* constructor for missing menu references */
CMissingMenuRefItem::CMissingMenuRefItem(const string& menuName, int arsStructItemType, string fromName)
{
	this->arsStructItemType = arsStructItemType;
	this->fromName = fromName;
	this->menuName = menuName;
	this->fieldInsideId = -1;
	this->schemaInsideId = -1;
}

CMissingMenuRefItem::~CMissingMenuRefItem(void)
{
}

bool CMissingMenuRefItem::operator <(const CMissingMenuRefItem& item)
{
	return (menuName.compare(item.menuName) < 0);
}

bool CMissingMenuRefItem::operator ==(const CMissingMenuRefItem &item)
{
	// the main matching criteria is the menu name
	if (menuName.compare(item.menuName) != 0) return false;

	// next criteria is the reference type
	if (arsStructItemType != item.arsStructItemType) return false;

	// now use different compare steps according to the type
	if (arsStructItemType == AR_STRUCT_ITEM_XML_FIELD)
	{
		// for a field reference just check schema- and field-id
		if (schemaInsideId != item.schemaInsideId) return false;
		if (fieldInsideId != item.fieldInsideId) return false;
	}
	else
	{
		// for all other references (ALs / Containers) just
		// check the from-name
		if (fromName.compare(item.fromName) != 0) return false;
	}
	return true;
}