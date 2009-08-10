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

#include "StdAfx.h"
#include "table.h"

using namespace OUTPUT;

CTable::CTable()
{
}

CTable::CTable(string htmId, string cssClass)
{
	this->htmId = htmId;
	this->cssClass = cssClass;
}

CTable::~CTable(void)
{
	this->listColumns.clear();
	this->listRows.clear();
	this->description = "";
}

int CTable::NumRows()
{
	return (int)this->listRows.size();
}

void CTable::AddColumn(int width, string text)
{
	CTableColumn col(width, text, "colStdClass");
	this->listColumns.push_back(col);
}

void CTable::AddColumn(int width, string text, string cssClass)
{
	CTableColumn col(width, text, cssClass);
	this->listColumns.push_back(col);
}

void CTable::AddRow(CTableRow tableRow)
{
	this->listRows.push_back(tableRow);
}

void CTable::SetHtmId(string htmId)
{
	this->htmId = htmId;
}

void CTable::SetCssClass(string cssClass)
{
	this->cssClass = cssClass;
}

void CTable::Clear()
{
	this->ClearColumns();
	this->ClearRows();
}

void CTable::ClearRows()
{
	this->listRows.clear();
}

void CTable::ClearColumns()
{
	this->listColumns.clear();
}

string CTable::GetHtmlRows()
{	
	stringstream strm;
	list<CTableRow>::iterator rowIter;
	for ( rowIter = listRows.begin(); rowIter != listRows.end(); rowIter++ )
	{	
		CTableRow *rowItem = &(*rowIter);
		strm << rowItem->ToXHtml() << endl;
	}
	return strm.str();
}

string CTable::GetCsvRows()
{	
	stringstream strm;
	list<CTableRow>::iterator rowIter;
	for ( rowIter = listRows.begin(); rowIter != listRows.end(); rowIter++ )
	{	
		CTableRow *rowItem = &(*rowIter);
		strm << rowItem->ToCsv() << "\n";
	}
	return strm.str();
}

string CTable::GetColumnDefinition()
{
	stringstream strm;
	strm << "<colgroup>" << endl;
	list<CTableColumn>::iterator colIter;
	CTableColumn *colItem;

	for ( colIter = listColumns.begin(); colIter != listColumns.end(); colIter++ )
	{	
		colItem = &(*colIter);
		strm << colItem->ColToXHtml() << endl;
	}

	strm << "</colgroup>" << endl;
	return strm.str();
}

string CTable::GetCsvHeaderDefinition()
{
	stringstream strm;
	strm.str("");

	list<CTableColumn>::iterator colIter;
	CTableColumn *colItem;	
	for ( colIter = listColumns.begin(); colIter != listColumns.end(); colIter++ )
	{	
		colItem = &(*colIter);
		strm << colItem->GetTitle() << "\t";
	}
	strm << "\n";
	return strm.str();
}

string CTable::GetHeaderDefinition()
{
	stringstream strm;
	strm << "<tr>" << endl;

	list<CTableColumn>::iterator colIter;
	CTableColumn *colItem;	
	for ( colIter = listColumns.begin(); colIter != listColumns.end(); colIter++ )
	{	
		colItem = &(*colIter);
		strm << colItem->HeaderCellToXHtml() << endl;
	}

	strm << "</tr>" << endl;   
	return strm.str();
}

string CTable::ToXHtml()
{
	return this->ToXHtml("");
}

string CTable::ToXHtml(string styleTag)
{
	stringstream strm;	    

	if(!this->description.empty())
	{
		strm << "<h2>" << endl;
		strm << description << endl;
		strm << "</h2>" << endl;
	}

	strm << "<table id=\"" << this->htmId << "\"";

	if(!styleTag.empty())
	{
		strm << " style=\"" << styleTag << "\"";
	}

	if(this->cssClass.length() == 0)
	{
		strm << ">" << endl;
	}
	else
	{
		strm << " class=\"" << this->cssClass << "\">" << endl;
	}

	strm << this->GetColumnDefinition();
	strm << this->GetHeaderDefinition();

	if(listRows.size() > 0)
	{
		strm << this->GetHtmlRows() << endl;	
	}
	else
	{
		size_t nColspan = listColumns.size();
		strm << "<tr>" << endl << "<td colspan=\"" << (unsigned int)nColspan << "\">" << endl;
		strm << "Table contains no data" << endl;
		strm << "</td>" << endl << "</tr>" << endl;
	}

	strm << "</table>" << endl;
	return strm.str();	
}


string CTable::ToXHtmlNoHeader()
{
	stringstream strm;
	strm.str("");

	strm << "<table id=\"" << this->htmId << "\"";

	if(this->cssClass.length() == 0)
	{
		strm << ">" << endl;
	}
	else
	{
		strm << " class=\"" << this->cssClass << "\">" << endl;
	}

	strm << this->GetColumnDefinition();
	if(listRows.size() > 0)
	{
		strm << this->GetHtmlRows() << endl;	
	}
	else
	{
		size_t nColspan = listColumns.size();
		strm << "<tr>" << endl << "<td colspan=\"" << (unsigned)nColspan << "\">" << endl;
		strm << "Table contains no data" << endl;
		strm << "</td>" << endl << "</tr>" << endl;
	}

	strm << "</table>" << endl;
	return strm.str();	
}

string CTable::ToCsv()
{
	stringstream strm;
	strm.str("");	

	strm << this->GetCsvHeaderDefinition();

	if(listRows.size() > 0)
	{
		strm << this->GetCsvRows();	
	}

	return strm.str();	
}