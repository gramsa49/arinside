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

CARContainer::CARContainer(string name, int insideId)
: CARServerObjectWithData(insideId)
{
	this->name = name;
	this->type = 0;
	this->label = NULL;
	this->description = NULL;

	memset(&groupList,0,sizeof(ARPermissionList));
	memset(&admingrpList,0,sizeof(ARInternalIdList));
	memset(&ownerObjList,0,sizeof(ARContainerOwnerObjList));
	memset(&references,0,sizeof(ARReferenceList));
	memset(&objPropList,0,sizeof(ARPropList));
}

CARContainer::~CARContainer(void)
{
	try
	{
		FreeARReferenceList(&references, false);
		FreeARPermissionList(&groupList, false);
		FreeARInternalIdList(&admingrpList, false);
		FreeARContainerOwnerObjList(&ownerObjList, false);
		FreeARReferenceList(&references, false);

		if(objPropList.props != NULL)
			FreeARPropList(&objPropList, false);
	}
	catch(...)
	{
	}
}

string CARContainer::GetURL(int rootLevel, bool showImage)
{
	string tmp = CWebUtil::RootPath(rootLevel)+CAREnum::ContainerDir(this->type)+"/"+this->FileID()+"/"+CWebUtil::DocName("index");
	return CWebUtil::Link(this->name, tmp, (showImage ? CAREnum::ContainerImage(this->type) : ""), rootLevel);
}
