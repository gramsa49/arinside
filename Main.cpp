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
bool DeleteDirectory(const char* sPath);


int main(int argc, char* argv[])
{
	CAppTimer mTimer;
	mTimer.StartTimer();
	int result = AR_RETURN_ERROR;

	string server, login, pwd, settingsIni, output;
	int tcp = 0;
	int rpc = 0;

	char* platform = "";
	if (sizeof(platform) == 4)
		platform = "32bit";
	if (sizeof(platform) == 8)
		platform = "64bit";

	cout << "ARInside Version " << AppVersion << " " << platform << endl << endl;

	CmdLine cmd("ARInside -- http://arinside.org\n"
		          "Copyright (C) 2011 Stefan Nerlich, LJ Longwing, John Luthgers\n"
							"This program comes with ABSOLUTELY NO WARRANTY, is free software, and you are welcome to "
							"redistribute it under certain conditions; see COPYING file for more details.", 
							' ', AppVersion);

	ValueArg<string> iniArg("i", "ini", "Application settings filename", true, "settings.ini", "string");
	ValueArg<string> serverArg("s","server","ARSystem server",false,"","string");
	ValueArg<string> loginArg("l", "login", "Login name", false, "", "string");
	ValueArg<string> pwdArg("p", "pwd", "Password", false, "", "string");
	ValueArg<int> tcpArg("t", "tcp", "Tcp port", false, 0, "int");
	ValueArg<int> rpcArg("r", "rcp", "Rcp port", false, 0, "int");
	ValueArg<string> outputFolder("o", "output", "Output folder", false, "", "string");
	SwitchArg verboseArg("v","verbose","Verbose Output",false);
#if ARINSIDE_TEST_SUPPORT
	SwitchArg testModeArg("","test", "", false);
#endif	
	try 
	{  
#if ARINSIDE_TEST_SUPPORT
		cmd.add(testModeArg);
#endif

		// add it in reverse order. the "--help" output lists the arguments from the last to the first added.
		cmd.add(verboseArg);
		cmd.add(rpcArg);
		cmd.add(tcpArg);
		cmd.add(pwdArg);
		cmd.add(loginArg);
		cmd.add(outputFolder);
		cmd.add(serverArg);
		cmd.add(iniArg);
		cmd.parse( argc, argv );

		server = serverArg.getValue();
		output = outputFolder.getValue();
		login = loginArg.getValue();
		pwd = pwdArg.getValue();
		tcp = tcpArg.getValue();
		rpc = rpcArg.getValue();
		settingsIni = iniArg.getValue();
		AppConfig::verboseMode = verboseArg.getValue();

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

		// override settings with values specified by the command line 
		if (serverArg.isSet())
			appConfig.serverName = server;

		if (outputFolder.isSet())
			appConfig.targetFolder = output;

		if (loginArg.isSet())
			appConfig.userName = login;

		if (pwdArg.isSet())
			appConfig.password = pwd;

		if (tcpArg.isSet())
			appConfig.tcpPort = tcp;

		if (rpcArg.isSet())
			appConfig.rpcPort = rpc;

#if ARINSIDE_TEST_SUPPORT
		appConfig.testMode = testModeArg.getValue();
#endif

		// special checks for server mode
		if (!appConfig.fileMode) 
		{
			string missingArgs;
			unsigned int missingCount = 0;

			if (!appConfig.fileMode && appConfig.serverName.empty())
			{
				missingCount++;
				missingArgs = "server / ServerName";
			}
			if (appConfig.userName.empty())
			{
				missingCount++;
				if (!missingArgs.empty()) missingArgs += ", ";
				missingArgs += "login / Username";
			}

			if (!missingArgs.empty())
			{
				string msg;
				StdOutput _output;
				msg = "Required argument(s) missing: " + missingArgs;
				
				cout << endl;
				CmdLineParseException parseErr(msg);
				_output.failure(cmd, parseErr);
			}
		}

		// validate the path of the target folder
		if (appConfig.targetFolder.empty())
		{
			cout << "[ERR] Target folder setting is missing or not setup correctly!" << endl;
			throw ExitException(1);
		}
		
		string fullOutputPath = CWindowsUtil::GetRealPathName(appConfig.targetFolder);
		if (CUtil::StrEndsWith(fullOutputPath, ":\\") || CUtil::StrEndsWith(fullOutputPath, ":/") || fullOutputPath == "/")
		{
			cout << "[ERR] The target directory points to the root of the device. This is not allowed!" << endl;
			throw ExitException(1);
		}

		CWindowsUtil winUtil(appConfig);		
		CARInside arInside(appConfig);

		//Delete existing files
		if(appConfig.bDeleteExistingFiles)
		{
			cout << "Deleting existing files" << endl;
			DeleteDirectory(appConfig.targetFolder.c_str());
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

bool DeleteDirectory(const char* sPath)
{
#ifdef WIN32
	try
	{
		HANDLE hFind; // file handle
		WIN32_FIND_DATA FindFileData;

		char DirPath[MAX_PATH];
		char FileName[MAX_PATH];

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

	return (_rmdir(sPath)==0 ? true : false); // remove the empty (maybe not) directory
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
	if (!rmdir(sPath)) return false; // delete the directory
	return true;
#endif
}
