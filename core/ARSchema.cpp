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
#include "../lists/ARSchemaList.h"
#include "ARField.h"
#include "ARVui.h"
#include "../ARInside.h"

CARSchema::CARSchema()
: CARServerObject(-1)
{
}

CARSchema::CARSchema(int insideId)
: CARServerObject(insideId)
{	
}

CARSchema::CARSchema(const string& schemaName)
: CARServerObject(-1)
{
	insideId = CARInside::GetInstance()->schemaList.Find(schemaName.c_str());
}

CARSchema::~CARSchema(void)
{
}

bool CARSchema::Exists()
{
	return (insideId >= 0 && (unsigned int)insideId < CARInside::GetInstance()->schemaList.GetCount());
}

bool CARSchema::IsClonable()
{
	return true;
}

CARServerObject* CARSchema::Clone()
{
	return new CARSchema(*this);
}

string CARSchema::GetURL(int rootLevel, bool useImage)
{
	CPageParams file(PAGE_DETAILS, this);
	return CWebUtil::Link(this->GetName(), file, (useImage ? "schema.gif" : ""), rootLevel);
}

string CARSchema::WebAlias()
{
	stringstream strm;
	strm.str("");

	try
	{
		const ARPropList& propList = this->GetProps();
		for(unsigned int i=0; i < propList.numItems; ++i)
		{
			if (propList.props[i].prop == AR_OPROP_FORM_NAME_WEB_ALIAS)
			{
				if(propList.props[i].value.dataType == AR_DATA_TYPE_CHAR && propList.props[i].value.u.charVal != NULL)
				{
					strm << propList.props[i].value.u.charVal;
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
	if (vuiId >= 0)
	{
		CARVui vui(this->GetInsideId(), vuiId);
		if (vui.Exists())
		{
			return vui.GetURL(fromRootLevel);
		}
	}
	return EmptyValue;
}


string CARSchema::LinkToVui(string vuiLabel, int fromRootLevel)
{	
	CARVUIList* vuiList = this->GetVUIs();
	unsigned int vuiCount = vuiList->GetCount();
	for (unsigned int vuiIndex = 0; vuiIndex < vuiCount; ++vuiIndex)
	{
		CARVui vui(GetInsideId(), 0, vuiIndex);
		const ARPropList& propList = vui.GetProps();

		for(unsigned int i=0; i < propList.numItems; ++i)
		{
			if (propList.props[i].prop == AR_DPROP_LABEL && propList.props[i].value.dataType == AR_DATA_TYPE_CHAR)
			{
				if(vuiLabel.compare(propList.props[i].value.u.charVal) == 0)
				{
					return CWebUtil::Link(propList.props[i].value.u.charVal, CPageParams(PAGE_DETAILS, &vui), "", fromRootLevel);
				}
				break;
			}
		}
	}

	return "<span class=\"fieldNotFound\">" + vuiLabel + "</span>";
}

string CARSchema::VuiGetLabel(int vuiId)
{
	unsigned int vuiCount = this->GetVUIs()->GetCount();
	for(unsigned int vuiIndex = 0; vuiIndex < vuiCount; ++vuiIndex)
	{
		CARVui vui(GetInsideId(), 0, vuiIndex);
		if(vui.GetId() == vuiId)
		{
			return vui.Label();
		}
	}

	stringstream strmTmp;
	strmTmp << vuiId;    
	return strmTmp.str();
}

string CARSchema::GetName()
{
	return CARInside::GetInstance()->schemaList.SchemaGetName(GetInsideId());
}

string CARSchema::GetName() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetName(GetInsideId());
}

string CARSchema::GetNameFirstChar()
{
	return CUtil::String2Comp(std::string(1, CARInside::GetInstance()->schemaList.SchemaGetName(GetInsideId())[0]));
}

bool CARSchema::NameStandardFirstChar()
{
	return CARObject::NameStandardFirstChar(GetNameFirstChar());
}

const char* CARSchema::GetHelpText() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetHelptext(GetInsideId());
}

ARTimestamp CARSchema::GetTimestamp()
{
	return CARInside::GetInstance()->schemaList.SchemaGetTimestamp(GetInsideId());
}

const ARAccessNameType& CARSchema::GetOwner() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetOwner(GetInsideId());
}

const ARAccessNameType& CARSchema::GetLastChanged() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetModifiedBy(GetInsideId());
}

