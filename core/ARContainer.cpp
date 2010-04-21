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
#include "ARContainer.h"
#include "../ARInside.h"

CARContainer::CARContainer(int insideId)
: CARServerObject(insideId)
{
}

CARContainer::CARContainer(const string& name)
: CARServerObject(-1)
{
	insideId = CARInside::GetInstance()->containerList.Find(name.c_str());
}

CARContainer::CARContainer(const char* name)
: CARServerObject(-1)
{
	insideId = CARInside::GetInstance()->containerList.Find(name);
}

bool CARContainer::Exists()
{
	return (insideId >= 0 && (unsigned int)insideId < CARInside::GetInstance()->containerList.GetCount());
}

CARContainer::~CARContainer(void)
{
}

bool CARContainer::IsClonable()
{
	return true;
}

CARServerObject* CARContainer::Clone()
{
	return new CARContainer(*this);
}

string CARContainer::GetURL(int rootLevel, bool showImage)
{
	unsigned int type = GetType();
	string tmp = CWebUtil::RootPath(rootLevel)+CAREnum::ContainerDir(type)+"/"+this->FileID()+"/"+CWebUtil::DocName("index");
	return CWebUtil::Link(GetName(), tmp, (showImage ? CAREnum::ContainerImage(type) : ""), rootLevel);
}

string CARContainer::GetName()
{
	return CARInside::GetInstance()->containerList.ContainerGetName(GetInsideId());
}

string CARContainer::GetName() const
{
	return CARInside::GetInstance()->containerList.ContainerGetName(GetInsideId());
}

string CARContainer::GetNameFirstChar()
{
	return CUtil::String2Comp(std::string(1, CARInside::GetInstance()->containerList.ContainerGetName(GetInsideId())[0]));
}

bool CARContainer::NameStandardFirstChar()
{
	return CARObject::NameStandardFirstChar(GetNameFirstChar());
}

const string& CARContainer::GetAppRefName() const
{
	return CARInside::GetInstance()->containerList.ContainerGetAppRefName(GetInsideId());
}

void CARContainer::SetAppRefName(const string &appName)
{
	return CARInside::GetInstance()->containerList.ContainerSetAppRefName(GetInsideId(), appName);
}

const char* CARContainer::GetHelpText() const
{
	return CARInside::GetInstance()->containerList.ContainerGetHelptext(GetInsideId());
}

ARTimestamp CARContainer::GetTimestamp()
{
	return CARInside::GetInstance()->containerList.ContainerGetTimestamp(GetInsideId());
}

const ARAccessNameType& CARContainer::GetOwner() const
{
	return CARInside::GetInstance()->containerList.ContainerGetOwner(GetInsideId());
}

const ARAccessNameType& CARContainer::GetLastChanged() const
{
	return CARInside::GetInstance()->containerList.ContainerGetModifiedBy(GetInsideId());
}

const char* CARContainer::GetChangeDiary() const
{
	return CARInside::GetInstance()->containerList.ContainerGetChangeDiary(GetInsideId());
}

const ARNameType& CARContainer::GetARName() const
{
	return CARInside::GetInstance()->containerList.ContainerGetName(GetInsideId());
}

const ARPermissionList& CARContainer::GetPermissions() const
{
	return CARInside::GetInstance()->containerList.ContainerGetPermissions(GetInsideId());
}

const ARInternalIdList& CARContainer::GetSubadmins() const
{
	return CARInside::GetInstance()->containerList.ContainerGetSubadmins(GetInsideId());
}

const ARContainerOwnerObjList& CARContainer::GetOwnerObjects() const
{
	return CARInside::GetInstance()->containerList.ContainerGetOwnerObjects(GetInsideId());
}

char* CARContainer::GetLabel() const
{
	return CARInside::GetInstance()->containerList.ContainerGetLabel(GetInsideId());
}

char* CARContainer::GetDescription() const
{
	return CARInside::GetInstance()->containerList.ContainerGetDescription(GetInsideId());
}

unsigned int CARContainer::GetType() const
{
	return CARInside::GetInstance()->containerList.ContainerGetType(GetInsideId());
}

const ARReferenceList& CARContainer::GetReferences() const
{
	return CARInside::GetInstance()->containerList.ContainerGetReferences(GetInsideId());
}
