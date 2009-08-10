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
#include "webcontrol.h"
#include "tablecell.h"

namespace OUTPUT
{
	class CTableRow :
		public CWebControl
	{
	public:
		CTableRow();
		CTableRow(string cssClass);
		~CTableRow(void);

		string ToXHtml();
		string ToCsv();

		string Name;
		void AddCell(string cellText);
		void AddCell(CTableCell cell);

		void AddCellList(string cellText0, string cellText1);
		void AddCellList(CTableCell cell0, CTableCell cell1);

		void AddCellList(CTableCell cell0, CTableCell cell1, CTableCell cell2);	
		void AddCellList(string cellText0, string cellText1, string cellText2);
		void ClearCells();
	private:
		string GetCells();	
		list<CTableCell> listCells;

	};
}