const char* CARSchema::GetChangeDiary() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetChangeDiary(GetInsideId());
}

const ARNameType& CARSchema::GetARName() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetName(GetInsideId());
}

const ARCompoundSchema& CARSchema::GetCompound() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetCompound(GetInsideId());
}

const ARPermissionList& CARSchema::GetPermissions() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetPermissions(GetInsideId());
}

const ARInternalIdList& CARSchema::GetSubadmins() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetSubadmins(GetInsideId());
}

const AREntryListFieldList& CARSchema::GetResultFields() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetResultFields(GetInsideId());
}

const ARSortList& CARSchema::GetSortList() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetSortList(GetInsideId());
}

const ARIndexList& CARSchema::GetIndexList() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetIndexList(GetInsideId());
}

const ARArchiveInfoStruct& CARSchema::GetArchiveInfo() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetArchiveInfo(GetInsideId());
}

const ARAuditInfoStruct& CARSchema::GetAuditInfo() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetAuditInfo(GetInsideId());
}

const ARNameType& CARSchema::GetDefaultVUI() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetDefaultVUI(GetInsideId());
}

const ARPropList& CARSchema::GetProps() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetPropList(GetInsideId());
}

const string& CARSchema::GetAppRefName() const
{
	return CARInside::GetInstance()->schemaList.SchemaGetAppRefName(GetInsideId());
}

void CARSchema::SetAppRefName(const string& appName)
{
	return CARInside::GetInstance()->schemaList.SchemaSetAppRefName(GetInsideId(), appName);
}

CARFieldList* CARSchema::GetFields()
{
	return CARInside::GetInstance()->schemaList.SchemaGetFields(GetInsideId());
}

CARVUIList* CARSchema::GetVUIs()
{
	return CARInside::GetInstance()->schemaList.SchemaGetVUIs(GetInsideId());
}

void CARSchema::AddFilter(const CARFilter &filter)
{
	CARInside::GetInstance()->schemaList.SchemaAddFilter(GetInsideId(), filter);
}

void CARSchema::AddActiveLink(const CARActiveLink& actlink)
{
	CARInside::GetInstance()->schemaList.SchemaAddActiveLink(GetInsideId(), actlink);
}

void CARSchema::AddEscalation(const CAREscalation& escalation)
{
	CARInside::GetInstance()->schemaList.SchemaAddEscalation(GetInsideId(), escalation);
}

void CARSchema::AddMissingFieldReference(int fieldId, const CFieldRefItem &refItem)
{
	CARInside::GetInstance()->schemaList.SchemaAddMissingFieldReference(GetInsideId(), fieldId, refItem);
}

const CARSchema::MissingReferenceList* CARSchema::GetMissingReferences()
{
	return CARInside::GetInstance()->schemaList.SchemaGetMissingReferences(GetInsideId());
}

const CARSchemaList::ObjectRefList& CARSchema::GetActiveLinks()
{
	return CARInside::GetInstance()->schemaList.SchemaGetALReferences(GetInsideId());
}

const CARSchemaList::ObjectRefList& CARSchema::GetFilters()
{
	return CARInside::GetInstance()->schemaList.SchemaGetFilterReferences(GetInsideId());
}

const CARSchemaList::ObjectRefList& CARSchema::GetEscalations()
{
	return CARInside::GetInstance()->schemaList.SchemaGetEscalationReferences(GetInsideId());
}
