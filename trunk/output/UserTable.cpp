
/****************************************************************************** 
 * 
 *  file:  UserTable.cpp
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
#include ".\usertable.h"

using namespace OUTPUT;

CUserTable::CUserTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("userList", "TblObjectList");
	tbl->AddColumn(40, "Login Name");
	tbl->AddColumn(20, "License");
	tbl->AddColumn(20, "Modified");
	tbl->AddColumn(20, "By");
}

CUserTable::~CUserTable(void)
{
}

void CUserTable::AddRow(CARUser &user, int rootLevel)
{	
	try
	{
		CTableRow tblRow("");
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(user.loginName, rootLevel)));
		tblRow.AddCell( CTableCell(CAREnum::UserGetLicType(user.licenseType)));
		tblRow.AddCell( CTableCell(CUtil::TimeToString(user.modified)));
		tblRow.AddCell( CTableCell(this->pInside->LinkToUser(user.modifiedBy, rootLevel)));

		this->tbl->AddRow(tblRow);
	}
	catch(...)
	{
		cout << "EXCEPTION adding user " << user.loginName << endl;
	}
}