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
#endif
#include "ARInside.h"
#include "ConfigFile.h"
#include "tclap/CmdLine.h"
#include "AppException.h"
#include "WindowsUtil.h"
#ifdef WIN32 // JLS17 LINUX
#include <io.h>
#endif
#include <sys/stat.h>

using namespace TCLAP;

const string AppName = "ARInside";
void LoadConfigFile(string fileName, AppConfig &cfg);
string LoadFromFile(string fileName);
int nFilesCreated;

BOOL IsDots(const TCHAR* str);
BOOL DeleteDirectory(const TCHAR* sPath);


int _tmain(int argc, _TCHAR* argv[])
{
	CAppTimer mTimer;
	mTimer.StartTimer();
	int result = AR_RETURN_ERROR;

	string server, login, pwd, settingsIni;
	int tcp = 0;
	int rpc = 0;

	try 
	{  
		cout << "ARInside Version " << AppVersion << endl;
		cout << "Copyright (c) 2009 Stefan Nerlich" << endl << endl;

		CmdLine cmd("ARInside command arguments", ' ', AppVersion);

		ValueArg<string> serverArg("s","server","ARSystem server",true,"servername","string");
		cmd.add(serverArg);

		ValueArg<string> loginArg("l", "login", "Login name", true, "Demo", "string");
		cmd.add(loginArg);

		ValueArg<string> pwdArg("p", "pwd", "Password", false, "", "string");
		cmd.add(pwdArg);

		ValueArg<string> iniArg("i", "ini", "Application settings filename", true, "settings.ini", "string");
		cmd.add(iniArg);

		ValueArg<int> tcpArg("t", "tcp", "Tcp port", false, 0, "int");
		cmd.add(tcpArg);

		ValueArg<int> rpcArg("r", "rcp", "Rcp port", false, 0, "int");
		cmd.add(rpcArg);

		SwitchArg verboseArg("v","verbose","Verbose Output",false);
		cmd.add(verboseArg);

		cmd.parse( argc, argv );
		server = serverArg.getValue();
		login = loginArg.getValue();
		pwd = pwdArg.getValue();
		tcp = tcpArg.getValue();
		rpc = rpcArg.getValue();
		settingsIni = iniArg.getValue();
		AppConfig::verboseMode = verboseArg.getValue();
	} 
	catch (ArgException &e)
	{ 
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		return AR_RETURN_ERROR;
	}

	try
	{
		std::ifstream in(settingsIni.c_str());		
		if(!in)
		{
			stringstream strm;
			strm << "File '" << settingsIni.c_str() << "' not found.";
			throw(AppException(strm.str(), "undefined", "FileNotFound"));
		}

		//Load application settings
		AppConfig appConfig;
		LoadConfigFile(settingsIni, appConfig);

		appConfig.serverName = server;

		CWindowsUtil winUtil(appConfig);		
		CARInside arInside(appConfig);;

		//Delete existing files
		if(appConfig.bDeleteExistingFiles)
		{
			cout << "Deleting existing files" << endl;
			DeleteDirectory(appConfig.targetFolder.c_str());
		}

		//Create the target directory specified in the configuration files
		winUtil.CreateAppDirectory();

		if(arInside.ValidateTargetDir(appConfig.targetFolder) == 0)
		{
			nFilesCreated = 0;

			if(appConfig.bCompactFolder) // Compact folder
			{
				string compactCmd = "compact /C /I /Q /S:" + appConfig.targetFolder;
				WinExec(compactCmd.c_str(), SW_SHOWNORMAL);

				//ShellExecute(NULL, compactCmd.c_str(), "", "", "", SW_SHOWNORMAL);
			}

			if(!appConfig.fileMode)	//ServerMode
			{
				if(arInside.Init(login, pwd, server, tcp, rpc) == AR_RETURN_OK)
				{
					arInside.DoWork(0);
					result = AR_RETURN_OK;
				}
			}
			else //FileMode
			{
				if(appConfig.objListXML.length() > 0)
				{		
					if(arInside.FileExists(appConfig.objListXML) == true)
					{
						if(arInside.Init("", "", "", 0, 0) == AR_RETURN_OK)
						{
							arInside.DoWork(1);
							result = AR_RETURN_OK;
						}
					}
					else
					{
						cout << "Couldt not find file '" << appConfig.objListXML << "'" << endl;
						result = AR_RETURN_ERROR;
					}
				}
				else
				{
					cout << "No 'ObjListXML' file specified in the application configuration file!" << endl;
					result = AR_RETURN_ERROR;
				}
			}
		}			
		else
		{
			cout << "Failed to create target directory: " << appConfig.targetFolder << endl;
			result = AR_RETURN_ERROR;
		}

		arInside.Terminate();		

		if(result == AR_RETURN_OK)
		{
			mTimer.EndTimer();
			float nTotalDuration = mTimer.GetDuration();

			cout << endl << nFilesCreated << " files created in " << nTotalDuration << " seconds." << endl;
			//cout << endl << nFilesCreated << " files created in " << arInside.nDurationLoad + arInside.nDurationDocumentation << " seconds." << endl;
			cout << "Documentation successfully saved. The application will now exit." << endl;
			result =  AR_RETURN_OK;		
		}
		else
		{
			cout << "Application terminated." << endl;
		}

	}
	catch(AppException &e)
	{
		cout << endl << "AppException: " << e.typeDescription() << endl << "Description: " << e.error();	
	}

	return result;
}

