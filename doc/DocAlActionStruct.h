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
#include "DocActionSetFieldsHelper.h"
#include "../core/ARAssignHelper.h"
#include "../core/ARQualification.h"

using namespace OUTPUT;

class CDocAlActionStruct
{
public:
	CDocAlActionStruct(CARInside &arIn, CARActiveLink &obj, string schemaName, int rootLevel);
	~CDocAlActionStruct(void);

public:
	string Get(const string &ifElse, const ARActiveLinkActionList &actList);	

private:
	CARInside *arIn;
	CARActiveLink *obj;
	string schemaName;
	int schemaInsideId;
	string dir;
	int rootLevel;
	int structItemType;
	string ifElse;

	//Get all matching IDs of two forms
	string AllMatchingIds(string table1, string table2, string description, int nAction);

	// AR_ACTIVE_LINK_ACTION_NONE
	string ActionNone(int nAction);

	// AR_ACTIVE_LINK_ACTION_MACRO
	string ActionMacro(const ARActiveLinkMacroStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_FIELDS
	string ActionSetFields(const ARSetFieldsActionStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_PROCESS
	string ActionProcess(const char *action, int nAction);

	// AR_ACTIVE_LINK_ACTION_MESSAGE
	string ActionMessage(const ARMessageStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_SET_CHAR
	string ActionSetChar(const ARFieldCharacteristics &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_DDE
	string ActionDde(const ARDDEStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_FIELDP
	string ActionPushFields(const ARPushFieldsActionStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_SQL
	string ActionSql(const ARSQLStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_AUTO
	string ActionAutomation(const ARAutomationStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_OPENDLG
	string ActionOpenDlg(const AROpenDlgStruct &action, int nAction);
	bool ActionOpenDlgQualifier(int nWindowType);

	// AR_ACTIVE_LINK_ACTION_COMMITC
	string ActionCommitChanges(const ARCommitChangesStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_CLOSEWND
	string ActionCloseWindow(const ARCloseWndStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_CALLGUIDE
	string ActionCallGuide(const ARCallGuideStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_EXITGUIDE
	string ActionExitGuide(const ARExitGuideStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL
	string ActionGotoGuideLabel(const ARGotoGuideLabelStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_WAIT
	string ActionWait(const ARWaitStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_GOTOACTION
	string ActionGotoAction(const ARGotoActionStruct &action, int nAction);

#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
	// AR_ACTIVE_LINK_ACTION_SERVICE
	string ActionService(const ARActiveLinkSvcActionStruct &action, int nAction);
#endif
};
