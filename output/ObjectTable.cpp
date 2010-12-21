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
#include "ObjectTable.h"

using namespace OUTPUT;

CObjectTable::CObjectTable(const string &htmlId, const string &cssClass)
: tbl(htmlId, cssClass)
{
}

CObjectTable::~CObjectTable(void)
{	
}

void CObjectTable::SetDescription(const string& desc)
{
	this->tbl.description = desc;
}

int CObjectTable::NumRows()
{
	return tbl.NumRows();
}

string CObjectTable::Print()
{
	return this->tbl.ToXHtml();
}

void CObjectTable::Print(std::ostream& strm)
{
	strm << tbl;
}

std::ostream& operator<<(std::ostream& strm, OUTPUT::CObjectTable& oTbl)
{
	oTbl.Print(strm);
	return strm;
}
