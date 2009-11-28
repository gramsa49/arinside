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
#include "ResourceFiles.h"

CResourceFiles::CResourceFiles(void)
{
}

CResourceFiles::~CResourceFiles(void)
{
}

void CResourceFiles::Load(AppConfig &appConfig)
{
	stringstream strm;	
	//strm << appConfig.targetFolder << "\\" << "template" << "\\";
	//LoadFromResource("NAVIGATION_HTM", "navigation.htm", strm.str());

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
	LoadFromResource("IMAGE", "image.gif", strm.str());
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
