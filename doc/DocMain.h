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
#include "DocBase.h"
#include "MessageItem.h"
#include "../output/Table.h"
#include "../output/TableRow.h"
#include "../output/FilterTable.h"
#include "../output/AlTable.h"
#include "../output/EscalTable.h"
#include "../output/ContainerTable.h"
#include "../output/MenuTable.h"
#include "../output/SchemaTable.h"
#include "../output/RoleTable.h"
#include "../output/GroupTable.h"
#include "../output/UserTable.h"
#include "../output/ImageTable.h"

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
	void ActiveLinkActionDetails(int nActionType, int &ifCount, int &elseCount);
	void FilterList(string fileName, string searchChar);	
	void FilterActionList(string fileName);
	void FilterActionDetails(int nActionType, int &ifCount, int &elseCount);
	void EscalationList(string fileName, string searchChar);
	void EscalationActionList(string fileName);
	void EscalationActionDetails(int nActionType);

	void GroupList(string fileName, string searchChar);
	void UserList(string fileName, string searchChar);
	void RoleList(string fileName, string searchChar);
	void ImageList(string fileName, string searchChar);

private:
	static void Sort(list<CMessageItem> &listResult);
	static bool SortByMsgNum(const CMessageItem& t1, const CMessageItem& t2 );

	static void Sort(list<CARGlobalField> &listResult);
	static bool SortByGlobalFieldId(const CARGlobalField& t1, const CARGlobalField& t2 );

	static string ShortMenu(string curCharacter);	
};
