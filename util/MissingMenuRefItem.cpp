//Copyright (C) 2009 John Luthgers | jls17
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
#include "MissingMenuRefItem.h"

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
	return (compare(item) < 0);
}

bool CMissingMenuRefItem::operator ==(const CMissingMenuRefItem &item)
{
	return compare(item) == 0;
}

int CMissingMenuRefItem::compare(const CMissingMenuRefItem &item)
{
	int result = 0;

	// the main matching criteria is the menu name
	result = menuName.compare(item.menuName);
	if (result != 0) return result;

	// next criteria is the reference type
	int leftOrder = GetStructTypeOrder(arsStructItemType);
	int rightOrder = GetStructTypeOrder(item.arsStructItemType);
	if (leftOrder < rightOrder) return -1;
	if (leftOrder > rightOrder) return 1;

	// now use different compare steps according to the type
	if (arsStructItemType == AR_STRUCT_ITEM_XML_FIELD)
	{
		// for a field reference just check schema- and field-id
		if (schemaInsideId < item.schemaInsideId) return -1;
		if (schemaInsideId > item.schemaInsideId) return 1;
		if (fieldInsideId < item.fieldInsideId) return -1;
		if (fieldInsideId > item.fieldInsideId) return 1;
		return 0;
	}
	else
	{
		// for all other references (ALs / Containers) just
		// check the from-name
		result = fromName.compare(item.fromName);
		return result;
	}
}

#define MMRI_STRUCT_ITEM_COUNT 15
int structTypeSortOrder[MMRI_STRUCT_ITEM_COUNT] = {
	0x01,      // AR_STRUCT_ITEM_XML_SCHEMA
	0x12,      // AR_STRUCT_ITEM_XML_FILTER
	0x11,      // AR_STRUCT_ITEM_XML_ACTIVE_LINK
	0x16,      // AR_STRUCT_ITEM_XML_CHAR_MENU
	0x14,      // AR_STRUCT_ITEM_XML_ESCALATION
	0x19,      // AR_STRUCT_ITEM_XML_DIST_MAP
	0x15,      // AR_STRUCT_ITEM_XML_CONTAINER
	0x19,      // AR_STRUCT_ITEM_XML_DIST_POOL
	0x02,      // AR_STRUCT_ITEM_XML_VUI
	0x03,      // AR_STRUCT_ITEM_XML_FIELD
	0x17,      // AR_STRUCT_ITEM_XML_APP
	0xFF,      // AR_STRUCT_ITEM_XML_SCHEMA_DATA
	0xFF,      // AR_STRUCT_ITEM_XML_LOCK_BLOCK
	0x20,      // AR_STRUCT_ITEM_XML_IMAGE
	0x04       // AR_STRUCT_ITEM_XML_LOCALE_VUI
};

int CMissingMenuRefItem::GetStructTypeOrder(int xmlStructItemType)
{
	if (xmlStructItemType < AR_STRUCT_ITEM_XML_SCHEMA)
		throw "Illegal struct item type!";
	xmlStructItemType -= AR_STRUCT_ITEM_XML_SCHEMA;
	
	if (xmlStructItemType >= MMRI_STRUCT_ITEM_COUNT)
		throw "Illegal struct item type!";

	return structTypeSortOrder[xmlStructItemType];
}
