
/****************************************************************************** 
 * 
 *  file:  SchemaTable.cpp
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
#include ".\schematable.h"

using namespace OUTPUT;

CSchemaTable::CSchemaTable(CARInside &arIn)
{
	this->pInside = &arIn;

	tbl = new CTable("schemaList", "TblObjectList");
	tbl->AddColumn(30, "Form Name");
	tbl->AddColumn(20, "Web Alias");
	tbl->AddColumn(5, "Fields");
	tbl->AddColumn(5, "Views");
	tbl->AddColumn(10, "Type");
	tbl->AddColumn(15, "Modified");
	tbl->AddColumn(15, "By");
}

CSchemaTable::~CSchemaTable(void)
{
}

void CSchemaTable::AddRow(CARSchema &schema, int rootLevel)
{
	CTableRow tblRow("");

	tblRow.AddCell( CTableCell(schema.GetURL(rootLevel)));
	tblRow.AddCell( CTableCell(schema.WebAlias()));
	tblRow.AddCell( CTableCell((unsigned int)schema.fieldList.size()));
	tblRow.AddCell( CTableCell((unsigned int)schema.vuiList.size()));
	tblRow.AddCell( CTableCell(CAREnum::SchemaType(schema.schema.schemaType)));
	tblRow.AddCell( CTableCell(CUtil::TimeToString(schema.timestamp)));
	tblRow.AddCell( CTableCell(this->pInside->LinkToUser(schema.lastChanged, rootLevel)));

	this->tbl->AddRow(tblRow);
}