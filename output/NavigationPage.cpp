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
#include "../gzstream.h"

using namespace OUTPUT;

void CNavigationPage::Write()
{
	// TODO: maybe add this page to IFileStructure?? or is a static file enough in this scenario??

	stringstream strmName;
	strmName << this->conf.targetFolder << "/" << CWebUtil::DocName(this->fileName);
	CARInside *pInside = CARInside::GetInstance();

	try
	{
		LOG << "Save file '" << strmName.str();

		ostream *outStream;
#ifdef ARINSIDE_ENABLE_ZLIB_SUPPORT
		if (pInside->appConfig.bGZCompression)
			outStream = new ogzstream(strmName.str().c_str(), ios::out);
		else 
#endif
			outStream = new ofstream(strmName.str().c_str(), ios::out);

		ostream &fout = *outStream;

		// header
		Header(fout);

		// content
		fout << "<li>";
			fout << CWebUtil::Link("Forms", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_SCHEMA), "schema.gif", rootLevel, TARGET_MODE_PARENT);			

			fout << "<ul>"; 
				fout << "<li>" << CWebUtil::Link("Regular", CPageParams(PAGE_SCHEMA_REGULAR), "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
				fout << "<li>" << CWebUtil::Link("Join"   , CPageParams(PAGE_SCHEMA_JOIN)   , "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
				fout << "<li>" << CWebUtil::Link("View"   , CPageParams(PAGE_SCHEMA_VIEW)   , "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
				fout << "<li>" << CWebUtil::Link("Dialog" , CPageParams(PAGE_SCHEMA_DIALOG) , "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
				fout << "<li>" << CWebUtil::Link("Vendor" , CPageParams(PAGE_SCHEMA_VENDOR) , "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
			fout << "</ul>";
		fout << "</li>";

		fout << "<li>";
			fout << CWebUtil::Link("Active Links", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK), "active_link.gif", rootLevel, TARGET_MODE_PARENT);			
			fout << "<ul><li>" << CWebUtil::Link("By Action", CPageParams(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_ACTIVE_LINK), "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li></ul>";
		fout << "</li>";

		fout << "<li>";
			fout << CWebUtil::Link("Filters", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER), "filter.gif", rootLevel, TARGET_MODE_PARENT);
			fout << "<ul>";
				fout << "<li>" << CWebUtil::Link("By Action", CPageParams(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_FILTER), "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_710
				if (pInside->CompareServerVersion(7,1) >= 0)
				{
					fout << "<li>" << CWebUtil::Link("Error Handler", CPageParams(PAGE_FILTER_ERRORHANDLERS), "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
				}
#endif
			fout << "</ul>";
		fout << "</li>";

		fout << "<li>";
			fout << CWebUtil::Link("Escalations", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION), "escalation.gif", rootLevel, TARGET_MODE_PARENT);
			fout << "<ul><li>" << CWebUtil::Link("By Action", CPageParams(PAGE_ACTION_OVERVIEW, AR_STRUCT_ITEM_XML_ESCALATION), "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li></ul>";
		fout << "</li>";

		fout << "<li>" << CWebUtil::Link("Menus"             , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CHAR_MENU)                    , "menu.gif"        , rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Active Link Guides", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_GUIDE)       , "al_guide.gif"    , rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Filter Guides"     , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_FILTER_GUIDE), "filter_guide.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Applications"      , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_APP)         , "application.gif" , rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Packing Lists"     , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_PACK)        , "packing_list.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Webservices"       , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_CONTAINER, ARCON_WEBSERVICE)  , "webservice.gif"  , rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Groups"            , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_GROUP)                        , "group.gif"       , rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Roles"             , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_ROLE)                         , "doc.gif"         , rootLevel, TARGET_MODE_PARENT) << "</li>";
		fout << "<li>" << CWebUtil::Link("Users"             , CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_USER)                         , "user.gif"        , rootLevel, TARGET_MODE_PARENT) << "</li>";

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
		if (pInside->CompareServerVersion(7,5) >= 0)
		{
			fout << "<li>" << CWebUtil::Link("Images", CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_IMAGE), "image.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
		}
#endif

		fout << "<li>"; 
			Image(fout, "../img/", "folder.gif", 16, 13); fout << "Information:";
			fout << "<ul>";
				fout << "<li>" << CWebUtil::Link("Messages"          , CPageParams(PAGE_MESSAGES)      , "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
				fout << "<li>" << CWebUtil::Link("Global&nbsp;Fields", CPageParams(PAGE_GLOBALFIELS)   , "doc.gif", rootLevel, false, TARGET_MODE_PARENT) << "</li>";
				fout << "<li>" << CWebUtil::Link("Validator"         , CPageParams(PAGE_VALIDATOR_MAIN), "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
				fout << "<li>" << CWebUtil::Link("Analyzer"          , CPageParams(PAGE_ANALYZER_MAIN) , "doc.gif", rootLevel, TARGET_MODE_PARENT) << "</li>";
			fout << "</ul>";
		fout << "</li>";

		// footer
		Footer(fout);

		outStream->flush();
		delete outStream;

		LOG << "' [OK]" << endl;
	}
	catch(exception& e)
	{		
		stringstream erStrm;
		erStrm << "Error saving file '" << strmName.str() << "' to disk. Error: " << e.what();
		throw(AppException(erStrm.str(), "undefined", "FileIo"));
	}
}

void CNavigationPage::Header(ostream &strm)
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

void CNavigationPage::Footer(ostream &strm)
{
	strm << "</ul></body></html>";
}

void CNavigationPage::Image(ostream &strm, char *path, char *file, int width, int height)
{
	strm << "<img src=\"" << path << file << "\" width=\"" << width << "\" height=\"" << height << "\" alt=\"" << file << "\" />";
}
