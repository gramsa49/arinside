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
#include ".\docbase.h"
#include ".\messageitem.h"
#include "..\output\table.h"
#include "..\output\tablerow.h"
#include "..\output\filtertable.h"
#include "..\output\altable.h"
#include "..\output\escaltable.h"
#include "..\output\containertable.h"
#include "..\output\menutable.h"
#include "..\output\schematable.h"
#include "..\output\roletable.h"
#include "..\output\grouptable.h"
#include "..\output\usertable.h"

using namespace OUTPUT;

class CDocMain :
	public CDocBase
{
public:
	CDocMain(CARInside &arIn);
	~CDocMain(void);

	int Index();
	void ServerInfoList();	
	void SchemaList(int nType, string fileName, string title, string searchChar);	
	void CharMenuList(string fileName, string searchChar);
	void ContainerList(int nType, string fileName, string title, string searchChar);
	void GlobalFieldList();
	void MessageList();

	void ActiveLinkList(string fileName, string searchChar);	
	void ActiveLinkActionList(string fileName);	
	void ActiveLinkActionDetails(int nActionType);
	void FilterList(string fileName, string searchChar);	
	void FilterActionList(string fileName);
	void FilterActionDetails(int nActionType);
	void EscalationList(string fileName, string searchChar);
	void EscalationActionList(string fileName);
	void EscalationActionDetails(int nActionType);

	void GroupList(string fileName, string searchChar);
	void UserList(string fileName, string searchChar);
	void RoleList(string fileName, string searchChar);

private:
	static void Sort(list<CMessageItem> &listResult);
	static bool SortByMsgNum(const CMessageItem& t1, const CMessageItem& t2 );

	static void Sort(list<CARGlobalField> &listResult);
	static bool SortByGlobalFieldId(const CARGlobalField& t1, const CARGlobalField& t2 );

	static string ShortMenu(string curCharacter);	
};
