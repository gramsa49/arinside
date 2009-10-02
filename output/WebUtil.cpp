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

using namespace OUTPUT;

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

string CWebUtil::WebPageSuffix()
{
	return "htm";
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

string CWebUtil::ImageTag(string imgName, int rootLevel)
{
	stringstream strm;

	int width  = 16;
	int height = 16;

	if(strcmp(imgName.c_str(), "doc.gif")==0)			{ width =15 ; height =10 ; }
	if(strcmp(imgName.c_str(), "down.gif")==0)			{ width =14 ; height =10 ; }
	if(strcmp(imgName.c_str(), "folder.gif")==0)		{ width =16 ; height =13 ; }
	if(strcmp(imgName.c_str(), "chapter.gif")==0)		{ width =15 ; height =13 ; }
	if(strcmp(imgName.c_str(), "next.gif")==0)			{ width =10 ; height =10 ; }
	if(strcmp(imgName.c_str(), "prev.gif")==0)			{ width =10 ; height =10 ; }
	if(strcmp(imgName.c_str(), "up.gif")==0)			{ width =14 ; height =10 ; }
	if(strcmp(imgName.c_str(), "visible.gif")==0)		{ width =18 ; height =18 ; }
	if(strcmp(imgName.c_str(), "edit.gif")==0)			{ width =18 ; height =18 ; }	
	if(strcmp(imgName.c_str(), "hidden.gif")==0)		{ width =18 ; height =18 ; }

	strm << "<img src=\"" << CWebUtil::RootPath(rootLevel) << "img/" << imgName << "\" width=\"" << width << "\" height=\"" << height << "\" alt=\"" << imgName << "\">";

	return strm.str();
}

string CWebUtil::ObjName(string objName)
{
	stringstream strm;
	strm << "<span class=\"objName\">" << objName << "</span>" << endl;
	return strm.str();
}


//targetMode
//_blank, to open the link in a new window
//_self, to open the link in the current window,
//_parent, um bei verschachtelten Framesets das aktuelle Frameset zu sprengen,
//_top, um bei verschachtelten Framesets alle Framesets zu sprengen.
string CWebUtil::Link(string caption, string linkTo, string imgName, int rootLevel)
{
	stringstream strm;

	string xhtmlCaption = Validate(caption);

	if(imgName.empty())
		strm << "<a href=\"" << linkTo << "\">" << xhtmlCaption << "</a>";
	else
		strm << ImageTag(imgName, rootLevel) << "<a href=\"" << Validate(linkTo) << "\">" << xhtmlCaption << "</a>";

	return strm.str();
}

string CWebUtil::Link(string caption, string linkTo, string imgName, int rootLevel, bool validate)
{
	stringstream strm;
	strm.str("");

	string xhtmlCaption = caption;

	if(validate)
	{
		xhtmlCaption = Validate(caption);
	}

	if(imgName.empty())
	{
		strm << "<a href=\"" << linkTo << "\">" << xhtmlCaption << "</a>";
	}
	else
	{
		strm << ImageTag(imgName, rootLevel) << "<a href=\"" << Validate(linkTo) << "\">" << xhtmlCaption << "</a>";
	}
	return strm.str();
}

string CWebUtil::LinkToActiveLinkIndex(int rootLevel)
{
	return LinkToActiveLinkIndex(-1, rootLevel);
}

string CWebUtil::LinkToActiveLinkIndex(int objectCount, int rootLevel)
{
	string name = "Active Link";

	if(objectCount > 1)
	{
		name = "Active Links";
	}

	return LinkToHelper(name, objectCount, "active_link", "active_link.gif", rootLevel);
}

string CWebUtil::LinkToActiveLinkGuideIndex(int rootLevel)
{
	return LinkToActiveLinkGuideIndex(-1, rootLevel);
}

string CWebUtil::LinkToActiveLinkGuideIndex(int objectCount, int rootLevel)
{
	string name = "Active Link Guide";

	if(objectCount > 1)
	{
		name = "Active Link Guides";
	}

	return LinkToHelper(name, objectCount, "active_link_guide", "al_guide.gif", rootLevel);
}

string CWebUtil::LinkToApplicationIndex(int rootLevel)
{
	return LinkToApplicationIndex(-1, rootLevel);
}

string CWebUtil::LinkToApplicationIndex(int objectCount, int rootLevel)
{
	string name = "Application";

	if(objectCount > 1)
	{
		name = "Applications";
	}

	return LinkToHelper(name, objectCount, "application", "application.gif", rootLevel);
}

string CWebUtil::LinkToEscalationIndex(int rootLevel)
{
	return LinkToEscalationIndex(-1, rootLevel);
}

string CWebUtil::LinkToEscalationIndex(int objectCount, int rootLevel)
{
	string name = "Escalation";

	if(objectCount > 1)
	{
		name = "Escalations";
	}

	return LinkToHelper(name, objectCount, "escalation", "escalation.gif", rootLevel);
}

string CWebUtil::LinkToFilterIndex(int rootLevel)
{
	return LinkToFilterIndex(-1, rootLevel);
}

string CWebUtil::LinkToFilterIndex(int objectCount, int rootLevel)
{
	string name = "Filter";

	if(objectCount > 1)
	{
		name = "Filters";
	}

	return LinkToHelper(name, objectCount, "filter", "filter.gif", rootLevel);
}

string CWebUtil::LinkToFilterGuideIndex(int rootLevel)
{
	return LinkToFilterGuideIndex(-1, rootLevel);
}

string CWebUtil::LinkToFilterGuideIndex(int objectCount, int rootLevel)
{
	string name = "Filter Guide";

	if(objectCount > 1)
	{
		name = "Filter Guides";
	}

	return LinkToHelper(name, objectCount, "filter_guide", "filter_guide.gif", rootLevel);
}

string CWebUtil::LinkToGroupIndex(int rootLevel)
{
	return LinkToGroupIndex(-1, rootLevel);
}

string CWebUtil::LinkToGroupIndex(int objectCount, int rootLevel)
{
	string name = "Group";

	if(objectCount > 1)
	{
		name = "Groups";
	}

	return LinkToHelper(name, objectCount, "group", "group.gif", rootLevel);
}

string CWebUtil::LinkToMenuIndex(int rootLevel)
{
	return LinkToMenuIndex(-1, rootLevel);
}

string CWebUtil::LinkToMenuIndex(int objectCount, int rootLevel)
{
	string name = "Menu";

	if(objectCount > 1)
	{
		name = "Menus";
	}

	return LinkToHelper(name, objectCount, "menu", "menu.gif", rootLevel);
}

string CWebUtil::LinkToPackingListIndex(int rootLevel)
{
	return LinkToPackingListIndex(-1, rootLevel);
}

string CWebUtil::LinkToPackingListIndex(int objectCount, int rootLevel)
{
	string name = "Packing List";

	if(objectCount > 1)
	{
		name = "Packing Lists";
	}

	return LinkToHelper(name, objectCount, "packing_list", "packing_list.gif", rootLevel);
}

string CWebUtil::LinkToRoleIndex(int rootLevel)
{
	return LinkToRoleIndex(-1, rootLevel);
}

string CWebUtil::LinkToRoleIndex(int objectCount, int rootLevel)
{
	string name = "Role";

	if(objectCount > 1)
	{
		name = "Roles";
	}

	return LinkToHelper(name, objectCount, "role", "doc.gif", rootLevel);
}

string CWebUtil::LinkToSchemaIndex(int rootLevel)
{
	return LinkToSchemaIndex(-1, rootLevel);
}

string CWebUtil::LinkToSchemaIndex(int objectCount, int rootLevel)
{
	string name = "Form";

	if(objectCount > 1)
	{
		name = "Forms";
	}

	return LinkToHelper(name, objectCount, "schema", "schema.gif", rootLevel);
}

string CWebUtil::LinkToUserIndex(int rootLevel)
{
	return LinkToUserIndex(-1, rootLevel);
}

string CWebUtil::LinkToUserIndex(int objectCount, int rootLevel)
{
	string name = "User";

	if(objectCount > 1)
	{
		name = "Users";
	}

	return LinkToHelper(name, objectCount, "user", "user.gif", rootLevel);
}

string CWebUtil::LinkToWebServiceIndex(int rootLevel)
{
	return LinkToWebServiceIndex(-1, rootLevel);
}

string CWebUtil::LinkToWebServiceIndex(int objectCount, int rootLevel)	
{
	string name = "Web Service";

	if(objectCount > 1)
	{
		name = "Web Services";
	}

	return LinkToHelper(name, objectCount, "webservice", "webservice.gif", rootLevel);
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

string CWebUtil::LinkToHelper(string name, int objectCount, string folderName, string image, int rootLevel)
{
	stringstream strmTmp;
	strmTmp.str("");

	if(objectCount > -1)
	{
		strmTmp << objectCount << " ";
	}

	strmTmp << CWebUtil::Link(name, RootPath(rootLevel) + folderName + "/" + CWebUtil::DocName("index"), image, rootLevel);
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
