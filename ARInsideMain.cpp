//Copyright (C) 2012 John Luthgers | jls17
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
//    along with ARInside. If not, see <http://www.gnu.org/licenses/>.

#include "ARInsideMain.h"
#include "ARInside.h"
#include "ConfigFile.h"
#include "AppException.h"
#include "WindowsUtil.h"

using namespace TCLAP;

extern string AppName = "ARInside";
void LoadConfigFile(string fileName, AppConfig &cfg);
string LoadFromFile(string fileName);
extern int nFilesCreated;

bool IsDots(const char* str);
bool DeleteDirectory(const char* sPath, bool topLevel = false);

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
		          "Copyright (C) 2012 Stefan Nerlich, LJ Longwing, John Luthgers\n"
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
	SwitchArg slowArg("", "slow", "Uses slow object loading", false);
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
		cmd.add(slowArg);
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

		appConfig.slowObjectLoading = slowArg.getValue();

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
