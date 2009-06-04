
/****************************************************************************** 
 * 
 *  file:  GroupTable.cpp
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
#include ".\grouptable.h"

using namespace OUTPUT;

CGroupTable::CGroupTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("groupList", "TblObjectList");
	tbl->AddColumn(5, "Type");	
	tbl->AddColumn(35, "Name");
	tbl->AddColumn(10, "ID");
	tbl->AddColumn(20, "Modified");
	tbl->AddColumn(20, "By");
}

CGroupTable::~CGroupTable(void)
{
}

void CGroupTable::AddRow(string appRefName, int id, int rootLevel)
{
	if(id < 0)
	{
		this->AddRoleRow(appRefName, id, rootLevel);
	}
	else
	{
		this->AddGroupRow(appRefName, id, rootLevel);
	}
}

void CGroupTable::AddRoleRow(string appRefName, int roleId, int rootLevel)
{
	CARRole *insertRole = NULL;
	list<CARRole>::iterator listIter;		
	for ( listIter = this->pInside->roleList.begin(); listIter != this->pInside->roleList.end(); listIter++ )
	{
		CARRole *role = &(*listIter);
		if(role->roleId == roleId)
		{
			insertRole = role;
		}	
	}

	if(insertRole != NULL)
	{
		CTableRow tblRow("");
		tblRow.AddCell( CTableCell("Role"));
		tblRow.AddCell( CTableCell(insertRole->GetURL(rootLevel)));	
		tblRow.AddCell( CTableCell(insertRole->roleId));
		tblRow.AddCell( CTableCell(CUtil::TimeToString(insertRole->modified)));
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(insertRole->modifiedBy, rootLevel)));
		this->tbl->AddRow(tblRow);
	}
	else
	{
		CTableRow tblRow("");

		stringstream strmName, strmId, strmTimestamp, strmLastchanged;
		strmName << roleId << " (not loaded)";
		strmId << roleId;
		strmTimestamp << EmptyValue;
		strmLastchanged << EmptyValue;

		tblRow.AddCell( CTableCell("Role"));
		tblRow.AddCell( CTableCell(strmName.str()));	
		tblRow.AddCell( CTableCell(strmId.str()));
		tblRow.AddCell( CTableCell(strmTimestamp.str()));
		tblRow.AddCell( CTableCell(strmLastchanged.str()));        
		this->tbl->AddRow(tblRow);
	}
}

void CGroupTable::AddGroupRow(string appRefName, int groupId, int rootLevel)
{	
	CARGroup *insertGrp = NULL;
	list<CARGroup>::iterator listIter;		
	for ( listIter = this->pInside->groupList.begin(); listIter != this->pInside->groupList.end(); listIter++ )
	{
		CARGroup *grp = &(*listIter);
		if(grp->groupId == groupId)
		{
			insertGrp = grp;
		}	
	}

	if(insertGrp != NULL)
	{
		CTableRow tblRow("");
		tblRow.AddCell( CTableCell("Group"));
		tblRow.AddCell( CTableCell(this->pInside->LinkToGroup(appRefName, insertGrp->insideId, rootLevel)));	
		tblRow.AddCell( CTableCell(insertGrp->groupId));
		tblRow.AddCell( CTableCell(CUtil::TimeToString(insertGrp->modified)));
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(insertGrp->modifiedBy, rootLevel)));
		this->tbl->AddRow(tblRow);
	}
	else
	{
		CTableRow tblRow("");

		stringstream strmName, strmId, strmTimestamp, strmLastchanged;
		strmName << groupId << " (not loaded)";
		strmId << groupId;
		strmTimestamp << EmptyValue;
		strmLastchanged << EmptyValue;

		tblRow.AddCell( CTableCell("Group"));
		tblRow.AddCell( CTableCell(strmName.str()));	
		tblRow.AddCell( CTableCell(strmId.str()));
		tblRow.AddCell( CTableCell(strmTimestamp.str()));
		tblRow.AddCell( CTableCell(strmLastchanged.str()));        
		this->tbl->AddRow(tblRow);
	}	
}