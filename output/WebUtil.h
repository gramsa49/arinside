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

namespace OUTPUT
{
	class CWebUtil
	{
	public:
		CWebUtil(void);
		~CWebUtil(void);

		static string RootPath(int level);	
		static string Validate(string text);
		static string Replace(string text);	
		static string Link(string caption, string linkTo, string imgName, int rootLevel);
		static string Link(string caption, string linkTo, string imgName, int rootLevel, bool validate);
		static string WebPageSuffix();
		static string CsvPageSuffix();
		static string ImageTag(string imgName, int rootLevel);
		static string DocName(string fName);
		static string CsvDocName(string fName);
		static string ObjName(string objName);
		static string ChkBoxInput(string nameAndValue, bool checked);

		static string LinkToActiveLinkIndex(int rootLevel);
		static string LinkToActiveLinkIndex(int objectCount, int rootLevel);
		static string LinkToActiveLinkGuideIndex(int rootLevel);
		static string LinkToActiveLinkGuideIndex(int objectCount, int rootLevel);
		static string LinkToApplicationIndex(int rootLevel);
		static string LinkToApplicationIndex(int objectCount, int rootLevel);
		static string LinkToEscalationIndex(int rootLevel);
		static string LinkToEscalationIndex(int objectCount, int rootLevel);
		static string LinkToFilterIndex(int rootLevel);
		static string LinkToFilterIndex(int objectCount, int rootLevel);
		static string LinkToFilterGuideIndex(int rootLevel);
		static string LinkToFilterGuideIndex(int objectCount, int rootLevel);
		static string LinkToGroupIndex(int rootLevel);
		static string LinkToGroupIndex(int objectCount, int rootLevel);
		static string LinkToImageIndex(int rootLevel);
		static string LinkToImageIndex(int objectCount, int rootLevel);
		static string LinkToMenuIndex(int rootLevel);
		static string LinkToMenuIndex(int objectCount, int rootLevel);
		static string LinkToPackingListIndex(int rootLevel);
		static string LinkToPackingListIndex(int objectCount, int rootLevel);
		static string LinkToRoleIndex(int rootLevel);
		static string LinkToRoleIndex(int objectCount, int rootLevel);
		static string LinkToSchemaIndex(int rootLevel);
		static string LinkToSchemaIndex(int objectCount, int rootLevel);
		static string LinkToUserIndex(int rootLevel);
		static string LinkToUserIndex(int objectCount, int rootLevel);
		static string LinkToWebServiceIndex(int rootLevel);
		static string LinkToWebServiceIndex(int objectCount, int rootLevel);	
		static string LinkToContainer(int objectCount, int rootLevel, int containerType);

		static string LinkToHelper(string name, int objectCount, string folderName, string image, int rootLevel);
	};
}
