
/****************************************************************************** 
 * 
 *  file:  TableRow.cpp
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
#include ".\tablerow.h"

using namespace OUTPUT;

CTableRow::CTableRow()
{
}

CTableRow::CTableRow(string cssClass)
{
	this->cssClass = cssClass;
}

CTableRow::~CTableRow(void)
{
	this->listCells.clear();
}

void CTableRow::ClearCells()
{
	this->listCells.clear();
}

void CTableRow::AddCell(string cellText)
{
	this->listCells.push_back(CTableCell(cellText));
}

void CTableRow::AddCell(CTableCell cell)
{
	this->listCells.push_back(cell);
}

void CTableRow::AddCellList(string cellText0, string cellText1)
{
	this->listCells.clear();
	this->listCells.push_back(CTableCell(cellText0));
	this->listCells.push_back(CTableCell(cellText1));
}

void CTableRow::AddCellList(CTableCell cell0, CTableCell cell1)
{
	this->listCells.clear();
	this->listCells.push_back(cell0);
	this->listCells.push_back(cell1);
}

void CTableRow::AddCellList(CTableCell cell0, CTableCell cell1, CTableCell cell2)
{
	this->listCells.clear();
	this->listCells.push_back(cell0);
	this->listCells.push_back(cell1);
	this->listCells.push_back(cell2);
}

void CTableRow::AddCellList(string cellText0, string cellText1, string cellText2)
{
	this->listCells.clear();
	this->listCells.push_back(CTableCell(cellText0));
	this->listCells.push_back(CTableCell(cellText1));
	this->listCells.push_back(CTableCell(cellText2));

}

string CTableRow::GetCells()
{
	stringstream strm;
	list<CTableCell>::iterator cellIter;
	for ( cellIter = listCells.begin(); cellIter != listCells.end(); cellIter++ )
	{		
		CTableCell *cell = &(*cellIter);
		strm << cell->ToXHtml() << endl;
	}
	return strm.str();
}

string CTableRow::ToXHtml()
{
	stringstream strm;	
	strm.str("");

	if(this->cssClass.length() == 0)
	{
		strm << "<tr>" << endl << this->GetCells() << "</tr>" << endl;
	}
	else
	{
		strm << "<tr class=\"" << this->cssClass << "\">" << endl << this->GetCells() << "</tr>" << endl;
	}
	return strm.str();
}

string CTableRow::ToCsv()
{
	stringstream strm;
	strm.str("");

	list<CTableCell>::iterator cellIter;
	for ( cellIter = listCells.begin(); cellIter != listCells.end(); cellIter++ )
	{		
		CTableCell *cell = &(*cellIter);
		strm << cell->ToCsv();
	}
	return strm.str();
}