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
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "ARField.h"
#include "../lists/ARFieldList.h"

CARField::CARField()
: CARServerObject(-1), schema(-1)
{
	fieldIndex = -1;
}

CARField::CARField(int SchemaInsideId, unsigned int fieldId, int SchemaFieldIndex)
: CARServerObject(fieldId), schema(SchemaInsideId)
{	
	if (!schema.Exists())
	{
		insideId = -1;
		fieldIndex = -1;
		return;
	}

	// look up and store the field index for faster access
	fieldList = schema.GetFields();

	if (fieldId == 0 && SchemaFieldIndex >= 0 && (unsigned int)SchemaFieldIndex < fieldList->GetCount())
	{
		fieldIndex = SchemaFieldIndex;
		insideId = GetFieldId();
	}
	else if (fieldId > 0)
	{
		fieldIndex = fieldList->Find(fieldId);
	}
	else
	{
		fieldIndex = SchemaFieldIndex;
	}
}

bool CARField::Exists()
{
	return (fieldIndex >= 0 && (unsigned int)fieldIndex < fieldList->GetCount());
}

bool CARField::IsClonable() const
{
	return true;
}

CARServerObject* CARField::Clone() const
{
	return new CARField(*this);
}

string CARField::GetURL(int rootLevel, bool showImage) const
{
	CPageParams file(PAGE_DETAILS, this);
	return CWebUtil::Link(GetName(), file, "", rootLevel);
}

string CARField::GetName()
{
	return fieldList->FieldGetName(fieldIndex);
}

string CARField::GetName() const
{
	return fieldList->FieldGetName(fieldIndex);
}

string CARField::GetNameFirstChar()
{
	return CUtil::String2Comp(std::string(1, fieldList->FieldGetName(fieldIndex)[0]));
}

bool CARField::NameStandardFirstChar()
{
	return CARObject::NameStandardFirstChar(GetNameFirstChar());
}

const char* CARField::GetHelpText() const
{
	return fieldList->FieldGetHelptext(fieldIndex);
}

ARTimestamp CARField::GetTimestamp()
{
	return fieldList->FieldGetTimestamp(fieldIndex);
}

const ARAccessNameType& CARField::GetOwner() const
{
	return fieldList->FieldGetOwner(fieldIndex);
}

const ARAccessNameType& CARField::GetLastChanged() const
{
	return fieldList->FieldGetModifiedBy(fieldIndex);
}

const char* CARField::GetChangeDiary() const
{
	return fieldList->FieldGetChangeDiary(fieldIndex);
}

ARInternalId CARField::GetFieldId() const
{
	return fieldList->FieldGetFieldId(fieldIndex); 
}

const ARFieldMappingStruct& CARField::GetMapping() const
{ 
	return fieldList->FieldGetMapping(fieldIndex);
}

unsigned int CARField::GetDataType() const
{
	return fieldList->FieldGetDataType(fieldIndex);
}

unsigned int CARField::GetOption() const
{
	return fieldList->FieldGetOption(fieldIndex);
}

unsigned int CARField::GetCreateMode() const
{
	return fieldList->FieldGetCreateMode(fieldIndex);
}

unsigned int CARField::GetFieldOption() const
{
	return fieldList->FieldGetOption(fieldIndex);
}

const ARValueStruct& CARField::GetDefaultValue() const
{
	return fieldList->FieldGetDefaultValue(fieldIndex);
}

const ARPermissionList& CARField::GetPermissions() const
{
	return fieldList->FieldGetPermissions(fieldIndex);
}

const ARFieldLimitStruct& CARField::GetLimits() const
{
	return fieldList->FieldGetLimits(fieldIndex);
}

const ARDisplayInstanceList& CARField::GetDisplayInstances() const
{
	return fieldList->FieldGetDisplayInstances(fieldIndex);
}

const CARSchema& CARField::GetSchema() const
{
	return schema;
}

bool CARField::ReferenceExists(const CRefItem &refItem) const
{
	return fieldList->FieldReferenceExists(fieldIndex, refItem);
}

void CARField::AddReference(const CRefItem& refItem) const
{
	fieldList->FieldReferenceAdd(fieldIndex, refItem);
}

const CARFieldList::ReferenceList& CARField::GetReferences() const
{
	return fieldList->FieldReferenceList(fieldIndex);
}
