
/****************************************************************************** 
 * 
 *  file:  RoleTable.cpp
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

#include "StdAfx.h"
#include ".\roletable.h"

using namespace OUTPUT;

CRoleTable::CRoleTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("roleList", "TblObjectList");
	tbl->AddColumn(25, "Role Name");
	tbl->AddColumn(5, "RoleID");
	tbl->AddColumn(30, "Application");	
	tbl->AddColumn(20, "Modified");
	tbl->AddColumn(20, "By");
}

CRoleTable::~CRoleTable(void)
{
}

void CRoleTable::AddRow(CARRole &role, int rootLevel)
{
	CTableRow tblRow("");
	tblRow.AddCell( CTableCell(role.GetURL(rootLevel)));
	tblRow.AddCell( CTableCell(role.roleId));
	tblRow.AddCell( CTableCell(this->pInside->LinkToContainer(role.applicationName, rootLevel)));
	tblRow.AddCell( CTableCell(CUtil::TimeToString(role.modified)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(role.modifiedBy, rootLevel)));
	this->tbl->AddRow(tblRow);
}