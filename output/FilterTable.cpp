
/****************************************************************************** 
 * 
 *  file:  FilterTable.cpp
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
#include ".\filtertable.h"

using namespace OUTPUT;

CFilterTable::CFilterTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("filterList", "TblObjectList");
	tbl->AddColumn(30, "Filter Name");
	tbl->AddColumn(5, "Enabled");
	tbl->AddColumn(5, "Order");
	tbl->AddColumn(20, "Execute On");
	tbl->AddColumn(5, "If");
	tbl->AddColumn(5, "Else");
	tbl->AddColumn(15, "Changed");
	tbl->AddColumn(15, "By");
}

CFilterTable::~CFilterTable(void)
{
}

void CFilterTable::AddRow(CARFilter &filter, int rootLevel)
{
	CTableRow tblRow("");

	tblRow.AddCell( CTableCell(filter.GetURL(rootLevel)));
	
	string tmpCssEnabled = "";
	if(filter.enable==0)
		tmpCssEnabled = "objStatusDisabled";
	tblRow.AddCell( CTableCell(CAREnum::ObjectEnable(filter.enable), tmpCssEnabled));

	tblRow.AddCell( CTableCell(filter.order));
	tblRow.AddCell( CTableCell(filter.GetExecuteOnEx()));
	tblRow.AddCell( CTableCell(filter.actionList.numItems));
	tblRow.AddCell( CTableCell(filter.elseList.numItems));
	tblRow.AddCell( CTableCell(CUtil::TimeToString(filter.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(filter.lastChanged, rootLevel)));
	
	this->tbl->AddRow(tblRow);
}