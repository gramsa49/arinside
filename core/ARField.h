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

#pragma once
#include "ARServerObject.h"

class CARField :
	public CARServerObjectWithData
{
public:
	CARField(int insideId);
	~CARField(void);

	ARInternalId fieldId;
	ARNameType fieldName;
	ARFieldMappingStruct fieldMap;
	unsigned int dataType;
	unsigned int option;
	unsigned int createMode;
	ARValueStruct defaultVal;
	ARPermissionList permissions;
	ARFieldLimitStruct limit;
	ARDisplayInstanceList dInstanceList;
	int schemaInsideId;

	string GetURL(int rootLevel, bool showImage = true);
	int GetServerObjectTypeXML() { return AR_STRUCT_ITEM_XML_FIELD; }
};
