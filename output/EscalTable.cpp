
/****************************************************************************** 
 * 
 *  file:  EscalTable.cpp
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
#include ".\escaltable.h"

using namespace OUTPUT;

CEscalTable::CEscalTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("escalationList", "TblObjectList");
	tbl->AddColumn(30, "Escalation Name");
	tbl->AddColumn(10, "Enabled");
	tbl->AddColumn(10, "ExecuteOn");
	tbl->AddColumn(5, "If");
	tbl->AddColumn(5, "Else");
	tbl->AddColumn(20, "Changed");
	tbl->AddColumn(20, "By");
}

CEscalTable::~CEscalTable(void)
{
}

void CEscalTable::AddRow(CAREscalation &escal, int rootLevel)
{
	CTableRow tblRow("");
	tblRow.AddCell( CTableCell(escal.GetURL(rootLevel)));

	string tmpCssEnabled = "";
	if(escal.enable==0)
		tmpCssEnabled = "objStatusDisabled";
	tblRow.AddCell( CTableCell(CAREnum::ObjectEnable(escal.enable), tmpCssEnabled));
	tblRow.AddCell( CTableCell(escal.GetExecuteOn()));
	tblRow.AddCell( CTableCell(escal.actionList.numItems));
	tblRow.AddCell( CTableCell(escal.elseList.numItems));
	tblRow.AddCell( CTableCell(CUtil::TimeToString(escal.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(escal.lastChanged, rootLevel)));
	this->tbl->AddRow(tblRow);
}