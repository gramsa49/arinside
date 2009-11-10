//Copyright (C) 2009 Stefan Nerlich | stefan.nerlich@hotmail.com
//
//This file is part of ARInside.
//
//    ARInside is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    ARInside is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "TableRow.h"

using namespace OUTPUT;

CTableRow::CTableRow()
{
}

CTableRow::CTableRow(const string &cssClass)
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
	list<CTableCell>::iterator endIter = listCells.end();
	for ( cellIter = listCells.begin(); cellIter != endIter; ++cellIter )
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

	if(this->cssClass.empty())
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
