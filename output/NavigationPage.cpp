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
#include "NavigationPage.h"
#include "WebUtil.h"
#include "../AppException.h"
#include "../ARInside.h"

using namespace OUTPUT;

void CNavigationPage::Write()
{
	stringstream strmName;
	strmName << this->conf.targetFolder << "/" << CWebUtil::DocName(this->fileName);
	CARInside *pInside = CARInside::GetInstance();

	try
	{	
		LOG << "Save file '" << strmName.str();
		ofstream fout( strmName.str().c_str(), ios::out);

		// header
		Header(fout);

		// content
		fout << "<li>";
			Image(fout, "../img/", "schema.gif", 16, 16); Link(fout, "../schema/index.htm", "Forms");

			fout << "<ul>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../schema/index_regular.htm", "Regular"); fout << "</li>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../schema/index_join.htm"   , "Join");    fout << "</li>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../schema/index_view.htm"   , "View");    fout << "</li>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../schema/index_dialog.htm" , "Dialog");  fout << "</li>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../schema/index_vendor.htm" , "Vendor");  fout << "</li>";
			fout << "</ul>";
		fout << "</li>";

		fout << "<li>";
			Image(fout, "../img/", "active_link.gif", 16, 16); Link(fout, "../active_link/index.htm", "Active Links");
			fout << "<ul><li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../active_link/index_action.htm" , "By Action"); fout << "</li></ul>";
		fout << "</li>";

		fout << "<li>";
			Image(fout, "../img/", "filter.gif", 16, 16); Link(fout, "../filter/index.htm", "Filters");
			fout << "<ul><li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../filter/index_action.htm" , "By Action"); fout << "</li></ul>";
		fout << "</li>";

		fout << "<li>";
			Image(fout, "../img/", "escalation.gif", 16, 16); Link(fout, "../escalation/index.htm", "Escalations");
			fout << "<ul><li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../escalation/index_action.htm" , "By Action"); fout << "</li></ul>";
		fout << "</li>";

		fout << "<li>"; Image(fout, "../img/", "menu.gif", 16, 16); Link(fout, "../menu/index.htm", "Menus"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "al_guide.gif", 16, 16); Link(fout, "../active_link_guide/index.htm", "Active Link Guides"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "filter_guide.gif", 16, 16); Link(fout, "../filter_guide/index.htm", "Filter Guides"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "application.gif", 16, 16); Link(fout, "../application/index.htm", "Applications"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "packing_list.gif", 16, 16); Link(fout, "../packing_list/index.htm", "Packing Lists"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "webservice.gif", 16, 16); Link(fout, "../webservice/index.htm", "Webservices"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "group.gif", 16, 16); Link(fout, "../group/index.htm", "Groups"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../role/index.htm", "Roles"); fout << "</li>";
		fout << "<li>"; Image(fout, "../img/", "user.gif", 16, 16); Link(fout, "../user/index.htm", "Users"); fout << "</li>";

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		if (pInside->CompareServerVersion(7,5) >= 0)
		{
			fout << "<li>"; Image(fout, "../img/", "image.gif", 15, 10); Link(fout, "../image/index.htm", "Images"); fout << "</li>";
		}
#endif

		fout << "<li>"; 
			Image(fout, "../img/", "folder.gif", 16, 13); fout << "Information:";
			fout << "<ul>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../other/message_list.htm", "Messages"); fout << "</li>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../other/global_field_list.htm", "Global&nbsp;Fields"); fout << "</li>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../other/validation_main.htm", "Validator"); fout << "</li>";
				fout << "<li>"; Image(fout, "../img/", "doc.gif", 15, 10); Link(fout, "../other/analyzer_main.htm", "Analyzer"); fout << "</li>";
			fout << "</ul>";
		fout << "</li>";

		// footer
		Footer(fout);

		fout.close();

		LOG << "' [OK]" << endl;
	}
	catch(exception& e)
	{		
		stringstream erStrm;
		erStrm << "Error saving file '" << strmName.str() << "' to disk. Error: " << e.what();
		throw(AppException(erStrm.str(), "undefined", "FileIo"));
	}
}

void CNavigationPage::Header(ofstream &strm)
{
	strm << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?> " << endl;

	// TODO: dont know if the encoding would be the best choice .. 
	// one day the whole program should use unicode support
	strm << 
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \
\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\
<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\
<!-- saved from url=(0025)http://arinside.org/ -->\
<head>\
	<title>Navigation</title>\
		<meta name=\"description\" content=\"ARSystem Documentation\" />\
		<meta name=\"author\" content=\"Demo\" />\
		<meta http-equiv=\"CONTENT-LANGUAGE\" content=\"EN\" />\
		<meta name=\"ROBOTS\" content=\"INDEX, FOLLOW\" />\
		<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\" />\
		<link rel=\"stylesheet\" type=\"text/css\" href=\"../img/style.css\" />\
	</head>\
	<body><ul>";
}

void CNavigationPage::Footer(ofstream &strm)
{
	strm << "</ul></body></html>";
}

void CNavigationPage::Image(ofstream &strm, char *path, char *file, int width, int height)
{
	strm << "<img src=\"" << path << file << "\" width=\"" << width << "\" height=\"" << height << "\" alt=\"" << file << "\" />";
}

void CNavigationPage::Link(ofstream &strm, char *href, char* content)
{
	strm << "<a href=\"" << href << "\" target=\"_parent\">" << content << "</a>";
}