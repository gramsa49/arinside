
/****************************************************************************** 
 * 
 *  file:  DocAlActionStruct.h
 * 
 *  Copyright (c) 2007, Stefan Nerlich | stefan.nerlich@hotmail.com 
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#pragma once
#include "docalhelper.h"
#include "..\core\arassignhelper.h"
#include "..\core\arqualification.h"

using namespace OUTPUT;

class CDocAlActionStruct
{
public:
	CDocAlActionStruct(CARInside &arIn, CARActiveLink &obj, string schemaName, string dir, int rootLevel);
	~CDocAlActionStruct(void);

public:
	string Get(string ifElse, ARActiveLinkActionList &actList);	

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
	string ActionMacro(ARActiveLinkMacroStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_FIELDS
	string ActionSetFields(ARSetFieldsActionStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_PROCESS
	string ActionProcess(char *action, int nAction);

	// AR_ACTIVE_LINK_ACTION_MESSAGE
	string ActionMessage(ARMessageStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_SET_CHAR
	string ActionSetChar(ARFieldCharacteristics &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_DDE
	string ActionDde(ARDDEStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_FIELDP
	string ActionPushFields(ARPushFieldsActionStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_SQL
	string ActionSql(ARSQLStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_AUTO
	string ActionAutomation(ARAutomationStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_OPENDLG
	string ActionOpenDlg(AROpenDlgStruct &action, int nAction);
	bool ActionOpenDlgQualifier(int nWindowType);

	// AR_ACTIVE_LINK_ACTION_COMMITC
	string ActionCommitChanges(ARCommitChangesStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_CLOSEWND
	string ActionCloseWindow(ARCloseWndStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_CALLGUIDE
	string ActionCallGuide(ARCallGuideStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_EXITGUIDE
	string ActionExitGuide(ARExitGuideStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_GOTOGUIDELABEL
	string ActionGotoGuideLabel(ARGotoGuideLabelStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_WAIT
	string ActionWait(ARWaitStruct &action, int nAction);

	// AR_ACTIVE_LINK_ACTION_GOTOACTION
	string ActionGotoAction(ARGotoActionStruct &action, int nAction);
};
