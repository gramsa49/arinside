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
#include "FileSystemUtil.h"
#include "AppException.h"
#include "ARApi.h"

#include <sys/types.h>
#include <sys/stat.h>

FileSystemUtil::FileSystemUtil(AppConfig &appConfig)
: appConfig(appConfig)
{
}

FileSystemUtil::~FileSystemUtil(void)
{
}

void FileSystemUtil::Load()
{
	stringstream strm;	

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
	LoadFromResource(ID_SCHEMAPAGE_JS, "schema_page.js", strm.str());
	LoadFromResource(ID_JQUERY_JS, "jquery.js", strm.str());
	LoadFromResource(ID_JQUERY_UI_JS, "jquery-ui.js", strm.str());
	LoadFromResource(ID_JQUERY_UI_CUSTOM_CSS, "jquery-ui-custom.css", strm.str());
	LoadFromResource(ID_JQUERY_ADDRESS_JS, "jquery.address.js", strm.str());
	LoadFromResource(ID_JQUERY_TIMER_JS, "jquery.timer.js", strm.str());
	LoadFromResource(ID_ARSHELPER_JS, "arshelper.js", strm.str());

	// specical subfolder "image" for jquery-ui
	strm.str("");
	strm << "img" << "/" << "images" << "/";
	this->CreateSubDirectory(strm.str());

	strm.str("");
	strm << appConfig.targetFolder << "/" << "img" << "/" << "images" << "/";	
	LoadFromResource(ID_JQUERY_UI_IMAGE_BGGLASS_E6, "ui-bg_glass_75_e6e6e6_1x400.png", strm.str());
	LoadFromResource(ID_JQUERY_UI_IMAGE_BGGLASS_FF, "ui-bg_glass_65_ffffff_1x400.png", strm.str());
}

void FileSystemUtil::LoadFromResource(unsigned int res, string fileName, string path)
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
		case ID_SCHEMAPAGE_JS: data = _binary_res_schema_page_js_start; len = (unsigned int)_binary_res_schema_page_js_size; break;
		case ID_JQUERY_JS: data = _binary_thirdparty_jquery_jquery_js_start; len = _binary_thirdparty_jquery_jquery_js_size; break;
		case ID_JQUERY_UI_JS: data = _binary_thirdparty_jquery_jquery_ui_js_start; len = _binary_thirdparty_jquery_jquery_ui_js_size; break;
		case ID_JQUERY_UI_CUSTOM_CSS: data = _binary_thirdparty_jquery_jquery_ui_custom_css_start; len = _binary_thirdparty_jquery_jquery_ui_custom_css_size; break;
		case ID_JQUERY_ADDRESS_JS: data = _binary_thirdparty_jquery_jquery_address_min_js_start; len = _binary_thirdparty_jquery_jquery_address_min_js_size; break;
		case ID_JQUERY_TIMER_JS: data = _binary_thirdparty_jquery_jquery_timers_js_start; len = _binary_thirdparty_jquery_jquery_timers_js_size; break;
		case ID_JQUERY_UI_IMAGE_BGGLASS_E6: data = _binary_thirdparty_jquery_images_ui_bg_glass_75_e6e6e6_1x400_png_start; len = _binary_thirdparty_jquery_images_ui_bg_glass_75_e6e6e6_1x400_png_size; break;
		case ID_JQUERY_UI_IMAGE_BGGLASS_FF: data = _binary_thirdparty_jquery_images_ui_bg_glass_65_ffffff_1x400_png_start; len = _binary_thirdparty_jquery_images_ui_bg_glass_65_ffffff_1x400_png_size; break;
		case ID_ARSHELPER_JS: data = _binary_res_arshelper_js_start; len = _binary_res_arshelper_js_size; break;
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
	}
	catch (exception e)
	{
		stringstream erStrm;
		erStrm << "Error saving file '" << fullFileName.str() << "' to disk. Error: " << e.what();
	}
}

bool FileSystemUtil::CreateAppDirectory()
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

int FileSystemUtil::CreateSubDirectory(string name)
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

string FileSystemUtil::GetRealPathName(const std::string &path)
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

bool FileSystemUtil::FileExistsAndReadable(const std::string &filename)
{
	bool result = false;

	try
	{
		fstream fin;
		fin.open(filename.c_str(),ios::in);
		result = fin.is_open();
		fin.close();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION An error occured while checking file exists: " << e.what() << endl;
	}

	return result;
}

bool FileSystemUtil::DeleteDirectory(const char* sPath, bool topLevel)
{
#ifdef WIN32
	try
	{
		HANDLE hFind; // file handle
		WIN32_FIND_DATA FindFileData;

		char DirPath[MAX_PATH+1];
		char FileName[MAX_PATH+1];

		strncpy(DirPath, sPath, MAX_PATH);
		strncat(DirPath, "/", MAX_PATH);
		DirPath[MAX_PATH] = 0;

		strncpy(FileName, sPath, MAX_PATH);
		strncat(FileName, "/*", MAX_PATH); // searching all files
		FileName[MAX_PATH] = 0;

		hFind = FindFirstFile(FileName, &FindFileData); // find the first file
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (IsDots(FindFileData.cFileName))
					continue;

				strcpy(FileName + strlen(DirPath), FindFileData.cFileName);
				if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					// we have found a directory, recurse
					LOG << "Delete " << FileName << endl;

					if( !DeleteDirectory(FileName, false) )
						break; // directory couldn't be deleted
				}
				else
				{
					if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
						_chmod(FileName, _S_IWRITE); // change read-only file mode

					LOG << "Delete " << FileName << endl;
					if( !DeleteFile(FileName) )
						break; // file couldn't be deleted
				}
			}

			while( FindNextFile(hFind,&FindFileData) );	
			FindClose(hFind); // closing file handle

		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in DeleteDirectory: " << e.what() << endl; 
	}	

	return (topLevel || _rmdir(sPath)==0 ? true : false); // remove the empty (maybe not) directory
