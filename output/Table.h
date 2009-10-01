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
		CTable(string htmId, string cssClass);
		~CTable(void);

		string description;	

		string ToXHtml();
		string ToXHtmlNoHeader();
		string ToXHtml(string styleTag);
		string ToCsv();

		void Clear();
		void ClearRows();
		void ClearColumns();
		void SetHtmId(string cssClass);
		void SetCssClass(string htmId);
		int NumRows();
		void AddColumn(int width, string text);
		void AddColumn(int width, string text, string cssClass);	
		void AddRow(CTableRow tableRow);

	private:	
		list<CTableRow> listRows;
		list<CTableColumn> listColumns;
		string htmId;
		string GetHtmlRows();
		string GetCsvRows();
		string GetColumnDefinition();
		string GetHeaderDefinition();
		string GetCsvHeaderDefinition();
	};
}
