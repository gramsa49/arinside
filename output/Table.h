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

#pragma once
#include "WebControl.h"
#include "TableRow.h"
#include "TableColumn.h"

namespace OUTPUT
{
	class CTable :
		public CWebControl
	{
	public:
		CTable();
		CTable(const string& htmId, const string& cssClass);
		~CTable(void);

		string description;	

		string ToXHtml();
		string ToXHtmlNoHeader();
		string ToXHtml(const string& styleTag);
		string ToCsv();

		void Clear();
		void ClearRows();
		void ClearColumns();
		void SetHtmId(const string& cssClass);
		void SetCssClass(const string& htmId);
		int NumRows();
		void AddColumn(int width, const string& text);
		void AddColumn(int width, const string& text, const string& cssClass);	
		void AddRow(const CTableRow& tableRow);

	private:	
		list<CTableRow> listRows;
		list<CTableColumn> listColumns;
		string htmId;
		void GetHtmlRows(std::ostream& strm);
		void GetCsvRows(std::ostream& strm);
		void GetColumnDefinition(std::ostream& strm);
		void GetHeaderDefinition(std::ostream& strm);
		void GetCsvHeaderDefinition(std::ostream& strm);
	};
}
