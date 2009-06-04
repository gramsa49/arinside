
/****************************************************************************** 
 * 
 *  file:  AlTable.cpp
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
#include ".\altable.h"

using namespace OUTPUT;

CAlTable::CAlTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("alList", "TblObjectList");
	tbl->AddColumn(30, "Active Link Name");	
	tbl->AddColumn(5, "Enabled");	
	tbl->AddColumn(5, "Groups");
	tbl->AddColumn(5, "Order");	
	tbl->AddColumn(15, "Execute On");
	tbl->AddColumn(5, "If");
	tbl->AddColumn(5, "Else");
	tbl->AddColumn(15, "Changed");
	tbl->AddColumn(15, "By");
}

CAlTable::~CAlTable(void)
{	
}

void CAlTable::AddRow(CARActiveLink &al, int rootLevel)
{
	CTableRow tblRow("");
	tblRow.AddCell( CTableCell(al.GetURL(rootLevel)));
	
	string tmpCssEnabled = "";
	if(al.enable==0)
		tmpCssEnabled = "objStatusDisabled";
	tblRow.AddCell( CTableCell(CAREnum::ObjectEnable(al.enable), tmpCssEnabled));

	stringstream strmNumGroup;
	strmNumGroup << al.groupList.numItems;
	tblRow.AddCell(	CTableCell(strmNumGroup.str(), ""));
	tblRow.AddCell(	CTableCell(al.order));
	tblRow.AddCell(	CTableCell(al.GetExecuteOnEx()));
	tblRow.AddCell(	CTableCell(al.actionList.numItems));
	tblRow.AddCell(	CTableCell(al.elseList.numItems));
	tblRow.AddCell(	CTableCell(CUtil::TimeToString(al.timestamp)));
	tblRow.AddCell(	CTableCell(this->pInside->LinkToUser(al.lastChanged, rootLevel)));
	this->tbl->AddRow(tblRow);


}