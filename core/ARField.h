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
#include "ARSchema.h"

class CARField :
	public CARServerObject
{
private:

public:
	CARField();
	CARField(int SchemaInsideId, unsigned int fieldId, int SchemaFieldIndex = -1);
	~CARField(void) {}

	// use this function after calling the constructor with just a fieldId and no index specified.
	bool Exists();

	// implement functions inherited from CARObject
	string GetName();
	string GetName() const;
	string GetNameFirstChar();
	bool NameStandardFirstChar();

	// implement functions inherited from CARServerObject
	const char* GetHelpText() const;
	ARTimestamp GetTimestamp();
	const ARAccessNameType& GetOwner() const;
	const ARAccessNameType& GetLastChanged() const;
	const char* GetChangeDiary() const;

	// other data access functions
	ARInternalId GetFieldId() const;
	const ARFieldMappingStruct& GetMapping() const;
	unsigned int GetDataType() const;
	unsigned int GetOption() const;
	unsigned int GetCreateMode() const;
	unsigned int GetFieldOption() const;
	const ARValueStruct& GetDefaultValue() const;
	const ARPermissionList& GetPermissions() const;
	const ARFieldLimitStruct& GetLimits() const;
	const ARDisplayInstanceList& GetDisplayInstances() const;
	const CARSchema& GetSchema() const;

	// helpers
	string GetURL(int rootLevel, bool showImage = true);

	// class type support
	int GetServerObjectTypeXML() const { return AR_STRUCT_ITEM_XML_FIELD; }
	bool IsClonable() const;
	CARServerObject* Clone() const;

	// internal workflow references
	typedef CARFieldList::ReferenceList ReferenceList;
	bool ReferenceExists(const CRefItem& refItem) const;
	void AddReference(const CRefItem& refItem) const;
	const ReferenceList& GetReferences() const;
private:
	CARSchema schema;
	CARFieldList *fieldList;
	int fieldIndex;
};
