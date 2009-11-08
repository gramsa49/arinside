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
#include "DocImageDetails.h"

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

extern int nFilesCreated;

CDocImageDetails::CDocImageDetails(int imageIndex, int rootLevel)
{
	this->path = "image\\" + CARObject::FileID(imageIndex);
	this->pInside = CARInside::GetInstance();
	this->imageIndex = imageIndex;
	this->rootLevel = rootLevel;
}

CDocImageDetails::~CDocImageDetails(void)
{
}

void CDocImageDetails::Documentation()
{
	try
	{
		// create a directory for each image
		CWindowsUtil winUtil(pInside->appConfig);
		if(winUtil.CreateSubDirectory(this->path)<0) return;

		// now create the page
		CWebPage webPage("index", pInside->imageList.ImageGetName(imageIndex), this->rootLevel, pInside->appConfig);

		// contentHead informations
		stringstream contHeadStrm;
		contHeadStrm << CWebUtil::LinkToImageIndex(this->rootLevel) << MenuSeparator << CWebUtil::ObjName(pInside->imageList.ImageGetName(imageIndex)) << endl;
		webPage.AddContentHead(contHeadStrm.str());

		// image details
		CTable tbl("imageDetails", "TblObjectList");
		tbl.AddColumn(30, "Description");
		tbl.AddColumn(70, "Value");

		CTableRow tblRow("");
		char *desc = pInside->imageList.ImageGetDescription(imageIndex);
		tblRow.AddCellList("Description", (desc == NULL ? "" : desc));
		tbl.AddRow(tblRow);

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		// save image and insert tag into web page
		this->SaveImage();

		stringstream imgTag;
		imgTag << "<p class=\"ars_image\"><img src=\"image." << pInside->imageList.ImageGetType(imageIndex) << "\" alt=\"" << pInside->imageList.ImageGetType(imageIndex) << "\" /></p>";
		webPage.AddContent(imgTag.str());

		// add workflow references
		webPage.AddContent(WorkflowReferences());
		
		// Histoy
		// TODO: add change history details
		//webPage.AddContent(pInside->ServerObjectHistory(this->pGroup, this->rootLevel));

		webPage.SaveInFolder(this->path);	
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocImageDetails_Documentation of '" << CARInside::GetInstance()->imageList.ImageGetName(imageIndex) << "': " << e.what() << endl;
	}
}

void CDocImageDetails::SaveImage()
{
	stringstream strm;
	
	if (!pInside->appConfig.targetFolder.empty())
		strm << pInside->appConfig.targetFolder << "\\";

	if(!path.empty())
		strm << path << "\\";

	strm << "image." << pInside->imageList.ImageGetType(imageIndex);

	try
	{	
		LOG << "Save image '" << strm.str();
		const ARImageDataStruct& imgData = pInside->imageList.ImageGetData(imageIndex);

		ofstream fout( strm.str().c_str(), ios::binary);
		fout.write((const char*)imgData.bytes, imgData.numItems);
		fout.close();

		LOG << "' [OK]" << endl;

		nFilesCreated++;
	}
	catch(exception& e)
	{		
		stringstream erStrm;
		erStrm << "Error saving image '" << strm.str() << "' to disk. Error: " << e.what();
		throw(AppException(erStrm.str(), "undefined", "FileIo"));
	}
}

string CDocImageDetails::WorkflowReferences()
{
	stringstream strm;
	strm.str("");

	try
	{
		//Field references
		CTable tblRef("referenceList", "TblObjectList");
		tblRef.AddColumn(10, "Type");
		tblRef.AddColumn(45, "Server object");
		tblRef.AddColumn(5, "Enabled");
		tblRef.AddColumn(40, "Description");

		// TODO: add references here

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference:";

		tblRef.description = tblDesc.str();

		strm << tblRef.ToXHtml();
		tblRef.Clear();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating workflow references for image: " << pInside->imageList.ImageGetName(imageIndex) << " -- " << e.what() << endl;
	}	

	return strm.str();
}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
