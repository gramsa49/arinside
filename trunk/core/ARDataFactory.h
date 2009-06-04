
/****************************************************************************** 
 * 
 *  file:  ARDataFactory.h
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
