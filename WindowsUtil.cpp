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
#include "resource.h"
#include "WindowsUtil.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#endif // WIN32

CWindowsUtil::CWindowsUtil(AppConfig &appConfig)
{
	this->appConfig = appConfig;
}

CWindowsUtil::~CWindowsUtil(void)
{
}

void CWindowsUtil::Load()
{
	stringstream strm;	
	//strm << appConfig.targetFolder << "/" << "template" << "/";
	//LoadFromResource("NAVIGATION_HTM", "navigation.htm", strm.str());

	strm.str("");
	strm << appConfig.targetFolder << "/" << "img" << "/";	
	LoadFromResource(ID_STYLE_CSS, "style.css", strm.str());
	LoadFromResource(ID_SCRIPT, "script.js", strm.str());
	LoadFromResource(ID_TABSCRIPT, "tabscript.js", strm.str());
	LoadFromResource(ID_SORTSCRIPT, "sortscript.js", strm.str());
	LoadFromResource(ID_SERVER, "server.gif", strm.str());
	LoadFromResource(ID_ACTIVE_LINK, "active_link.gif", strm.str());
	LoadFromResource(ID_AL_GUIDE, "al_guide.gif", strm.str());
	LoadFromResource(ID_APPLICATION, "application.gif", strm.str());
	LoadFromResource(ID_ARS_EDIT, "ars_edit.gif", strm.str());
	LoadFromResource(ID_DISABLED, "disabled.gif", strm.str());
	LoadFromResource(ID_DOC, "doc.gif", strm.str());
	LoadFromResource(ID_DOWN, "down.gif", strm.str());
	LoadFromResource(ID_USER, "user.gif", strm.str());
	LoadFromResource(ID_EMPTY_LIST, "empty_list.gif", strm.str());
	LoadFromResource(ID_ESCALATION, "escalation.gif", strm.str());
	LoadFromResource(ID_FILTER, "filter.gif", strm.str());
	LoadFromResource(ID_FILTER_GUIDE, "filter_guide.gif", strm.str());
	LoadFromResource(ID_FOLDER, "folder.gif", strm.str());
	LoadFromResource(ID_GROUP, "group.gif", strm.str());
	LoadFromResource(ID_CHAPTER, "chapter.gif", strm.str());
	LoadFromResource(ID_MENU, "menu.gif", strm.str());
	LoadFromResource(ID_NEXT, "next.gif", strm.str());
	LoadFromResource(ID_PACKING_LIST, "packing_list.gif", strm.str());
	LoadFromResource(ID_PREV, "prev.gif", strm.str());
	LoadFromResource(ID_SCHEMA, "schema.gif", strm.str());
	LoadFromResource(ID_SEARCH, "search.gif", strm.str());
	LoadFromResource(ID_UP, "up.gif", strm.str());
	LoadFromResource(ID_WEBSERVICE, "webservice.gif", strm.str());
	LoadFromResource(ID_SORT_DESC, "sort_desc.gif", strm.str());
	LoadFromResource(ID_SORT_ASC, "sort_asc.gif", strm.str());
	LoadFromResource(ID_VISIBLE, "visible.gif", strm.str());
	LoadFromResource(ID_EDIT, "edit.gif", strm.str());
	LoadFromResource(ID_HIDDEN, "hidden.gif", strm.str());
	LoadFromResource(ID_INFO, "info.gif", strm.str());
	LoadFromResource(ID_IMAGE, "image.gif", strm.str());
}

void CWindowsUtil::LoadFromResource(unsigned int res, string fileName, string path)
{
	void* data;
	//HANDLE hfile;
	unsigned int len;
#ifdef WIN32
	HRSRC	hres;
	HGLOBAL hres1;

	if (!(hres=FindResource(GetModuleHandle(NULL),MAKEINTRESOURCE(res),RT_RCDATA))
		|| !(len=SizeofResource(NULL,hres))
		|| !(hres1=LoadResource(NULL,hres))
		|| !(data=LockResource(hres1))) 
	{
		ExitProcess(0);
	}
#else
	// TODO: implement loading of resources for *nix-systems. Use objcopy and
	// a table to find out the mem-address of the id given via res-parameter.
#endif

	stringstream fullFileName;
	fullFileName << path << "\\" << fileName;

	try
	{
		ofstream fout(fullFileName.str().c_str(), ios::binary);

		try
		{
			fout.write((const char*)data, len);
		}
		catch (exception &e)
		{
			stringstream erStrm;
			erStrm << "Error saving file '" << fullFileName.str() << "' to disk. Error: " << e.what();
		}
		fout.close();

		//if (!(hfile=CreateFile(fullFileName.str().c_str(), GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL))) 
		//{
		//	ExitProcess(0);
		//}

		//WriteFile(hfile,data,len,&c,NULL);
		//CloseHandle(hfile);	
	}
	catch (exception e)
	{
		stringstream erStrm;
		erStrm << "Error saving file '" << fullFileName.str() << "' to disk. Error: " << e.what();
	}
}

bool CWindowsUtil::CreateAppDirectory()
{
	int nResult = 
#ifdef WIN32
	_mkdir(appConfig.targetFolder.c_str());
#else
	mkdir(appConfig.targetFolder.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
#endif
	cout << "Create target directory: " << appConfig.targetFolder << " [" << nResult <<"]" << endl;	

	return (nResult==0);
}

int CWindowsUtil::CreateSubDirectory(string name)
{	
	stringstream strm;
	strm << appConfig.targetFolder << "/" << name;

	try
	{
#ifdef WIN32		
		return (_mkdir(strm.str().c_str()) == 0);
#else
		return (mkdir(strm.str().c_str(), S_IRWXU|S_IRWXG|S_IRWXO) == 0);
#endif
	}
	catch(exception& e)
	{
		cout << "Error creating directory '" << strm.str() << "': " << e.what() << endl;
	}

	return 0;
}