#else
	if (strlen(sPath) == 0) return false;

	// first off, we need to create a pointer to a directory
	DIR *pdir;
	pdir = opendir(sPath);
	
	struct dirent *pent = NULL;
	if (pdir == NULL)
		return false; // return false to say "we couldn't do it"

	string file;
	struct stat stats;

	while (pent = readdir(pdir)) { // while there is still something in the directory to list
		if (!IsDots(pent->d_name))
		{
			file = sPath;
			char lastChar = file.at(file.length()-1);
		  if (lastChar != '/' || lastChar != '\\') file += "/";
			file += pent->d_name;		

			if (stat(file.c_str(), &stats) != 0)
				return false;

			LOG << "Delete " << file << endl;

			if (S_ISDIR(stats.st_mode))
				DeleteDirectory(file.c_str(), false);
		else
				remove(file.c_str());
		}
	}

	// finally, let's clean up
	closedir(pdir); // close the directory
	if (!topLevel && !rmdir(sPath)) return false; // delete the directory
	return true;
#endif
}

bool FileSystemUtil::IsDots(const char* str) 
{
	if(strcmp(str,".") && strcmp(str,"..")) return false;
	return true;
}

int FileSystemUtil::ValidateTargetDir(string targetFolder)
{		
	int nResult = -1;
	try
	{
		cout << "Validating target folder: " << targetFolder << endl;

		stringstream fName;
		fName << targetFolder << "/valid.txt";

		ofstream fout( fName.str().c_str(), ios::out);
		fout << "arinside" << endl;
		fout.close();

		nResult = remove(fName.str().c_str());
	}
	catch(exception& e)
	{
		cout << "EXCEPTION ValidateTargetDir '" << targetFolder << "' -- " << e.what() << endl;
	}

	return nResult;
}

void FileSystemUtil::CompactFolder(string path)
{
#ifdef WIN32
	string compactCmd = "compact /C /I /Q /S:" + path;
	WinExec(compactCmd.c_str(), SW_SHOWNORMAL);
#else
	cout << "[WARN] CompactFolder is only supported on windows platform!" << endl;
#endif
}

string FileSystemUtil::GetExecutableDirectory(const char* argv_0)
{
	// http://stackoverflow.com/questions/933850/how-to-find-the-location-of-the-executable-in-c
#if WIN32
	DWORD size = 1024;
	LPTSTR buffer = (LPTSTR)malloc(size * sizeof(TCHAR));
ged_start:
	if (realloc(buffer, size) == NULL)
	{
		cerr << "GetExecutableDirectory: Failed to realloc memory buffer to " << size << "!" << endl;
	}
	
	if (GetModuleFileName(NULL, buffer, size) == 0)
	{
		cerr << "Failed to get executable directory" << GetFormattedMessage(GetLastError()) << endl;
		return "";
	}
	
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		size = size << 1;
		goto ged_start;
	}

	PathRemoveFileSpec(buffer);
	string path = buffer;
	free(buffer);

	return path;
#else
	// first, we want to make sure we did get a valid argument
	if (argv_0 == NULL || argv_0[0] == 0)
	{
		return "";
	}

	string path;

	// if the program name starts with an '/', there is an absolute path specified
	if (argv_0[0] == '/')
	{
		path = argv_0;
		
		string::size_type pos = path.find_last_of("/");
		if (pos != -1)
		{
			path.resize(pos+1);  // keep the slash at the end, it's safer.
		}
	}

	// otherwise we use the current working directory or an additional relative path
	else
	{
		char cwd[PATH_MAX+1];
		ARZeroMemory(cwd);

		if (getcwd(cwd, PATH_MAX) == NULL)
		{
			// error while getting current working directory
			return "";
		}
		path = cwd;

		// ok, do we have some relative path informations?
		if (strchr(argv_0, '/') != NULL)
		{
			if (path.length() > 0 && path[path.length()-1] != '/')
			{
				path += '/';
			}

			path += argv_0;
			string::size_type pos = path.find_last_of("/");
			if (pos != -1)
			{
				path.resize(pos+1);  // keep the slash at the end, it's safer.
			}
		}
	}
	return path;
#endif
}

#if WIN32
string FileSystemUtil::GetFormattedMessage(unsigned int error)
{
	LPSTR buffer = NULL;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, buffer, 200, NULL) == 0)
	{
		return "failed to get error message";
	}
	string errMessage = buffer;
	LocalFree(buffer);
	return errMessage;
}
#endif
