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
#include "WebUtil.h"
#include "../ARInside.h"

using namespace OUTPUT;

string CWebUtil::webpageFileExtension;

CWebUtil::CWebUtil(void)
{
}

CWebUtil::~CWebUtil(void)
{
}

string CWebUtil::RootPath(int level)
{
	switch(level)
	{
	case 0:
	default: return "./";
	case 1: return "../";
	case 2: return "../../";
	case 3: return "../../../";
	}
}

string CWebUtil::Validate(string text)
{
	text = CUtil::StrReplace("&", "&amp;", text);
	text = CUtil::StrReplace("\"", "&quot;", text);
	text = CUtil::StrReplace("<", "&lt;", text);
	text = CUtil::StrReplace(">", "&gt;", text);
	return text;
}

string CWebUtil::HtmlPageSuffix()
{
	return "htm";
}

string CWebUtil::HtmlGZPageSuffix()
{
	return "htm.gz";
}

string CWebUtil::WebPageSuffix()
{
	return webpageFileExtension;
}

string CWebUtil::CsvPageSuffix()
{
	return "txt";
}

string CWebUtil::DocName(string fName)
{
	return fName+"."+WebPageSuffix();
}

string CWebUtil::CsvDocName(string fName)
{
	return fName+"."+CsvPageSuffix();
}

string CWebUtil::ImageTag(const string& imgName, int rootLevel)
{
	stringstream strm;

	int width  = 16;
	int height = 16;

	if(imgName == "doc.gif")        { width =15 ; height =10 ; }
	if(imgName == "down.gif")       { width =14 ; height =10 ; }
	if(imgName == "folder.gif")     { width =16 ; height =13 ; }
	if(imgName == "chapter.gif")    { width =15 ; height =13 ; }
	if(imgName == "next.gif")       { width =10 ; height =10 ; }
	if(imgName == "prev.gif")       { width =10 ; height =10 ; }
	if(imgName == "up.gif")         { width =14 ; height =10 ; }
	if(imgName == "visible.gif")    { width =18 ; height =18 ; }
	if(imgName == "edit.gif")       { width =18 ; height =18 ; }	
	if(imgName == "hidden.gif")     { width =18 ; height =18 ; }

	strm << "<img src=\"" << CWebUtil::RootPath(rootLevel) << "img/" << imgName << "\" width=\"" << width << "\" height=\"" << height << "\" alt=\"" << imgName << "\">";

	return strm.str();
}

string CWebUtil::ObjName(const string& objName)
{
	stringstream strm;
	strm << "<span class=\"objName\">" << objName << "</span>" << endl;
	return strm.str();
}


//targetMode
//_blank, to open the link in a new window
//_self, to open the link in the current window
//_parent, to cleanup only the current frameset
//_top, to cleanup all layered framesets
string CWebUtil::Link(const string& caption, const string& linkTo, const string& imgName, int rootLevel)
{
	return Link(caption, linkTo, imgName, rootLevel, true);
}

// The following function should be removed completly. Use: "Link(const string&, CPageParams&, const string&, int)" instead.
string CWebUtil::Link(const string& caption, const string& linkTo, const string& imgName, int rootLevel, bool validate)
{
	stringstream strm;
	strm.str("");

	string xhtmlCaption = (validate ? Validate(caption) : caption);

	if(!imgName.empty())
		strm << ImageTag(imgName, rootLevel);

	strm << "<a href=\"" << linkTo << "\">" << xhtmlCaption << "</a>";
	return strm.str();
}

string CWebUtil::LinkToActiveLinkIndex(int rootLevel)
{
	return LinkToActiveLinkIndex(-1, rootLevel);
}

string CWebUtil::LinkToActiveLinkIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1)?"Active Links":"Active Link";
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK), "active_link.gif", rootLevel);
}

string CWebUtil::LinkToActiveLinkGuideIndex(int rootLevel)
{
	return LinkToActiveLinkGuideIndex(-1, rootLevel);
}

string CWebUtil::LinkToActiveLinkGuideIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Active Link Guides":"Active Link Guide");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_GUIDE), "al_guide.gif", rootLevel);
}

string CWebUtil::LinkToApplicationIndex(int rootLevel)
{
	return LinkToApplicationIndex(-1, rootLevel);
}

string CWebUtil::LinkToApplicationIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Applications":"Application");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_APP), "application.gif", rootLevel);
}

string CWebUtil::LinkToEscalationIndex(int rootLevel)
{
	return LinkToEscalationIndex(-1, rootLevel);
}

string CWebUtil::LinkToEscalationIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Escalations":"Escalation");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION), "escalation.gif", rootLevel);

}

string CWebUtil::LinkToFilterIndex(int rootLevel)
{
	return LinkToFilterIndex(-1, rootLevel);
}

string CWebUtil::LinkToFilterIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Filters":"Filter");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER), "filter.gif", rootLevel);	// TODO: retrieve folder name from IFileStructure later
}

string CWebUtil::LinkToFilterGuideIndex(int rootLevel)
{
	return LinkToFilterGuideIndex(-1, rootLevel);
}

string CWebUtil::LinkToFilterGuideIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Filter Guides":"Filter Guide");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_FILTER_GUIDE), "filter_guide.gif", rootLevel);
}

string CWebUtil::LinkToGroupIndex(int rootLevel)
{
	return LinkToGroupIndex(-1, rootLevel);
}

