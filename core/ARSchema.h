
/****************************************************************************** 
 * 
 *  file:  ARSchema.h
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
#include ".\arserverobject.h"
#include ".\arfield.h"
#include ".\arvui.h"

class CARSchema :
	public CARServerObject
{
public:
	CARSchema();
	CARSchema(string name, int insideId);
public:
	~CARSchema(void);

	string alias;
	ARCompoundSchema  schema;
	ARPermissionList	groupList;
	ARInternalIdList	admingrpList;
	AREntryListFieldList getListFields;
	ARSortList	sortList;
	ARIndexList	indexList;
	ARArchiveInfoStruct	archiveInfo;
	ARAuditInfoStruct auditInfo;
	ARNameType defaultVui;
	ARPropList objPropList;
	string WebAlias();
        
	list<CARField> fieldList;
	list<CARVui> vuiList;

	string GetURL(int rootLevel);

	string LinkToVui(int vuiId, int fromRootLevel);
	string LinkToVui(string vuiName, int fromRootLevel);
	string VuiGetLabel(int vuiId);
};
