
/****************************************************************************** 
 * 
 *  file:  ResourceFiles.cpp
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
#include ".\resourcefiles.h"

CResourceFiles::CResourceFiles(void)
{
}

CResourceFiles::~CResourceFiles(void)
{
}

void CResourceFiles::Load(AppConfig &appConfig)
{
	stringstream strm;	
	strm << appConfig.targetFolder << "\\" << "template" << "\\";
	LoadFromResource("NAVIGATION_HTM", "navigation.htm", strm.str());

	strm.str("");
	strm << appConfig.targetFolder << "\\" << "img" << "\\";	
	LoadFromResource("STYLE_CSS", "style.css", strm.str());
	LoadFromResource("SCRIPT", "script.js", strm.str());
	LoadFromResource("TABSCRIPT", "tabscript.js", strm.str());
	LoadFromResource("SORTSCRIPT", "sortscript.js", strm.str());
	LoadFromResource("SERVER", "server.gif", strm.str());
	LoadFromResource("ACTIVE_LINK", "active_link.gif", strm.str());
	LoadFromResource("AL_GUIDE", "al_guide.gif", strm.str());
	LoadFromResource("APPLICATION", "application.gif", strm.str());
	LoadFromResource("ARS_EDIT", "ars_edit.gif", strm.str());
	LoadFromResource("DISABLED", "disabled.gif", strm.str());
	LoadFromResource("DOC", "doc.gif", strm.str());
	LoadFromResource("DOWN", "down.gif", strm.str());
	LoadFromResource("USER", "user.gif", strm.str());
	LoadFromResource("EMPTY_LIST", "empty_list.gif", strm.str());
	LoadFromResource("ESCALATION", "escalation.gif", strm.str());
	LoadFromResource("FILTER", "filter.gif", strm.str());
	LoadFromResource("FILTER_GUIDE", "filter_guide.gif", strm.str());
	LoadFromResource("FOLDER", "folder.gif", strm.str());
	LoadFromResource("GROUP", "group.gif", strm.str());
	LoadFromResource("CHAPTER", "chapter.gif", strm.str());
	LoadFromResource("MENU", "menu.gif", strm.str());
	LoadFromResource("NEXT", "next.gif", strm.str());
	LoadFromResource("PACKING_LIST", "packing_list.gif", strm.str());
	LoadFromResource("PREV", "prev.gif", strm.str());
	LoadFromResource("SCHEMA", "schema.gif", strm.str());
	LoadFromResource("SEARCH", "search.gif", strm.str());
	LoadFromResource("UP", "up.gif", strm.str());
	LoadFromResource("WEBSERVICE", "webservice.gif", strm.str());		
	LoadFromResource("SORT_DESC", "sort_desc.gif", strm.str());		
	LoadFromResource("SORT_ASC", "sort_asc.gif", strm.str());		
	LoadFromResource("VISIBLE", "visible.gif", strm.str());		
	LoadFromResource("EDIT", "edit.gif", strm.str());		
	LoadFromResource("HIDDEN", "hidden.gif", strm.str());		
	LoadFromResource("INFO", "info.gif", strm.str());
}

void CResourceFiles::LoadFromResource(string strRes, string fileName, string path)
{
	LPVOID  data;
	HANDLE hfile;
	DWORD len,c;
	HRSRC	hres;
	HGLOBAL hres1;

	if (!(hres=FindResource(GetModuleHandle(NULL),strRes.c_str(),RT_RCDATA))
		|| !(len=SizeofResource(NULL,hres))
		|| !(hres1=LoadResource(NULL,hres))
		|| !(data=LockResource(hres1))) 
	{
		ExitProcess(0);
	}

	stringstream fullFileName;
	fullFileName << path << "\\" << fileName;
	
	if (!(hfile=CreateFile(fullFileName.str().c_str(), GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL))) 
	{
		ExitProcess(0);
	}

	WriteFile(hfile,data,len,&c,NULL);
	CloseHandle(hfile);	
}