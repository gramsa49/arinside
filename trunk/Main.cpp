
/****************************************************************************** 
 * 
 *  file:  Main.cpp
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

#include "stdafx.h"
#include <windows.h>
#include "arinside.h"
#include "configfile.h"
#include "tclap\CmdLine.h"
#include "appexception.h"
#include "windowsutil.h"
#include <io.h>
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
	int result = AR_RETURN_ERROR;

	string server, login, pwd, settingsIni;
	int tcp = 0;
	int rpc = 0;

	try 
	{  
		cout << "ARInside Version " << AppVersion << endl;
		cout << "Copyright (c) 2006-2007 Stefan Nerlich" << endl << endl;

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

		cmd.parse( argc, argv );
		server = serverArg.getValue();
		login = loginArg.getValue();
		pwd = pwdArg.getValue();
		tcp = tcpArg.getValue();
		rpc = rpcArg.getValue();
		settingsIni = iniArg.getValue();

	} 
	catch (ArgException &e)
	{ 
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
		return AR_RETURN_ERROR;
	}

	try
	{
		cout << "ARInside Version" << AppVersion << endl;
		
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
					arInside.Prepare();
					winUtil.Load();
					arInside.LoadServerObjects(0);			
					arInside.Documentation();
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
							arInside.Prepare();
							winUtil.Load();
							arInside.LoadServerObjects(1);
							arInside.Documentation();
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
			cout << endl << nFilesCreated << " files created in " << arInside.nDurationLoad + arInside.nDurationDocumentation << " seconds." << endl;
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
		cout << endl;
		
		cout << "UserForm: " << cfg.userForm << endl;		
		cout << "UserQuery: " << cfg.userQuery << endl;
		cout << "GroupForm: " << cfg.groupForm << endl;
		cout << "GroupQuery: " << cfg.groupQuery << endl;
		cout << "RoleForm: " << cfg.roleForm << endl;
		cout << "RoleQuery: " << cfg.roleQuery << endl;
		cout << "MaxRetrieve: " << cfg.maxRetrieve << endl;
		cout << "CompanyName: " << cfg.companyName << endl;
		cout << "CompanyUrl: " << cfg.companyUrl << endl;
		cout << "TargetFolder: " << cfg.targetFolder << endl;
		cout << "FileMode: " << cfg.fileMode << endl;
		cout << "ObjListXML: " << cfg.objListXML << endl;
		cout << "BlackList: " << cfg.blackList << endl;
		cout << "LoadServerInfoList: " << cfg.bLoadServerInfoList << endl;
		cout << "LoadUserList: " << cfg.bLoadUserList << endl;
		cout << "LoadGroupList: " << cfg.bLoadGroupList << endl;
		cout << "LoadRoleList: " << cfg.bLoadRoleList << endl;
		cout << "Utf-8: " << cfg.bUseUtf8 << endl;
		cout << "CompactFolder: " << cfg.bCompactFolder << endl;
		cout << "DeleteExistingFiles: " << cfg.bDeleteExistingFiles << endl;
		cout << endl;		
	}
	catch(...)
	{
		throw(AppException("Error in LoadConfigFile().", "undefined", "AppInitialization"));
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
	catch(...)
	{
		stringstream strm;
		strm << "Error loading file '"<< fileName <<"'." << endl; 
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
				if( IsDots(FindFileData.cFileName) 
			)
			continue;

			_tcscpy(FileName + _tcslen(DirPath), FindFileData.cFileName);
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// we have found a directory, recurse
				cout << "Delete " << FileName << endl;

				if( !DeleteDirectory(FileName) )
				break; // directory couldn't be deleted
			}
			else
			{
				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					_chmod(FileName, _S_IWRITE); // change read-only file mode

				cout << "Delete " << FileName << endl;
				if( !DeleteFile(FileName) )
					break; // file couldn't be deleted
			}
		}

		while( FindNextFile(hFind,&FindFileData) );	
		FindClose(hFind); // closing file handle
		
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in DeleteDirectory" << endl; 
	}	

	return RemoveDirectory(sPath); // remove the empty (maybe not) directory
}