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
#include "../core/ARImage.h"
#include "DocOverlayHelper.h"

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750

extern int nFilesCreated;

CDocImageDetails::CDocImageDetails(int imageIndex)
{
	this->imageIndex = imageIndex;
}

CDocImageDetails::~CDocImageDetails(void)
{
}

void CDocImageDetails::Documentation()
{
	CARImage image(imageIndex);
	CPageParams file(PAGE_DETAILS, &image);

	try
	{
		rootLevel = file->GetRootLevel();

		// create a directory for each image
		CWindowsUtil winUtil(pInside->appConfig);
		if(winUtil.CreateSubDirectory(file->GetPath())<0) return;

		// now create the page
		CWebPage webPage(file->GetFileName(), image.GetName(), rootLevel, pInside->appConfig);
		CDocOverlayHelper overlayHelper(image, rootLevel);

		// contentHead informations
		stringstream contHeadStrm;
		int overlayType = image.GetOverlayType();
		contHeadStrm << CWebUtil::LinkToImageIndex(rootLevel) << MenuSeparator << CWebUtil::ObjName(image.GetName()) << CAREnum::GetOverlayTypeString(overlayType) << endl;

		webPage.AddContentHead(contHeadStrm.str(), overlayHelper.PlaceOverlayLink());
		webPage.AddContent(overlayHelper.PlaceOverlaidNotice());

		// image details
		CTable tbl("imageDetails", "TblObjectList");
		tbl.AddColumn(30, "Description");
		tbl.AddColumn(70, "Value");

		CTableRow tblRow("");
		char *desc = image.GetDescription();
		tblRow.AddCellList("Description", (desc == NULL ? "" : desc));
		tbl.AddRow(tblRow);

		webPage.AddContent(tbl.ToXHtml());
		tbl.Clear();

		// save image and insert tag into web page
		this->SaveImage();

		CPageParams imageLink(PAGE_IMAGE_DATA, &image);

		stringstream imgTag;
		imgTag << "<p class=\"ars_image\"><img src=\"" << CWebUtil::RootPath(rootLevel) << imageLink->GetFullFileName() <<  "\" alt=\"" << image.GetType() << "\" /></p>";
		webPage.AddContent(imgTag.str());

		// add workflow references
		webPage.AddContent(WorkflowReferences());
		
		// Histoy
		webPage.AddContent(pInside->ServerObjectHistory(&image, rootLevel));

		webPage.SaveInFolder(file->GetPath());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CDocImageDetails_Documentation of '" << image.GetName() << "': " << e.what() << endl;
	}
}

void CDocImageDetails::SaveImage()
{
	stringstream strm;
	
	if (!pInside->appConfig.targetFolder.empty())
		strm << pInside->appConfig.targetFolder << "/";

	CARImage image(imageIndex);
	CPageParams file(PAGE_IMAGE_DATA, &image);

	strm << file->GetFullFileName();

	try
	{	
		LOG << "Save image '" << strm.str();
		const ARImageDataStruct& imgData = image.GetData();

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

		CARImage image(this->imageIndex);
		const CARImage::ReferenceList& refs = image.GetReferences();
		CARImage::ReferenceList::const_iterator curIt = refs.begin();
		CARImage::ReferenceList::const_iterator endIt = refs.end();
		for ( ; curIt != endIt; ++curIt)
		{			
			CTableRow row("cssStdRow");		
			row.AddCell(CAREnum::XmlStructItem(curIt->GetObjectType()));				
			row.AddCell(pInside->LinkToObjByRefItem(*curIt, rootLevel));

			string tmpEnabled = "";
			string tmpCssEnabled = "";

			bool enabledSupported = false;
			int enabled = curIt->GetObjectEnabled(enabledSupported);

			if (enabledSupported)
			{
				tmpEnabled = CAREnum::ObjectEnable(enabled);
				if (!enabled) { tmpCssEnabled = "objStatusDisabled"; }
			}

			row.AddCell(CTableCell(tmpEnabled, tmpCssEnabled));
			row.AddCell(curIt->GetDescription(rootLevel));
			tblRef.AddRow(row);
		}

		stringstream tblDesc;
		tblDesc << CWebUtil::ImageTag("doc.gif", rootLevel) << "Workflow Reference:";

		tblRef.description = tblDesc.str();

		strm << tblRef;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION enumerating workflow references for image: " << pInside->imageList.ImageGetName(imageIndex) << " -- " << e.what() << endl;
	}	

	return strm.str();
}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
