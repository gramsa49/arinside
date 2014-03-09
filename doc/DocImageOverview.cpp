//Copyright (C) 2014 John Luthgers | jls17
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
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "DocImageOverview.h"
#include "DocMain.h"
#include "../ARInside.h"
#include "../output/ImageTable.h"
#include "../output/LetterFilterControl.h"

CDocImageOverview::CDocImageOverview(const std::string &startChar, std::vector<int> &countPerLetter)
: searchChar(startChar), objCountPerLetter(countPerLetter)
{
}

CDocImageOverview::~CDocImageOverview(void)
{
}

unsigned int CDocImageOverview::Build()
{
	unsigned int objCount = 0;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	CARInside *pInside = CARInside::GetInstance();

	// server version older than 7.5 ?? then there are no files to generate
	if (pInside->CompareServerVersion(7,5) < 0) return 0;
	if (searchChar.size() != 1) return 0;

	unsigned int page = (unsigned int)searchChar[0];
	CPageParams file(page, AR_STRUCT_ITEM_XML_IMAGE);
	LetterFilterControl letterFilter;

	try
	{
		rootLevel = file->GetRootLevel();
		CImageTable imgTable(*pInside);

		unsigned int len = pInside->imageList.GetCount();
		for (unsigned int idx = 0; idx < len; ++idx)
		{
			CARImage img(idx);
			bool bInsert = false;

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (pInside->appConfig.bOverlaySupport && !IsVisibleObject(img))
				continue;
#endif

			if (searchChar == "*") // All objects
			{
				letterFilter.IncStartLetterOf(img);
				// the first call to this function holds always "*" as search char. That's the
				// best time to sum up the object count per letter.
				string firstChar = img.GetNameFirstChar();
				if (firstChar.empty()) firstChar = "*";
				int index = CARObject::GetFirstCharIndex(firstChar[0]);
				++(objCountPerLetter[index]);
				bInsert = true;
			}
			else if (searchChar == "#")
			{
				if (!img.NameStandardFirstChar())
					bInsert = true;
			}
			else 
			{
				if (img.GetNameFirstChar() == searchChar)
					bInsert = true;
			}

			if (bInsert)
			{
				imgTable.AddRowJson(idx, rootLevel);
				objCount++;
			}
		}

		if (imgTable.NumRows() > 0 || searchChar == "*")
		{
			CWebPage webPage(file->GetFileName(), "Image List", rootLevel, pInside->appConfig);

			stringstream strmTmp;
			strmTmp << "<span id='imageListFilterResultCount'></span>" << CWebUtil::LinkToImageIndex(objCount, rootLevel);

			if (searchChar == "*")
			{
				webPage.GetReferenceManager()
					.AddScriptReference("img/object_list.js")
					.AddScriptReference("img/imageList.js")
					.AddScriptReference("img/jquery.timers.js")
					.AddScriptReference("img/jquery.address.min.js");

				strmTmp << CDocMain::CreateImageFilterControl() << endl;
			}
			if (objCount > 0)
			{
				imgTable.RemoveEmptyMessageRow();
			}

			strmTmp << letterFilter;
			strmTmp << imgTable;
			
			webPage.AddContent(strmTmp.str());
			webPage.SaveInFolder(file->GetPath());
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ImageList: " << e.what() << endl;
	}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	return objCount;
}
