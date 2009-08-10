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
#include ".\aruser.h"
#include ".\argroup.h"
#include ".\arrole.h"
#include "..\appconfig.h"
#include "..\appexception.h"

class CARDataFactory
{
public:
	CARDataFactory(ARControlStruct &control, ARStatusList &list);
	~CARDataFactory(void);

	void GetListGroup(AppConfig &appConfig, list<CARGroup> &listResult);
	void GetListUser(AppConfig &appConfig, list<CARUser> &listResult);
	void GetListRoles(AppConfig &appConfig, list<CARRole> &listResult);

	void Sort(list<CARGroup> &listResult);
	void Sort(list<CARUser> &listUser);
	void Sort(list<CARRole> &listRole);
private:
	ARControlStruct *pControl;
	ARStatusList *pStatus;

	static bool SortGroupsByName(const CARGroup& ti, const CARGroup& t2);
	static bool SortUsersByName(const CARUser& ti, const CARUser &t2);
	static bool SortRolesByName(const CARRole& t1, const CARRole &t2);

	CARGroup LoadGroup(ARNameType &schemaName, string requestId);
	CARUser LoadUser(ARNameType &schemaName, string requestId, int insideId);
	CARRole LoadRole(ARNameType &schemaName, string requestId, int insideId);
};
