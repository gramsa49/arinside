
/****************************************************************************** 
 * 
 *  file:  DocFilterActionStruct.h
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
#include "docfilterhelper.h"
#include "..\core\arassignhelper.h"
#include "..\core\arqualification.h"

using namespace OUTPUT;

class CDocFilterActionStruct
{
public:
	CDocFilterActionStruct(CARInside &arIn, CARServerObject &obj, string schemaName, string dir, int rootLevel, int structItemType);
	~CDocFilterActionStruct(void);

public:
	string Get(string ifElse, ARFilterActionList &actList);	

private:
	CARInside *arIn;
	CARServerObject *obj;
	string schemaName;
	int schemaInsideId;
	string dir;
	int rootLevel;
	int structItemType;
	string ifElse;

	//Get all matching IDs of two forms
	string AllMatchingIds(string table1, string table2, string description, int nAction);

	//AR_FILTER_ACTION_NONE
	string FilterActionNone(int nAction);

	// AR_FILTER_ACTION_NOTIFY
	string FilterActionNotify(ARFilterActionNotify &action, int nAction);

	// AR_FILTER_ACTION_MESSAGE
	string FilterActionMessage(ARFilterStatusStruct &action, int nAction);

	// AR_FILTER_ACTION_LOG
	string FilterActionLog(char *action, int nAction);

	// AR_FILTER_ACTION_FIELDS
	string FilterActionSetFields(ARSetFieldsActionStruct &action, int nAction);

	// AR_FILTER_ACTION_PROCESS
	string FilterActionProcess(char *action, int nAction);

	// AR_FILTER_ACTION_FIELDP
	string FilterActionPushFields(ARPushFieldsActionStruct &action, int nAction);

	// AR_FILTER_ACTION_SQL
	string FilterActionSql(ARSQLStruct &action, int nAction);

	// AR_FILTER_ACTION_GOTOACTION 
	string FilterActionGotoAction(ARGotoActionStruct &action, int nAction);

	// AR_FILTER_ACTION_CALLGUIDE
	string FilterActionCallGuide(ARCallGuideStruct &action, int nAction);

	// AR_FILTER_ACTION_EXITGUIDE
	string FilterActionExitGuide(ARExitGuideStruct &action, int nAction);

	// AR_FILTER_ACTION_GOTOGUIDELABEL
	string FilterActionGotoGuideLabel(ARGotoGuideLabelStruct &action, int nAction);
};
