
/****************************************************************************** 
 * 
 *  file:  DocMain.h
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
