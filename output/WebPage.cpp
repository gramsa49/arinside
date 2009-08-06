
/****************************************************************************** 
 * 
 *  file:  WebPage.cpp
 * 
 *  Copyright (c) 2007, Stefan Nerlich | stefan.nerlich@hotmail.com 
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#include "StdAfx.h"
#include "webpage.h"
#include "webutil.h"
#include "..\arinside.h"

using namespace OUTPUT;

#define LOG if(verboseMode) cout

extern int nFilesCreated;

CWebPage::CWebPage(string fileName, string title, int dirLevel, AppConfig &appConfig)
{
	this->fileName = fileName;	
	this->appConfig = appConfig;
	this->title = title;
	this->rootLevel = dirLevel;
}

CWebPage::~CWebPage(void)
{
	bodyStrm.str("");
	contentStrm.str("");
	navStrm.str("");
}

void CWebPage::AddNavigation(string nav)
{
	navStrm << nav << endl;
}

void CWebPage::AddContent(string bodyContent)
{	
	bodyStrm << bodyContent << endl;
}

void CWebPage::AddContentHead(string description)
{
	bodyStrm << description << "<br/><br/>" << endl;
}

string CWebPage::PageHeader()
{
	stringstream strm;
	strm << "<?xml version=\"1.0\" ?>" << endl;
	strm << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
    strm << "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">" << endl;
	strm << "<!-- saved from url=(0025)http://devdor.org/ -->" << endl;
	strm << "<head>" << endl;
	strm << "<title>" << title << "</title>" << endl;
	strm << "<meta http-equiv=\"content-language\" content=\"EN\" />" << endl;
	strm << "<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\" />" << endl;
	strm << "<meta http-equiv=\"expires\" content=\"-1\" />" << endl;
	strm << "<meta name=\"author\" content=\"ARInside\" />" << endl;
	strm << "<link rel=\"stylesheet\" type=\"text/css\" href=\"" << CWebUtil::RootPath(rootLevel) << "img/style.css\" />" << endl;
	strm << "<script src=\"" << CWebUtil::RootPath(rootLevel) << "img/sortscript.js\" type=\"text/javascript\"></script>" << endl;
	strm <<	"<script src=\"" << CWebUtil::RootPath(rootLevel) << "img/tabscript.js\" type=\"text/javascript\"></script>" << endl;
	strm << "</head>" << endl;
	return strm.str();
}

string CWebPage::DynamicHeaderText()
{
	stringstream strm;
	strm << "<table>" << endl;
	strm << "<tr>" << endl;
	strm << "<td>" << CWebUtil::Link("Main", CWebUtil::RootPath(rootLevel)+ CWebUtil::DocName("index"), "server.gif", rootLevel) << "</td>" << endl;
	strm << "<td>" << " (Server: " << CWebUtil::Link(appConfig.serverName, CWebUtil::RootPath(rootLevel) + "other/" +CWebUtil::DocName("server"), "", rootLevel) << "</td>" << endl;
    strm << "<td>" << "@" << "</td>" << endl;
	strm << "<td>" << "<a href=\"" << appConfig.companyUrl << "\" target=\"_blank\">" << appConfig.companyName << "</a>" << ")" << "</td>" << endl;
	strm << "</tr>" << endl;
	strm << "</table>" << endl;	
	return strm.str();
}

string CWebPage::CurrentDateTime()
{
	struct tm *curtime;
	time_t curseconds;
	char string[80];

	time(&curseconds);
	curtime = localtime(&curseconds);
	strftime(string, 80, "%Y-%m-%d %H:%M:%S",curtime);
	return string;
}

string CWebPage::DynamicFooterText()
{
	stringstream strm;
	strm << "<table><tr>" << endl;
	strm << "<td>" << CWebUtil::Link("Main", CWebUtil::RootPath(rootLevel)+ CWebUtil::DocName("index"), "next.gif", rootLevel)<< "</td>" << endl;
	strm << "<td>&nbsp;</td>" << endl;
	strm << "<td>" << CWebUtil::Link("Top", "#top", "up.gif", rootLevel)<< "</td>" << endl;
	strm << "<td>&nbsp;</td>" << endl;
	strm << "<td>(Page created "<< CurrentDateTime() <<" by <a href=\"http://devdor.org\" target=\"_blank\">ARInside v"<< AppVersion <<"</a>)</td>";
	strm << "</tr></table>" << endl;
	return strm.str();
}

string CWebPage::ContentOpen()
{
	stringstream strm;
	strm << "<body>" << endl;
	strm << "<a name=\"top\"></a>" << endl;
	strm << "<table class=\"TblMain\">" << endl;
	strm << "<tr><td class=\"TdMainHeader\" colspan=\"3\">" << endl;
	strm << DynamicHeaderText() << endl;
	strm << "</td></tr><tr><td class=\"TdMainMenu\">" << endl;
	strm << "<iframe id=\"IFrameMenu\" src=\"" << CWebUtil::RootPath(rootLevel) << "template/navigation.htm\" name=\"Navigation\" frameborder=\"0\">" << endl;
	strm << "<p>IFrame not supported by this browser.</p></iframe></td><td class=\"TdMainContent\">" << endl;
	
	return strm.str();
}

string CWebPage::ContentClose()
{
	stringstream strm;

	strm << "</td>" << endl;

	if(navStrm.str().size() > 0)
	{
		strm << "<td class=\"TdMainContentSmall\" style=\"width: 170px;\">" << endl;
		strm << navStrm.str() << endl;
		strm << "</td>" << endl;
	}
	else
	{
		strm << "<td></td>" << endl;
	}

	strm << "</tr><tr><td class=\"TdMainButtom\" colspan=\"3\">" << endl;
	strm << DynamicFooterText() << endl;
	strm << "</td></tr></table></body></html>" << endl;
	return strm.str();
}



string CWebPage::GetFileContent()
{	
	contentStrm << PageHeader() << endl;
	contentStrm << ContentOpen() << endl;
	contentStrm << bodyStrm.str() << endl;
	contentStrm << ContentClose();
	return this->contentStrm.str();
}


int CWebPage::SaveInFolder(string path)
{
	int result = 0;

	stringstream strm;
	if(!path.empty())
		strm << this->appConfig.targetFolder << "\\" << path << "\\" << CWebUtil::DocName(this->fileName);
	else
		strm << this->appConfig.targetFolder << "\\" << CWebUtil::DocName(this->fileName);

	try
	{	
		LOG << "Save file '" << strm.str();

		ofstream fout( strm.str().c_str(), ios::out);
		fout << this->GetFileContent() << endl;
		fout.close();

		LOG << "' [OK]" << endl;
		
		nFilesCreated++;
		result = 1;
	}
	catch(...)
	{		
		stringstream erStrm;
		erStrm << "Error saving file '" << strm.str() << "' to disk. ";
		throw(AppException(erStrm.str(), "undefined", "FileIo"));
	}

	return result;
}