void LoadConfigFile(string fileName, AppConfig &cfg)
{
	try
	{
		cout << endl << "Load application configuration settings: '" << fileName << "' ";
		ConfigFile config(fileName);
		config.readInto(cfg.userForm, "UserForm");
		config.readInto(cfg.userQuery, "UserQuery");
		config.readInto(cfg.groupForm, "GroupForm");
		config.readInto(cfg.groupQuery, "GroupQuery");
		config.readInto(cfg.roleForm, "RoleForm");
		config.readInto(cfg.roleQuery, "RoleQuery");
		config.readInto(cfg.maxRetrieve, "MaxRetrieve");
		config.readInto(cfg.companyName, "CompanyName");
		config.readInto(cfg.companyUrl, "CompanyUrl");
		config.readInto(cfg.targetFolder, "TargetFolder");
		config.readInto(cfg.fileMode, "FileMode");
		config.readInto(cfg.objListXML, "ObjListXML");
		config.readInto(cfg.blackList, "BlackList");
		config.readInto(cfg.bLoadServerInfoList, "LoadServerInfoList");
		config.readInto(cfg.bLoadUserList, "LoadUserList");
		config.readInto(cfg.bLoadGroupList, "LoadGroupList");
		config.readInto(cfg.bLoadRoleList, "LoadRoleList");
		config.readInto(cfg.bUseUtf8, "Utf-8");
		config.readInto(cfg.bCompactFolder, "CompactFolder");
		config.readInto(cfg.bDeleteExistingFiles, "DeleteExistingFiles");
		config.readInto(cfg.runNotes, "RunNotes");
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
		LOG << "BlackList: " << cfg.blackList << endl;
		LOG << "LoadServerInfoList: " << cfg.bLoadServerInfoList << endl;
		LOG << "LoadUserList: " << cfg.bLoadUserList << endl;
		LOG << "LoadGroupList: " << cfg.bLoadGroupList << endl;
		LOG << "LoadRoleList: " << cfg.bLoadRoleList << endl;
		LOG << "Utf-8: " << cfg.bUseUtf8 << endl;
		LOG << "CompactFolder: " << cfg.bCompactFolder << endl;
		LOG << "DeleteExistingFiles: " << cfg.bDeleteExistingFiles << endl;
		LOG << "RunNotes: " << cfg.runNotes << endl;
		LOG << endl;		
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

BOOL IsDots(const TCHAR* str) 
{
	if(_tcscmp(str,".") && _tcscmp(str,"..")) return FALSE;
	return TRUE;
}

BOOL DeleteDirectory(const TCHAR* sPath)
{
	try
	{
		HANDLE hFind; // file handle
		WIN32_FIND_DATA FindFileData;

		TCHAR DirPath[MAX_PATH];
		TCHAR FileName[MAX_PATH];

		_tcscpy(DirPath,sPath);
		_tcscat(DirPath,_T("\\"));
		_tcscpy(FileName,sPath);
		_tcscat(FileName,_T("\\*")); // searching all files

		hFind = FindFirstFile(FileName, &FindFileData); // find the first file
		if( hFind != INVALID_HANDLE_VALUE )
		{
			do
			{
				if( IsDots(FindFileData.cFileName))
					continue;

				_tcscpy(FileName + _tcslen(DirPath), FindFileData.cFileName);
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

	return RemoveDirectory(sPath); // remove the empty (maybe not) directory
}
