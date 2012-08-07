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

#include "Main.h"
#include "ARInside.h"
#include "ConfigFile.h"
#include "tclap/CmdLine.h"
#include "AppException.h"
#include "WindowsUtil.h"
#include "util/CommandLineValidator.h"

using namespace TCLAP;

// this variable counts all generated files and as such is defined globally and referenced in other files
// TODO: remote this global dependency
int nFilesCreated;

void CMain::LoadConfigFile(string fileName, AppConfig &cfg)
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

bool CMain::IsDots(const char* str) 
{
	if(strcmp(str,".") && strcmp(str,"..")) return false;
	return true;
}

bool CMain::DeleteDirectory(const char* sPath, bool topLevel)
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

int CMain::Run(int argc, char* argv[])
{
	mTimer.StartTimer();
	int result = AR_RETURN_ERROR;

	OutputARInsideBanner();

	try
	{
		CommandLineValidator cmdLineValidator(argc, argv);
		if (!cmdLineValidator.Validate())
			return cmdLineValidator.GetExitCode();

		string settingsIni = cmdLineValidator.GetIniFilename();

		if(CWindowsUtil::FileExistsAndReadable(settingsIni))
		{
			stringstream strm;
			strm << "File '" << settingsIni.c_str() << "' not found.";
			throw(AppException(strm.str(), "undefined", "FileNotFound"));
		}

		//Load application settings
		AppConfig appConfig;
		LoadConfigFile(settingsIni, appConfig);

		// validate throws exceptions if configuration isnt valid
		appConfig.Validate(cmdLineValidator);
		
		CWindowsUtil winUtil(appConfig);		
		CARInside arInside(appConfig);

		//Delete existing files
		if(appConfig.bDeleteExistingFiles)
		{
			cout << "Deleting existing files" << endl;
			if (!DeleteDirectory(appConfig.targetFolder.c_str(), true))
				cout << "Deletion failed!" << endl;			
		}

		//Create the target directory specified in the configuration files
		//TODO: CreateAppDirectory returns false if the directory already exists .. this should be changed so
		//      we can check if there is something going wrong and stop the process!!
		winUtil.CreateAppDirectory();

		if(arInside.ValidateTargetDir(appConfig.targetFolder) == 0)
		{
			nFilesCreated = 0;

			if(appConfig.bCompactFolder) // Compact folder
			{
#ifdef WIN32
				string compactCmd = "compact /C /I /Q /S:" + appConfig.targetFolder;
				WinExec(compactCmd.c_str(), SW_SHOWNORMAL);
#else
				cout << "[WARN] CompactFolder is only supported on windows platform!" << endl;
#endif
			}

			if(!appConfig.fileMode)	//ServerMode
			{
				if(arInside.Init(appConfig.userName, appConfig.password, appConfig.serverName, appConfig.tcpPort, appConfig.rpcPort) == AR_RETURN_OK)
				{
					arInside.DoWork(0);
					result = AR_RETURN_OK;
				}
			}
			else //FileMode
			{
				if(!appConfig.objListXML.empty())
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
						cout << "Could not find file '" << appConfig.objListXML << "'" << endl;
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
			// TODO: ValidateTargetDir returns 0 on success. On error there is no information about the error
			// at all .. only "whoops, cant create directory" .. hell yes, and now? where is my glass sphere...
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
		cout << endl << "AppException: " << e.typeDescription() << endl << "Description: " << e.error() << endl;	
	}
	catch (ArgException &e)
	{ 
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		return AR_RETURN_ERROR;
	}
	catch (ExitException &ee) 
	{
		exit(ee.getExitStatus());
	}
	catch(exception &e)
	{
		cout << endl << "Unhandled execption in _main: " << e.what() << endl;
	}

	return result;
}

char* CMain::GetPlatformString()
{
	char* platform;
	if (sizeof(platform) == 4)
		return "32bit";
	if (sizeof(platform) == 8)
		return "64bit";
	return "";
}

void CMain::OutputARInsideBanner()
{
	cout << "ARInside Version " << AppVersion << " " << GetPlatformString() << endl << endl;
}
