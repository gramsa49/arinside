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
#ifdef WIN32 // JLS17 LINUX
#include <windows.h>
#include <io.h>
#include <direct.h>
#else // WIN32
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif // WIN32

#include "ARInside.h"
#include "ConfigFile.h"
#include "tclap/CmdLine.h"
#include "AppException.h"
#include "WindowsUtil.h"

#include <sys/stat.h>

using namespace TCLAP;

const string AppName = "ARInside";
void LoadConfigFile(string fileName, AppConfig &cfg);
string LoadFromFile(string fileName);
int nFilesCreated;

bool IsDots(const char* str);
bool DeleteDirectory(const char* sPath, bool topLevel = false);

void LoadConfigFile(string fileName, AppConfig &cfg)
{
	try
	{
		cout << endl << "Load application configuration settings: '" << fileName << "' ";
		ConfigFile config(fileName);
		config.readInto<string>(cfg.userForm, "UserForm", "User");
		config.readInto<string>(cfg.userQuery, "UserQuery", "1=1");
		config.readInto<string>(cfg.groupForm, "GroupForm", "Group");
		config.readInto<string>(cfg.groupQuery, "GroupQuery", "1=1");
		config.readInto<string>(cfg.roleForm, "RoleForm", "Roles");
		config.readInto<string>(cfg.roleQuery, "RoleQuery", "1=1");
		config.readInto<int>(cfg.maxRetrieve, "MaxRetrieve", 0);
		config.readInto<string>(cfg.companyName, "CompanyName", "");
		config.readInto<string>(cfg.companyUrl, "CompanyUrl", "");
		config.readInto<string>(cfg.targetFolder, "TargetFolder", "");
		config.readInto<bool>(cfg.fileMode, "FileMode", false);
		config.readInto<string>(cfg.objListXML, "ObjListXML", "");
		config.readInto<bool>(cfg.oldNaming, "OldNaming", false);
		config.readInto<string>(cfg.blackList, "BlackList", "");
		config.readInto<bool>(cfg.bLoadServerInfoList, "LoadServerInfoList", true);
		config.readInto<bool>(cfg.bLoadUserList, "LoadUserList", true);
		config.readInto<bool>(cfg.bLoadGroupList, "LoadGroupList", true);
		config.readInto<bool>(cfg.bLoadRoleList, "LoadRoleList", true);
		config.readInto<bool>(cfg.bUseUtf8, "Utf-8", false);
		config.readInto<bool>(cfg.bCompactFolder, "CompactFolder", false);
		config.readInto<bool>(cfg.bGZCompression, "GZCompression", false);
		config.readInto<bool>(cfg.bDeleteExistingFiles, "DeleteExistingFiles", false);
		config.readInto<string>(cfg.runNotes, "RunNotes", "");
		config.readInto<string>(cfg.serverName, "ServerName", "");
		config.readInto<int>(cfg.tcpPort, "TCPPort", 0);
		config.readInto<int>(cfg.rpcPort, "RPCPort", 0);
		config.readInto<string>(cfg.userName, "Username", "");
		config.readInto<string>(cfg.password, "Password", "");
		config.readInto<int>(cfg.apiTimeout, "APITimeout", 0);
		config.readInto<string>(cfg.overlayMode, "OverlayMode", "TRUE");
		cout << endl;

		LOG << "UserForm: " << cfg.userForm << endl;		
		LOG << "UserQuery: " << cfg.userQuery << endl;
		LOG << "GroupForm: " << cfg.groupForm << endl;
		LOG << "GroupQuery: " << cfg.groupQuery << endl;
		LOG << "RoleForm: " << cfg.roleForm << endl;
		LOG << "RoleQuery: " << cfg.roleQuery << endl;
		LOG << "MaxRetrieve: " << cfg.maxRetrieve << endl;
		LOG << "CompanyName: " << cfg.companyName << endl;
		LOG << "CompanyUrl: " << cfg.companyUrl << endl;
		LOG << "TargetFolder: " << cfg.targetFolder << endl;
		LOG << "FileMode: " << cfg.fileMode << endl;
		LOG << "ObjListXML: " << cfg.objListXML << endl;
		LOG << "OldNaming: " << cfg.oldNaming << endl;
		LOG << "BlackList: " << cfg.blackList << endl;
		LOG << "LoadServerInfoList: " << cfg.bLoadServerInfoList << endl;
		LOG << "LoadUserList: " << cfg.bLoadUserList << endl;
		LOG << "LoadGroupList: " << cfg.bLoadGroupList << endl;
		LOG << "LoadRoleList: " << cfg.bLoadRoleList << endl;
		LOG << "Utf-8: " << cfg.bUseUtf8 << endl;
		LOG << "CompactFolder: " << cfg.bCompactFolder << endl;
		LOG << "GZCompression: " << cfg.bGZCompression << endl;
		LOG << "DeleteExistingFiles: " << cfg.bDeleteExistingFiles << endl;
		LOG << "RunNotes: " << cfg.runNotes << endl;
		LOG << endl;

		if (cfg.bGZCompression)
			CWebUtil::webpageFileExtension = CWebUtil::HtmlGZPageSuffix();
		else
			CWebUtil::webpageFileExtension = CWebUtil::HtmlPageSuffix();

		// later we can support configuring specific layer documentation
		if (cfg.overlayMode != "FALSE" && cfg.overlayMode != "TRUE")
		{
			cfg.overlayMode = "TRUE";
		}

		// should overlay support be disable completely?
		if (cfg.overlayMode == "FALSE")
			cfg.bOverlaySupport = false;
		else
			cfg.bOverlaySupport = true;

	}
	catch(exception& e)
	{
		throw(AppException(e.what(), "Error in LoadConfigFile().", "AppInitialization"));
	}
}

string LoadFromFile(string fileName)
{
	string result = "";

	try
	{
		stringstream strm;			
		ifstream fin(fileName.c_str());

		if( !fin ) 
		{
			strm << "Error loading file '"<< fileName <<"'." << endl; 
			throw(AppException(strm.str(), "undefined", "AppInitialization"));
			return 0;
		}

		string tmp;
		while( getline(fin, tmp))
			strm << tmp << endl;
		fin.close();
		result = strm.str();
	}
	catch(exception& e)
	{
		stringstream strm;
		strm << "Error loading file '"<< fileName <<"'. Error: " << e.what() << endl; 
		throw(AppException(strm.str(), "undefined", "AppInitialization"));
	}	

	return result;
}

bool IsDots(const char* str) 
{
	if(strcmp(str,".") && strcmp(str,"..")) return false;
	return true;
}

bool DeleteDirectory(const char* sPath, bool topLevel)
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

					if( !DeleteDirectory(FileName) )
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
				DeleteDirectory(file.c_str());
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
