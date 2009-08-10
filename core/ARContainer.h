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
#include ".\arserverobject.h"
#include "..\core\arenum.h"

class CARContainer :
	public CARServerObject
{
public:
	CARContainer(string name, int insideId);
	~CARContainer(void);

	ARPermissionList	groupList;
	ARInternalIdList	admingrpList;
	ARContainerOwnerObjList	ownerObjList;
	char	*label;
	char	*description;
	unsigned int	type;
	ARReferenceList	references;
	ARPropList	objPropList;

	string GetURL(int rootLevel);
};