string CWebUtil::LinkToGroupIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1 ? "Groups" : "Group");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, DATA_TYPE_GROUP), "group.gif", rootLevel);
}

string CWebUtil::LinkToMenuIndex(int rootLevel)
{
	return LinkToMenuIndex(-1, rootLevel);
}

string CWebUtil::LinkToMenuIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Menus":"Menu");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CHAR_MENU), "menu.gif", rootLevel);
}

string CWebUtil::LinkToPackingListIndex(int rootLevel)
{
	return LinkToPackingListIndex(-1, rootLevel);
}

string CWebUtil::LinkToPackingListIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Packing Lists":"Packing List");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_PACK), "packing_list.gif", rootLevel);
}

string CWebUtil::LinkToRoleIndex(int rootLevel)
{
	return LinkToRoleIndex(-1, rootLevel);
}

string CWebUtil::LinkToRoleIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Roles":"Role");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, DATA_TYPE_ROLE), "doc.gif", rootLevel);
}

string CWebUtil::LinkToSchemaIndex(int rootLevel)
{
	return LinkToSchemaIndex(-1, rootLevel);
}

string CWebUtil::LinkToSchemaIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Forms":"Form");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_SCHEMA), "schema.gif", rootLevel);
}

string CWebUtil::LinkToUserIndex(int rootLevel)
{
	return LinkToUserIndex(-1, rootLevel);
}

string CWebUtil::LinkToUserIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1?"Users":"User");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, DATA_TYPE_USER), "user.gif", rootLevel);
}

string CWebUtil::LinkToWebServiceIndex(int rootLevel)
{
	return LinkToWebServiceIndex(-1, rootLevel);
}

string CWebUtil::LinkToWebServiceIndex(int objectCount, int rootLevel)	
{
	string name = (objectCount > 1?"Web Services":"Web Service");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_WEBSERVICE), "webservice.gif", rootLevel);
}

string CWebUtil::LinkToContainer(int objectCount, int rootLevel, int containerType)
{
	switch(containerType)
	{
	case ARCON_GUIDE: return LinkToActiveLinkGuideIndex(objectCount, rootLevel);
	case ARCON_APP: return LinkToApplicationIndex(objectCount, rootLevel);
	case ARCON_PACK: return LinkToPackingListIndex(objectCount, rootLevel);
	case ARCON_FILTER_GUIDE: return LinkToFilterGuideIndex(objectCount, rootLevel);
	case ARCON_WEBSERVICE: return LinkToWebServiceIndex(objectCount, rootLevel);
	default: return "";
	}
}

string CWebUtil::LinkToHelper(string name, int objectCount, const CPageParams& page, string image, int rootLevel)
{
	stringstream strmTmp;
	strmTmp.str("");

	if(objectCount > -1) strmTmp << objectCount << " ";

	strmTmp << CWebUtil::Link(name, RootPath(rootLevel) + page->GetFullFileName(), image, rootLevel);
	return strmTmp.str();
}

string CWebUtil::ChkBoxInput(std::string nameAndValue, bool checked)
{
	stringstream strmTmp; strmTmp.str("");

	strmTmp << "<input type=\"checkbox\" name=\"" << nameAndValue << "\" value=\"" << nameAndValue << "\"";
	if (checked) strmTmp << " checked";
	strmTmp << " />";
	return strmTmp.str();
}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
string CWebUtil::LinkToImageIndex(int rootLevel)
{
	return LinkToImageIndex(-1, rootLevel);
}

string CWebUtil::LinkToImageIndex(int objectCount, int rootLevel)
{
	string name = (objectCount > 1? "Images" : "Image");
	return LinkToHelper(name, objectCount, CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_IMAGE), "image.gif", rootLevel);
}
#endif

string CWebUtil::Link(const string& caption, const CPageParams &linkToPage, const string& imgName, int rootLevel)
{
	return Link(caption, linkToPage, imgName, rootLevel, true, TARGET_MODE_SELF);
}

string CWebUtil::Link(const string& caption, const CPageParams &linkToPage, const string& imgName, int rootLevel, bool validate)
{
	return Link(caption, linkToPage, imgName, rootLevel, validate, TARGET_MODE_SELF);
}

string CWebUtil::Link(const string& caption, const CPageParams &linkToPage, const string& imgName, int rootLevel, LinkTargetMode target)
{
	return Link(caption, linkToPage, imgName, rootLevel, true, target);
}

string CWebUtil::Link(const string& caption, const CPageParams &linkToPage, const string& imgName, int rootLevel, bool validate, LinkTargetMode target)
{
	stringstream strmTmp;

	string xhtmlCaption = (validate ? Validate(caption) : caption);

	if (!imgName.empty()) 
		strmTmp << ImageTag(imgName,rootLevel);
	
	if (!linkToPage.Valid())
		strmTmp << xhtmlCaption;
	else
	{
		strmTmp << "<a href=\"" << RootPath(rootLevel) + linkToPage->GetFullFileName() << "\"";
		if (target > TARGET_MODE_SELF)
		{
			strmTmp << " target=\"";
			switch (target)
			{
				case TARGET_MODE_PARENT: strmTmp << "_parent"; break;
				case TARGET_MODE_TOP: strmTmp << "_top"; break;
				case TARGET_MODE_BLANK: strmTmp << "_blank"; break;
			}
			strmTmp << "\"";
		}
		strmTmp << ">" << xhtmlCaption << "</a>";
	}

	return strmTmp.str();
}