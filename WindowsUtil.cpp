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
#include "AppException.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#else
#include <errno.h>
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
	switch (res)
	{
		case ID_ACTIVE_LINK: data = _binary_res_active_link_gif_start; len = (unsigned int)_binary_res_active_link_gif_size; break;
		case ID_AL_GUIDE: data = _binary_res_al_guide_gif_start; len = (unsigned int)_binary_res_al_guide_gif_size; break;
		case ID_APPLICATION: data = _binary_res_application_gif_start; len = (unsigned int)_binary_res_application_gif_size; break;
		case ID_ARS_EDIT: data = _binary_res_ars_edit_gif_start; len = (unsigned int)_binary_res_ars_edit_gif_size; break;
		case ID_CHAPTER: data = _binary_res_chapter_gif_start; len = (unsigned int)_binary_res_chapter_gif_size; break;
		case ID_DISABLED: data = _binary_res_disabled_gif_start; len = (unsigned int)_binary_res_disabled_gif_size; break;
		case ID_DOC: data = _binary_res_doc_gif_start; len = (unsigned int)_binary_res_doc_gif_size; break;
		case ID_DOWN: data = _binary_res_down_gif_start; len = (unsigned int)_binary_res_down_gif_size; break;
		case ID_EDIT: data = _binary_res_edit_gif_start; len = (unsigned int)_binary_res_edit_gif_size; break;
		case ID_EMPTY_LIST: data = _binary_res_empty_list_gif_start; len = (unsigned int)_binary_res_empty_list_gif_size; break;
		case ID_ESCALATION: data = _binary_res_escalation_gif_start; len = (unsigned int)_binary_res_escalation_gif_size; break;
		case ID_FILTER: data = _binary_res_filter_gif_start; len = (unsigned int)_binary_res_filter_gif_size; break;
		case ID_FILTER_GUIDE: data = _binary_res_filter_guide_gif_start; len = (unsigned int)_binary_res_filter_guide_gif_size; break;
		case ID_FOLDER: data = _binary_res_folder_gif_start; len = (unsigned int)_binary_res_folder_gif_size; break;
		case ID_GROUP: data = _binary_res_group_gif_start; len = (unsigned int)_binary_res_group_gif_size; break;
		case ID_HIDDEN: data = _binary_res_hidden_gif_start; len = (unsigned int)_binary_res_hidden_gif_size; break;
		case ID_IMAGE: data = _binary_res_image_gif_start; len = (unsigned int)_binary_res_image_gif_size; break;
		case ID_INFO: data = _binary_res_info_gif_start; len = (unsigned int)_binary_res_info_gif_size; break;
		case ID_MENU: data = _binary_res_menu_gif_start; len = (unsigned int)_binary_res_menu_gif_size; break;
		case ID_NEXT: data = _binary_res_next_gif_start; len = (unsigned int)_binary_res_next_gif_size; break;
		case ID_PACKING_LIST: data = _binary_res_packing_list_gif_start; len = (unsigned int)_binary_res_packing_list_gif_size; break;
		case ID_PREV: data = _binary_res_prev_gif_start; len = (unsigned int)_binary_res_prev_gif_size; break;
		case ID_SCHEMA: data = _binary_res_schema_gif_start; len = (unsigned int)_binary_res_schema_gif_size; break;
		case ID_SCRIPT: data = _binary_res_script_js_start; len = (unsigned int)_binary_res_script_js_size; break;
		case ID_SEARCH: data = _binary_res_search_gif_start; len = (unsigned int)_binary_res_search_gif_size; break;
		case ID_SERVER: data = _binary_res_server_gif_start; len = (unsigned int)_binary_res_server_gif_size; break;
		case ID_SORT_ASC: data = _binary_res_sort_asc_gif_start; len = (unsigned int)_binary_res_sort_asc_gif_size; break;
		case ID_SORT_DESC: data = _binary_res_sort_desc_gif_start; len = (unsigned int)_binary_res_sort_desc_gif_size; break;
		case ID_SORTSCRIPT: data = _binary_res_sortscript_js_start; len = (unsigned int)_binary_res_sortscript_js_size; break;
		case ID_STYLE_CSS: data = _binary_res_style_css_start; len = (unsigned int)_binary_res_style_css_size; break;
		case ID_TABSCRIPT: data = _binary_res_tabscript_js_start; len = (unsigned int)_binary_res_tabscript_js_size; break;
		case ID_UP: data = _binary_res_up_gif_start; len = (unsigned int)_binary_res_up_gif_size; break;
		case ID_USER: data = _binary_res_user_gif_start; len = (unsigned int)_binary_res_user_gif_size; break;
		case ID_VISIBLE: data = _binary_res_visible_gif_start; len = (unsigned int)_binary_res_visible_gif_size; break;
		case ID_WEBSERVICE: data = _binary_res_webservice_gif_start; len = (unsigned int)_binary_res_webservice_gif_size; break;
	}
#endif

	stringstream fullFileName;
	fullFileName << path << "/" << fileName;

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
	cout << "Create target directory: " << appConfig.targetFolder << " [" << (nResult == 0 ? "OK" : strerror(errno)) <<"]" << endl;	

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

string CWindowsUtil::GetRealPathName(const std::string &path)
{
#ifdef WIN32
	char buffer[MAX_PATH]; buffer[0] = 0;
	LPSTR *filePos = NULL;

	DWORD len = GetFullPathName(path.c_str(), MAX_PATH, buffer, filePos);
	if (len > MAX_PATH || len == 0)
	{
		stringstream tmp;
		tmp << "Error: could not retrieve the full output path! (" << GetLastError() << ")";
		throw AppException(tmp.str(), "filesystem");			
	}
#else
	// NOTE: realpath doesn't return the full path under SunOS if a relativ path is specified.
	char buffer[PATH_MAX]; buffer[0] = 0;
	char *p;
	p = realpath(path.c_str(), buffer);
	if (p == NULL)
	{
		if (errno == ENOENT) return path;
		stringstream tmp;
		tmp << strerror(errno) << ": " << path;
		throw AppException(tmp.str(), "filesystem");			
	}
#endif
	return buffer;
}
