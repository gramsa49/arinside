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
{
	this->pInside = &arIn;

	tbl = new CTable("imageList", "TblObjectList");
	tbl->AddColumn(40, "Image Name");
	tbl->AddColumn(20, "Type");
	tbl->AddColumn(20, "Modified");
	tbl->AddColumn(20, "By");
}

CImageTable::~CImageTable(void)
{
}

void CImageTable::AddRow(int imageIndex, int rootLevel)
{	
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	try
	{
		// TODO: add rows to table
		CTableRow tblRow("");
		tblRow.AddCell(this->pInside->LinkToImage(imageIndex, rootLevel));
		tblRow.AddCell(this->pInside->imageList.ImageGetType(imageIndex));
		tblRow.AddCell(CUtil::DateTimeToHTMLString(this->pInside->imageList.ImageGetTimestamp(imageIndex)));
		tblRow.AddCell(this->pInside->imageList.ImageGetModifiedBy(imageIndex));
		//tblRow.AddCell( CTableCell(this->pInside->LinkToUser(user.loginName, rootLevel)));
		//tblRow.AddCell( CTableCell(CAREnum::UserGetLicType(user.licenseType)));
		//tblRow.AddCell( CTableCell(CUtil::DateTimeToHTMLString(user.modified)));
		//tblRow.AddCell( CTableCell(this->pInside->LinkToUser(user.modifiedBy, rootLevel)));

		this->tbl->AddRow(tblRow);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION adding image: " << e.what()<< endl;
	}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
}
