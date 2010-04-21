//Copyright (C) 2009 John Luthgers | jls17
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
#include "ImageTable.h"

using namespace OUTPUT;

CImageTable::CImageTable(CARInside &arIn)
: CObjectTable("imageList", "TblObjectList")
{
	this->pInside = &arIn;

	tbl.AddColumn(40, "Image Name");
	tbl.AddColumn(20, "Type");
	tbl.AddColumn(20, "Modified");
	tbl.AddColumn(20, "By");
}

CImageTable::~CImageTable(void)
{
}

void CImageTable::AddRow(int imageIndex, int rootLevel)
{	
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	try
	{
		CTableRow tblRow("");
		tblRow.AddCell(this->pInside->LinkToImage(imageIndex, rootLevel));
		tblRow.AddCell(this->pInside->imageList.ImageGetType(imageIndex));
		tblRow.AddCell(CUtil::DateTimeToHTMLString(this->pInside->imageList.ImageGetTimestamp(imageIndex)));
		tblRow.AddCell(this->pInside->imageList.ImageGetModifiedBy(imageIndex));

		this->tbl.AddRow(tblRow);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION adding image: " << e.what()<< endl;
	}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
}
