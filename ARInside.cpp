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
#include "ARInside.h"

#include "core/ChangeHistoryEntry.h"

#include "doc/DocMain.h"
#include "doc/DocUserDetails.h"
#include "doc/DocGroupDetails.h"
#include "doc/DocSchemaDetails.h"
#include "doc/DocAlDetails.h"
#include "doc/DocFilterDetails.h"
#include "doc/DocEscalationDetails.h"
#include "doc/DocCharMenuDetails.h"
#include "doc/DocFieldDetails.h"
#include "doc/DocVuiDetails.h"
#include "doc/DocWebserviceDetails.h"
#include "doc/DocPacklistDetails.h"
#include "doc/DocApplicationDetails.h"
#include "doc/DocAlGuideDetails.h"
#include "doc/DocFilterGuideDetails.h"
#include "doc/DocContainerHelper.h"
#include "doc/DocAnalyzer.h"
#include "doc/DocValidator.h"
#include "doc/DocRoleDetails.h"
#include "doc/DocSummaryInfo.h"
#include "doc/DocImageDetails.h"

#include "output/Table.h"
#include "output/WebUtil.h"
#include "output/NavigationPage.h"

#include "output/IFileStructure.h"
#include "output/FileNaming.h"

#include "scan/ScanMain.h"

/////////
// the following file is generated via a pre-build step using "svnrev_template.h" as template. 
// The $WCREV$ keyword of the template is replaced with the revision number.
#include "svnrev.h"

/////////
// version information block
#define VERSION "3.0.3"
#if defined(_DEBUG)
#define VERSION_STR VERSION "." SVN_REV_STR " Debug"
#elif defined(_ARINSIDE_BETA)
#define VERSION_STR VERSION "." SVN_REV_STR " Beta"
#else
#define VERSION_STR VERSION "." SVN_REV_STR
#endif
const string AppVersion = VERSION_STR;
/////////


using namespace OUTPUT;

// some kind of singleton pattern to keep compatibility
CARInside* CARInside::pInsideInstance = NULL;

CARInside::CARInside(AppConfig &appConfig)
{
	this->appConfig = appConfig;
	this->globalFieldList.clear();
	this->overlayMode = 1; // TODO: this is the default value for ars764. check for later versions

	this->nDurationLoad = 0;
	this->nDurationDocumentation = 0;

	this->vMajor = 0;
	this->vMinor = 0;
	this->vRevision = 0;
	this->arXmlVersion = 0;

	if (appConfig.oldNaming)
		SetFileNamingStrategy(new FileNaming::DefaultFileNamingStrategy());
	else
		SetFileNamingStrategy(new FileNaming::ObjectNameFileNamingStrategy());

	if (CARInside::pInsideInstance == NULL) 
		CARInside::pInsideInstance = this;

	ARZeroMemory(&emptyPropList); // init struct; see lists/ARListHelpers.h
}

CARInside::~CARInside(void)
{
	DestroyFileNamingStrategy();
}

int CARInside::Init(string user, string pw, string server, int port, int rpc)
{
	cout << endl << "Connecting to server " << server << "..." << endl;

	memset(&arControl, '\0', sizeof(arControl));
	memset(&arStatus, '\0', sizeof(arStatus));

	if(this->appConfig.bUseUtf8)
		strcpy(arControl.localeInfo.charSet, "UTF-8");

	strncpy(arControl.user, user.c_str(), AR_MAX_NAME_SIZE);
	arControl.user[AR_MAX_NAME_SIZE]='\0';
	strncpy(arControl.password, pw.c_str(), AR_MAX_PASSWORD_SIZE);
	arControl.password[AR_MAX_PASSWORD_SIZE]='\0';
	strncpy(arControl.server, server.c_str(), AR_MAX_SERVER_SIZE);
	arControl.server[AR_MAX_SERVER_SIZE]='\0';

	int nResult = ARInitialization(&this->arControl,&this->arStatus);
	if (nResult != AR_RETURN_OK)
	{
		cout << "Initilization of ARAPI returned: " << nResult << " (" << CAREnum::ActiveLinkMessageType(nResult) << ")" << endl;
		cout << GetARStatusError();
		return nResult;
	}

	if(server == "" && nResult == AR_RETURN_OK) // Filemode
	{
		return AR_RETURN_OK;
	}

	if ( nResult == AR_RETURN_OK)
	{
		if(port>0)
		{
			nResult = ARSetServerPort(&this->arControl, this->arControl.server, port, rpc, &this->arStatus);
			if (nResult != AR_RETURN_OK)
			{
				throw(AppException(GetARStatusError(), "undefined", "ARSystem"));
			}
		}

		if(this->appConfig.apiTimeout > 120) // at least 120 seconds api timeout
		{
			ARValueStruct val; 
			for (unsigned int valId = AR_SESS_TIMEOUT_NORMAL; valId <= AR_SESS_TIMEOUT_XLONG; ++valId)
			{
				ARZeroMemory(&val);
				nResult = ARGetSessionConfiguration(&this->arControl, valId, &val, &this->arStatus);

				// validate result
				if (nResult != AR_RETURN_OK) continue;	// ok, if this fails, dont bother .. next one
				if (val.dataType != AR_DATA_TYPE_INTEGER) continue;
				if (val.u.intVal > this->appConfig.apiTimeout) continue;

				// setup value
				val.dataType = AR_DATA_TYPE_INTEGER;
				val.u.intVal = this->appConfig.apiTimeout;

				// now configure session
				nResult = ARSetSessionConfiguration(&this->arControl, valId, &val, &this->arStatus);
				if (nResult != AR_RETURN_OK)
				{
					cout << "Setting session timeout failed: " << GetARStatusError();
				}
			}
		}

		if(nResult == AR_RETURN_OK)
		{
			ARBoolean isAdmin, isSubadmin, hasCustomize;
			nResult = ARVerifyUser(&this->arControl, &isAdmin, &isSubadmin, &hasCustomize, &this->arStatus);
			
			if (nResult != AR_RETURN_OK)
			{
				throw(AppException(GetARStatusError(), "undefined", "ARSystem"));
			}
			FreeARStatusList(&this->arStatus, false);

			serverInfoList.LoadAndGetValue(AR_SERVER_INFO_HOSTNAME, StoreTo(this->srvHostName));
			serverInfoList.LoadAndGetValue(AR_SERVER_INFO_FULL_HOSTNAME, StoreTo(this->srvFullHostName));
			serverInfoList.LoadAndGetValue(AR_SERVER_INFO_VERSION, StoreTo(this->arServerVersion));

			ParseVersionString(this->arServerVersion);

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
			if (CompareServerVersion(7,6,4) >= 0)
				serverInfoList.LoadAndGetValue(AR_SERVER_INFO_OVERLAY_MODE, StoreTo(this->overlayMode));
#endif
			cout << "User '" << this->arControl.user <<"' connected to server " << srvFullHostName << endl;

			blackList.LoadFromServer(appConfig.blackList);
		}
	}

	FreeARStatusList(&this->arStatus, false);
	return nResult;
}

int CARInside::Terminate(void)
{
	ARTermination(&this->arControl, &this->arStatus);
	FreeARStatusList(&this->arStatus, false);
	return 0;
}

string CARInside::GetARStatusError(ARStatusList* status)
{
	const unsigned int maxTypes = 3;		// max index of the following array
	const char* returnTypes[] = { "ARROK", "ARWARN", "ARERR", "" };
	stringstream strm;
	strm.str("");

	if (status == NULL || status->numItems == 0) return strm.str();

	if(status->statusList != NULL)
	{
		for (unsigned int i = 0; i < status->numItems; i++)
		{
			const char* typeStr = returnTypes[min(status->statusList[i].messageType,maxTypes)];
			strm << "[" << typeStr << " " << status->statusList[i].messageNum << "] ";
			strm << status->statusList[i].messageText << endl;
			if (status->statusList[i].appendedText != NULL) 
				strm << "  " << status->statusList[i].appendedText << endl;
		}
	}

	FreeARStatusList(status, false);
	return strm.str();
}

string CARInside::GetARStatusError()
{
	string errorText = GetARStatusError(&this->arStatus);
	return errorText;
}

int CARInside::ValidateTargetDir(string targetFolder)
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

bool CARInside::FileExists(string fName)
{
	bool result = false;

	try
	{
		bool flag = false;
		fstream fin;
		fin.open(fName.c_str(),ios::in);
		if( fin.is_open() )
		{
			cout << fName << " exists" << endl;
			result =true;
		}
		fin.close();

	}
	catch(exception& e)
	{
		cout << "EXCEPTION An error occured validating the target path: " << e.what() << endl;
	}

	return result;
}


void CARInside::Prepare(void)
{	
	CDocMain *docMain = new CDocMain();

	if( docMain->Index() == 1)
	{
		InitFileNamingStrategy();
		if (appConfig.bGZCompression)
			WriteHTAccess();
	}

	delete docMain;
}

bool CARInside::FieldreferenceExists(int schemaInsideId, int fieldInsideId, const CRefItem &refItem)
{
	CARField fld(schemaInsideId, fieldInsideId);
	if (!fld.Exists()) return false;

	return fld.ReferenceExists(refItem);
}

void CARInside::LoadServerObjects(int nMode)
{
	CAppTimer mTimer;
	mTimer.StartTimer();

	if(nMode==1)
	{
		this->LoadFromFile();
	}
	else
	{
		this->LoadFromServer();
	}
	mTimer.EndTimer();
	this->nDurationLoad = mTimer.GetDuration();
}

void CARInside::LoadFromFile(void)
{
	try
	{
		cout << endl << "Loading objects from file '" << appConfig.objListXML << "'" << endl;

		ARXMLInputDoc xmlInputDoc;
		xmlInputDoc.docType = AR_XML_DOC_FILE_NAME;
		xmlInputDoc.u.fileName = (char*)appConfig.objListXML.c_str();

		ARXMLParsedStream parsedStream;
		ARStructItemList parsedObjects;
		unsigned int xmlDocVersion = 0;

		if(ARParseXMLDocument(&this->arControl, 
			&xmlInputDoc,
			NULL,
			&parsedStream,
			&parsedObjects,
			NULL,
			&this->arStatus) == AR_RETURN_OK)
		{			
			cout << parsedObjects.numItems << " items loaded." << endl;

			unsigned int schemaCount = 0;
			unsigned int imagesCount = 0;
			unsigned int activelinkCount = 0;
			unsigned int filterCount = 0;
			unsigned int escalationCount = 0;
			unsigned int containerCount = 0;
			unsigned int menuCount = 0;

			for (unsigned int i=0; i < parsedObjects.numItems; ++i)
			{
				switch (parsedObjects.structItemList[i].type)
				{
				case AR_STRUCT_ITEM_XML_SCHEMA:
					++schemaCount;
					break;
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
					++activelinkCount;
					break;
				case AR_STRUCT_ITEM_XML_FILTER:
					++filterCount;
					break;
				case AR_STRUCT_ITEM_XML_ESCALATION:
					++escalationCount;
					break;
				case AR_STRUCT_ITEM_XML_CONTAINER:
					++containerCount;
					break;
				case AR_STRUCT_ITEM_XML_CHAR_MENU:
					++menuCount;
					break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
				case AR_STRUCT_ITEM_XML_IMAGE:
					++imagesCount; 
					break;
#endif
				}
			}

			if (schemaCount > 0) schemaList.Reserve(schemaCount);
			if (activelinkCount > 0) alList.Reserve(activelinkCount);
			if (filterCount > 0) filterList.Reserve(filterCount);
			if (escalationCount > 0) escalationList.Reserve(escalationCount);
			if (containerCount > 0) containerList.Reserve(containerCount);
			if (menuCount > 0) menuList.Reserve(menuCount);
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
			if (imagesCount > 0) imageList.Reserve(imagesCount);
#endif

			for(unsigned int i=0; i< parsedObjects.numItems; i++)
			{
				switch(parsedObjects.structItemList[i].type)
				{						
				case AR_STRUCT_ITEM_XML_FILTER:
					{
						LOG << "Loading Filter: " << parsedObjects.structItemList[i].name; 

						int objInsideId = filterList.AddFilterFromXML(parsedStream, parsedObjects.structItemList[i].name, &xmlDocVersion);

						if (objInsideId > -1)
						{
							ParseVersionString(xmlDocVersion);
							LOG << " (InsideID: " << objInsideId << ") [OK]" << endl;
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_SCHEMA:
					{
						LOG << "Loading Form: " << parsedObjects.structItemList[i].name;

						int objInsideId = schemaList.AddSchemaFromXML(parsedStream, parsedObjects.structItemList[i].name, &xmlDocVersion);

						if (objInsideId > -1)
						{
							ParseVersionString(xmlDocVersion);
							LOG << " (InsideID: " << objInsideId << ") [OK]" << endl;
						}
					}
					break;					
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
					{
						LOG << "Loading ActiveLink: " << parsedObjects.structItemList[i].name; 

						int objInsideId = alList.AddActiveLinkFromXML(parsedStream, parsedObjects.structItemList[i].name, &xmlDocVersion);

						if (objInsideId > -1)
						{
							ParseVersionString(xmlDocVersion);
							LOG << " (InsideID: " << objInsideId << ") [OK]" << endl;
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_CHAR_MENU:
					{
						LOG << "Loading CharMenu: " << parsedObjects.structItemList[i].name; 

						int objInsideId = menuList.AddMenuFromXML(parsedStream, parsedObjects.structItemList[i].name, &xmlDocVersion);

						if (objInsideId > -1)
						{
							ParseVersionString(xmlDocVersion);
							LOG << " (InsideID: " << objInsideId << ") [OK]" << endl;
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_ESCALATION:
					{
						LOG << "Loading Escalation: " << parsedObjects.structItemList[i].name; 

						int objInsideId = escalationList.AddEscalationFromXML(parsedStream, parsedObjects.structItemList[i].name, &xmlDocVersion);

						if (objInsideId > -1)
						{
							ParseVersionString(xmlDocVersion);
							LOG << " (InsideID: " << objInsideId << ") [OK]" << endl;
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_CONTAINER:
					{
						LOG << "Loading Container: " << parsedObjects.structItemList[i].name; 

						int objInsideId = containerList.AddContainerFromXML(parsedStream, parsedObjects.structItemList[i].name, &xmlDocVersion);

						if (objInsideId > -1)
						{
							ParseVersionString(xmlDocVersion);
							LOG << " (InsideID: " << objInsideId << ") [OK]" << endl;
						}
					}
					break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
				case AR_STRUCT_ITEM_XML_IMAGE:
					{
						LOG << "Loading Image: " << parsedObjects.structItemList[i].name;
						
						int imageInsideId = imageList.AddImageFromXML(parsedStream, parsedObjects.structItemList[i].name);
						
						// dont know why bmc has decided to remove the arDocVersion parameter from the
						// ARGetImageFromXML api call. Now in case the xml-file contains only images we 
						// dont have a version at all. So we set it to version 7.5 by default. if other
						// objects are present they will overwrite this version if it is a 7.5+ version.
						ParseVersionString(AR_XML_VERSION_750);

						if (imageInsideId > -1)
						{
							LOG << " (InsideID: " << imageInsideId << ") [OK]" << endl;
						}
					}
					break;
#endif
#if _DEBUG
				default:
					cout << "Unused object type: [" << parsedObjects.structItemList[i].type << "] " << parsedObjects.structItemList[i].name << endl;
					break;
#endif
				}	
			}		
			
			schemaList.Sort();
			alList.Sort();
			filterList.Sort();
			escalationList.Sort();
			containerList.Sort();
			menuList.Sort();
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
			imageList.Sort();
#endif
		}
		else
		{
			cout << "An error occured parsing the xml document '" << appConfig.objListXML << "'" << endl;
			cout << GetARStatusError();
		}
		if (!arServerVersion.empty())
			cout << "server version: " << arServerVersion << endl;

		FreeARXMLParsedStream(&parsedStream, false);
		FreeARStatusList(&this->arStatus, false);
	} 
	catch (...)
	{ 
		cerr << "EXCEPTION loading server objects from xml file." << endl; 
	}
}

void CARInside::LoadFromServer(void)
{
	cout << endl << "Loading objects from server '" << appConfig.serverName << "'" << endl;
	cout << "server version: " << arServerVersion << endl;

	//LoadServerInfoList	
	if(appConfig.bLoadServerInfoList)
	{
		cout << "Start loading Server Information:" << endl;
		serverInfoList.Load();
		cout << serverInfoList.GetCount() << " server settings loaded" << endl;
	}
	else
		cout << endl << "Loading Server Informations [SKIPPED]" << endl;

	SetupOverlaySupport();

	//LoadUserList
	if(appConfig.bLoadUserList)
	{
		cout << endl << "Start loading Users:" << endl;		
		userList.LoadFromServer();
		cout << userList.GetCount() << " Users loaded" << endl;
	}
	else
		cout << endl << "Loading Users [SKIPPED]" << endl;


	//LoadGroupList
	if(appConfig.bLoadGroupList)
	{
		cout << endl << "Start loading Groups:" << endl;		
		groupList.LoadFromServer();
		cout << groupList.GetCount() << " Groups loaded" << endl;
	}
	else
		cout << endl << "Loading Groups [SKIPPED]" << endl;

	//LoadRoleList
	if(appConfig.bLoadRoleList)
	{
		cout << endl << "Start loading Roles:" << endl;		
		roleList.LoadFromServer();
		cout << (unsigned int)roleList.GetCount() << " Roles loaded" << endl;
	}
	else
		cout << endl << "Loading Roles [SKIPPED]" << endl;

	//ActiveLinks		
	cout << endl << "Start loading Active Links:" << endl;
	int nResult = LoadActiveLinks();
	cout << nResult << " Active Links loaded" << endl;


	//Filters	
	cout << endl << "Start loading Filters:" << endl;
	nResult = LoadFilters();
	cout << nResult << " Filters loaded" << endl;

	//Container	
	cout << endl << "Start loading Containers:" << endl;
	nResult = LoadContainer();
	cout << nResult << " Containers loaded" << endl;

	//Escalations	
	cout << endl << "Start loading Escalations:" << endl;
	nResult = LoadEscalations();
	cout << nResult << " Escalations loaded" << endl;

	//CharMenus	
	cout << endl << "Start loading Menus:" << endl;
	nResult = LoadCharMenus();
	cout << nResult << " Menus loaded" << endl;

	//Images
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	if (CompareServerVersion(7,5) >= 0)
	{
		cout << endl << "Start loading Images:" << endl;
		nResult = LoadImages();
		cout << nResult << " Images loaded" << endl;
	}
#endif

	//Load schemas
	int insideId = 0;
	cout << endl << "Start loading Forms:" << endl;

	nResult = LoadForms();
	cout << nResult << " Forms loaded" << endl << endl;

	// TODO: if we want to keep the old output, we need to iterate the forms and count all types
	////Regular forms
	//nResult = LoadForms(AR_LIST_SCHEMA_REGULAR, insideId);
	//cout << nResult << " Regular Forms loaded" << endl;

	////Dialogs
	//nResult = LoadForms(AR_LIST_SCHEMA_DIALOG, insideId);
	//cout << nResult << " Dialog Forms loaded" << endl;

	////Join
	//nResult = LoadForms(AR_LIST_SCHEMA_JOIN, insideId);
	//cout << nResult << " Join Forms loaded" << endl;

	////Vendor
	//nResult = LoadForms(AR_LIST_SCHEMA_VENDOR, insideId);
	//cout << nResult << " Vendor Forms loaded" << endl;

	////View
	//nResult = LoadForms(AR_LIST_SCHEMA_VIEW, insideId);
	//cout << nResult << " View Forms loaded" << endl << endl;
}

int CARInside::LoadForms()
{
	try
	{
		schemaList.LoadFromServer();
		schemaList.Sort();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Forms: " << e.what() << endl;
	}

	return schemaList.GetCount();
}

int CARInside::LoadContainer(void)
{
	try
	{
		containerList.LoadFromServer();
		containerList.Sort();		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Container: " << e.what() << endl;
	}

	return containerList.GetCount();
}

int CARInside::LoadCharMenus(void)
{
	try
	{
		menuList.LoadFromServer();
		menuList.Sort();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Menus: " << e.what() << endl;
	}

	return menuList.GetCount();
}

int CARInside::LoadEscalations(void)
{
	try
	{
		escalationList.LoadFromServer();
		escalationList.Sort();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Escalations: " << e.what() << endl;
	}

	return escalationList.GetCount();
}

int CARInside::LoadFilters(void)
{
	try
	{
		filterList.LoadFromServer();
		filterList.Sort();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Filters: " << e.what() << endl;
	}

	return filterList.GetCount();
}

int CARInside::LoadActiveLinks(void)
{
	try
	{
		alList.LoadFromServer();
		alList.Sort();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading ActiveLinks: " << e.what() << endl;
	}
	return alList.GetCount();
}

void CARInside::Documentation(void)
{	
	CAppTimer mTimer;
	mTimer.StartTimer();

	string strValue = objectNameFirstCharLetters; // "abcdefghijklmnopqrstuvwxyz0123456789#";
	CDocMain *docMain = new CDocMain();

	//Server information
	docMain->ServerInfoList();

	// container object count per first char
	vector<int> algObjCount; algObjCount.resize(38);
	vector<int> appObjCount; appObjCount.resize(38);
	vector<int> pklObjCount; pklObjCount.resize(38);
	vector<int> ftgObjCount; ftgObjCount.resize(38);
	vector<int> wbsObjCount; wbsObjCount.resize(38);

	//ContainerList
	docMain->ContainerList(ARCON_GUIDE, "ContainerList (ActiveLinkGuide)", "*", algObjCount);
	docMain->ContainerList(ARCON_APP, "ContainerList (Application)", "*", appObjCount);
	docMain->ContainerList(ARCON_PACK, "ContainerList (PackingList)", "*", pklObjCount);
	docMain->ContainerList(ARCON_FILTER_GUIDE, "ContainerList (FilterGuide)", "*", ftgObjCount);
	docMain->ContainerList(ARCON_WEBSERVICE, "ContainerList (Webservice)", "*", wbsObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ContainerList(ARCON_GUIDE, "GuideList", std::string(1, strValue.at(i)), algObjCount);
		docMain->ContainerList(ARCON_APP, "ApplicationList", std::string(1, strValue.at(i)), appObjCount);
		docMain->ContainerList(ARCON_PACK, "PackinglistList", std::string(1, strValue.at(i)), pklObjCount);
		docMain->ContainerList(ARCON_FILTER_GUIDE, "FilterGuideList", std::string(1, strValue.at(i)), ftgObjCount);
		docMain->ContainerList(ARCON_WEBSERVICE, "WebserviceList", std::string(1, strValue.at(i)), wbsObjCount);
	}

	//Application Details
	int nTmpCnt = 1;

	//Create documentation here to fill objects applicationName reference information	
	cout << "Starting Container Documentation" << endl;

	unsigned int cntCount = this->containerList.GetCount();
	for ( unsigned int cntIndex = 0; cntIndex < cntCount; ++cntIndex )
	{
		CARContainer cont(cntIndex);

		switch(cont.GetType())
		{
		case ARCON_APP:
			{
				LOG << "Application [" << (cntIndex + 1) << "-" << cntCount << "] '" << cont.GetName() << "': ";
				CDocApplicationDetails appDetails(cont);
				appDetails.Documentation();
			}
			break;
		default:
			{
				// TODO: is this output really correct? All other container types are documented and LOGged within the next loop!
				LOG << "Container [" << (cntIndex + 1) << "-" << cntCount << "] '" << cont.GetName() << "' [OK]" << endl;
			}
			break;
		}

		nTmpCnt++;
	}

	unsigned int tmpCount = this->containerList.GetCount();
	for ( unsigned int cntIndex = 0; cntIndex < tmpCount; ++cntIndex )
	{
		CARContainer cont(cntIndex);
		switch(cont.GetType())
		{
		case ARCON_WEBSERVICE:
			{
				LOG << "Webservice [" << (cntIndex + 1) << "-" << tmpCount << "] '" << cont.GetName() << "': ";
				CDocWebserviceDetails wsDetails(cont);
				wsDetails.Documentation();
			}
			break;
		case ARCON_GUIDE:
			{
				LOG << "ActiveLinkGuide [" << (cntIndex + 1) << "-" << tmpCount << "] '" << cont.GetName() << "': ";
				CDocAlGuideDetails guideDetails(cont);
				guideDetails.Documentation();
			}
			break;
		case ARCON_FILTER_GUIDE:
			{
				LOG << "FilterGuide [" << (cntIndex + 1) << "-" << tmpCount << "] '" << cont.GetName() << "': ";
				CDocFilterGuideDetails fltGuideDetails(cont);
				fltGuideDetails.Documentation();
			}
			break;
		case ARCON_PACK:
			{
				LOG << "PackingList [" << (cntIndex + 1) << "-" << tmpCount << "] '" << cont.GetName() << "': ";
				CDocPacklistDetails packDetails(cont);
				packDetails.Documentation();
			}
			break;
		case ARCON_APP:
			{
				LOG << "Application [" << (cntIndex + 1) << "-" << tmpCount << "] '" << cont.GetName() << "' [OK]" << endl;
			}
			break;
		}

		nTmpCnt++;
	}

	// active link object count per first char
	vector<int> alObjCount; alObjCount.resize(38);

	//ActiveLink List
	docMain->ActiveLinkList("*", alObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ActiveLinkList(std::string(1, strValue.at(i)), alObjCount);
	}	

	docMain->ActiveLinkActionList();

	//ActiveLink Details
	tmpCount = alList.GetCount();
	cout << "Starting ActiveLink Documentation" << endl;
	for (unsigned int actlinkIndex = 0; actlinkIndex < tmpCount; ++actlinkIndex)
	{	
		LOG << "ActiveLink [" << actlinkIndex << "-" << nTmpCnt << "] '" << alList.ActiveLinkGetName(actlinkIndex) << "': ";
		CDocAlDetails alDetails(actlinkIndex);
		alDetails.Documentation();
	}


	// filter object count per first char
	vector<int> fltObjCount; fltObjCount.resize(38);

	//Filter List
	docMain->FilterList("*", fltObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->FilterList(std::string(1, strValue.at(i)), fltObjCount);
	}	
	docMain->FilterActionList();
	docMain->FilterErrorHandlers();

	//Filter Details
	tmpCount = filterList.GetCount();
	cout << "Starting Filter Documentation" << endl;
	for (unsigned int filterIndex = 0; filterIndex < tmpCount; ++filterIndex)
	{
		LOG << "Filter [" << filterIndex << "-" << tmpCount << "] '" << filterList.FilterGetName(filterIndex) << "': ";
		CDocFilterDetails filterDetails(filterIndex);
		filterDetails.Documentation();
	}


	// escalation object count per first char
	vector<int> escObjCount; escObjCount.resize(38);

	//Escalation List
	docMain->EscalationList("*", escObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->EscalationList(std::string(1, strValue.at(i)), escObjCount);
	}
	docMain->EscalationActionList();

	//Escalation Details
	tmpCount = escalationList.GetCount();
	cout << "Starting Escalation Documentation" << endl;
	for (unsigned int escalIndex = 0; escalIndex < tmpCount; ++escalIndex)
	{
		LOG << "Escalation [" << escalIndex << "-" << tmpCount << "] '" << escalationList.EscalationGetName(escalIndex) << "': ";
		CDocEscalationDetails escalDetails(escalIndex);
		escalDetails.Documentation();
	}


	// char menu object count per first char
	vector<int> mnuObjCount; mnuObjCount.resize(38);

	//CharMenus
	docMain->CharMenuList("*", mnuObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->CharMenuList(std::string(1, strValue.at(i)), mnuObjCount);
	}	

	// Char Menu Details
	tmpCount = menuList.GetCount();
	cout << "Starting Menu Documentation" << endl;
	for (unsigned int menuIndex = 0; menuIndex < tmpCount; ++menuIndex)
	{
		LOG << "Menu [" << menuIndex << "-" << tmpCount << "] '" << menuList.MenuGetName(menuIndex) << "': ";
		CDocCharMenuDetails menuDetails(menuIndex);
		menuDetails.Documentation();
	}

	// schema types object first char count
	vector<int> allObjCount; allObjCount.resize(38);

	//Schema List
	docMain->SchemaList(AR_SCHEMA_NONE,    CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_SCHEMA)      , "Formlist (All)"    , "*", allObjCount);
	docMain->SchemaList(AR_SCHEMA_REGULAR, CPageParams(PAGE_SCHEMA_REGULAR, AR_STRUCT_ITEM_XML_SCHEMA), "Formlist (Regular)", "*", allObjCount);
	docMain->SchemaList(AR_SCHEMA_JOIN,    CPageParams(PAGE_SCHEMA_JOIN, AR_STRUCT_ITEM_XML_SCHEMA)   , "Formlist (Join)"   , "*", allObjCount);
	docMain->SchemaList(AR_SCHEMA_VIEW,    CPageParams(PAGE_SCHEMA_VIEW, AR_STRUCT_ITEM_XML_SCHEMA)   , "Formlist (View)"   , "*", allObjCount);
	docMain->SchemaList(AR_SCHEMA_DIALOG,  CPageParams(PAGE_SCHEMA_DIALOG, AR_STRUCT_ITEM_XML_SCHEMA) , "Formlist (Dialog)" , "*", allObjCount);
	docMain->SchemaList(AR_SCHEMA_VENDOR,  CPageParams(PAGE_SCHEMA_VENDOR, AR_STRUCT_ITEM_XML_SCHEMA) , "Formlist (Vendor)" , "*", allObjCount);	
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->SchemaList(AR_SCHEMA_NONE, CPageParams(strValue.at(i), AR_STRUCT_ITEM_XML_SCHEMA), "Formlist", std::string(1, strValue.at(i)), allObjCount);
	}

	//Schema and field Details
	nTmpCnt=1;
	unsigned int schemaCount = schemaList.GetCount();
	cout << "Starting Schema Documentation" << endl;
	for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
	{
		int rootLevel = 2;
		CARSchema schema(schemaIndex);

		LOG << "Schema [" << (schemaIndex + 1) << "-" << schemaCount << "] '" << schema.GetName() << "': ";
		CDocSchemaDetails *schemaDetails = new CDocSchemaDetails(schemaIndex, rootLevel);
		schemaDetails->Documentation();
		delete schemaDetails;


		//VuiDetails
		LOG << "VuiDetails Schema '" << schema.GetName() << "'" << endl;
		unsigned int objCount = schema.GetVUIs()->GetCount();
		for (unsigned int vuiIndex = 0; vuiIndex < objCount; ++vuiIndex)
		{
			CARVui vui(schema.GetInsideId(), 0, vuiIndex);

			LOG << "SchemaView '" << vui.GetName() << "': ";
			CDocVuiDetails *vuiDetails = new CDocVuiDetails(schema.GetInsideId(), vui, rootLevel);
			vuiDetails->Documentation();
			delete vuiDetails;
		}

		//FieldDetails
		LOG << "FieldDetails Schema '" << schema.GetName() << "'" << endl;		
		objCount = schema.GetFields()->GetCount();
		for (unsigned int fieldIndex = 0; fieldIndex < objCount; ++fieldIndex)
		{	
			CARField field(schema.GetInsideId(), 0, fieldIndex);
			CDocFieldDetails *fieldDetails = new CDocFieldDetails(schema.GetInsideId(), field, rootLevel);
			fieldDetails->Documentation();
			delete fieldDetails;
		}

		nTmpCnt++;
	}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
	// image object count per first char
	vector<int> imgObjCount; imgObjCount.resize(38);

	// Image
	docMain->ImageList("*", imgObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ImageList(std::string(1, strValue.at(i)), imgObjCount);
	}	

	// Image Details
	tmpCount = imageList.GetCount();
	cout << "Starting Image Documentation" << endl;
	for (unsigned int imgIndex = 0; imgIndex < tmpCount; ++imgIndex)
	{
		LOG << "Image [" << imgIndex << "-" << nTmpCnt << "] '" << imageList.ImageGetName(imgIndex) << "': ";

		CDocImageDetails imgDetails(imgIndex);
		imgDetails.Documentation();
	}
#endif

	//GlobalFieldList
	docMain->GlobalFieldList();	

	//MessageList
	docMain->MessageList();	

	//NotificationList
	docMain->NotificationList();

	//Analyzer
	CDocAnalyzer *analyzer = new CDocAnalyzer();
	analyzer->Documentation();
	delete analyzer;

	//Validation
	CDocValidator *validator = new CDocValidator();
	validator->Main();
	delete validator;

	// group count per first char
	vector<int> grpObjCount; grpObjCount.resize(38);

	//Group List
	docMain->GroupList("*", grpObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->GroupList(std::string(1, strValue.at(i)), grpObjCount);
	}	


	//Group Details
	cout << "Starting Group Documentation" << endl;

	tmpCount = groupList.GetCount();
	for (unsigned int groupIndex = 0; groupIndex < tmpCount; ++groupIndex)
	{
		CARGroup grp(groupIndex);

		LOG << "Group [" << (groupIndex + 1) << "-" << tmpCount << "] '" << grp.GetName() << "': ";
		CDocGroupDetails grpDetails(grp);
		grpDetails.Documentation();
	}


	// role count per first char
	vector<int> roleObjCount; roleObjCount.resize(38);

	//Role List
	docMain->RoleList("*", roleObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->RoleList(std::string(1, strValue.at(i)), roleObjCount);
	}	

	//Role Details
	cout << "Starting Role Documentation" << endl;

	tmpCount = roleList.GetCount();
	for (unsigned int roleIndex = 0; roleIndex < tmpCount; ++roleIndex)
	{
		CARRole role(roleIndex);

		LOG << "Role [" << (roleIndex + 1) << "-" << tmpCount << "] '" << role.GetName() << "': ";
		CDocRoleDetails roleDetails(role);
		roleDetails.Documentation();
	}


	// user count per first char
	vector<int> usrObjCount; usrObjCount.resize(38);

	//Userlists
	docMain->UserList("*", usrObjCount);
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->UserList(std::string(1, strValue.at(i)), usrObjCount);
	}	

	//User Details
	cout << "Starting User Documentation" << endl;

	tmpCount = userList.GetCount();
	for (unsigned int userIndex = 0; userIndex < tmpCount; ++userIndex)
	{
		CARUser user(userIndex);

		LOG << "User [" << (userIndex + 1) << "-" << tmpCount << "] '" << user.GetName() << "': ";
		
		CDocUserDetails userDetails(user);
		userDetails.Documentation();
	}

	mTimer.EndTimer();
	this->nDurationDocumentation = mTimer.GetDuration();

	CDocSummaryInfo indexSummary(*this, "");
	indexSummary.Documentation();

	delete docMain;
}

string CARInside::GetFieldEnumValue(int schemaInsideId, int fieldInsideId, int enumPosition)
{
	CARField field(schemaInsideId, fieldInsideId);
	if (field.Exists())
	{
		if(field.GetDataType() == AR_DATA_TYPE_ENUM)
		{
			const ARFieldLimitStruct& limits = field.GetLimits();
			switch(limits.u.enumLimits.listStyle)
			{
			case AR_ENUM_STYLE_REGULAR:
				if (static_cast<unsigned int>(enumPosition) < limits.u.enumLimits.u.regularList.numItems)
					return limits.u.enumLimits.u.regularList.nameList[enumPosition];
				break;
			case AR_ENUM_STYLE_CUSTOM:
				for (unsigned int i=0; i < limits.u.enumLimits.u.customList.numItems; i++) { 
					if (limits.u.enumLimits.u.customList.enumItemList[i].itemNumber == enumPosition) 
						return limits.u.enumLimits.u.customList.enumItemList[i].itemName; 
				} 
				break;
			case AR_ENUM_STYLE_QUERY:
				return "QUERY";
			}						
		}
	}
	return "";
}

string CARInside::LinkToVui(const string& schemaName, int vuiInsideId, int fromRootLevel)
{
	return LinkToVui(SchemaGetInsideId(schemaName), vuiInsideId, fromRootLevel);
}

string CARInside::LinkToVui(int schemaInsideId, int vuiInsideId, int fromRootLevel)
{
	CARVui vui(schemaInsideId, vuiInsideId);
	if (vui.Exists())
	{
		return vui.GetURL(fromRootLevel);
	}
	return "";
}

string CARInside::LinkToField(const string& schemaName, int fieldInsideId, int fromRootLevel)
{	
	return LinkToField(SchemaGetInsideId(schemaName), fieldInsideId, fromRootLevel);
}

string CARInside::LinkToField(int schemaInsideId, int fieldInsideId, const string& linkText, int fromRootLevel)
{	
	CARField field(schemaInsideId, fieldInsideId);
	if (field.Exists())
	{
		return field.GetURL(linkText, fromRootLevel);
	}

	//Field has not been found
	stringstream tmp;

	if(fieldInsideId > 0 && schemaInsideId > -1) //OpenWindow uses 0 what is no valid field
	{
		tmp << "<span class=\"fieldNotFound\">" << fieldInsideId << "</span>";
	}
	else
	{
		tmp << fieldInsideId;
	}

	return tmp.str();
}

string CARInside::LinkToField(int schemaInsideId, int fieldInsideId, int fromRootLevel)
{
	return LinkToField(schemaInsideId, fieldInsideId, "", fromRootLevel);
}

// TODO: maybe change callers to LinkToField and remove this function completely
string CARInside::LinkToMenuField(int schemaInsideId, int fieldInsideId, int fromRootLevel)
{
	return LinkToField(schemaInsideId, fieldInsideId, fromRootLevel);
}

string CARInside::LinkToSchemaTypeList(int schemaType, int rootLevel)
{
	unsigned int page = PAGE_OVERVIEW;

	switch (schemaType)
	{
		case AR_SCHEMA_REGULAR: page = PAGE_SCHEMA_REGULAR; break;
		case AR_SCHEMA_JOIN: page = PAGE_SCHEMA_JOIN; break;
		case AR_SCHEMA_VIEW: page = PAGE_SCHEMA_VIEW; break;
		case AR_SCHEMA_DIALOG: page = PAGE_SCHEMA_DIALOG; break;
		case AR_SCHEMA_VENDOR: page = PAGE_SCHEMA_VENDOR; break;
	}

	return CWebUtil::Link(CAREnum::SchemaType(schemaType), CPageParams(page), "", rootLevel);
}

// TODO: add a function with parameter CARSchema instead of schemaInsideId to pass in the object directly
string CARInside::LinkToSchemaIndex(string indexName, int schemaInsideId, int fromRootLevel)
{
	CARSchema schema(schemaInsideId);
	CPageParams file(PAGE_SCHEMA_INDEXES, &schema);
	return CWebUtil::Link(indexName, file, "", fromRootLevel);
}

string CARInside::LinkToSchema(int insideId, int fromRootLevel)
{
	CARSchema schema(insideId);
	if(schema.Exists())
	{
		return schema.GetURL(fromRootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToSchema(const string& schemaName, int fromRootLevel)
{
	CARSchema schema(schemaName);
	if(schema.Exists())
	{
		return schema.GetURL(fromRootLevel);
	}
	return schemaName;
}

int CARInside::SchemaGetInsideId(string searchObjName)
{
	CARSchema schema(searchObjName);
	if (schema.Exists())
	{
		return schema.GetInsideId();
	}
	return -1;
}

string CARInside::LinkToUser(string loginName, int rootLevel)
{
	CARUser user(loginName);

	if (user.Exists())
		return CWebUtil::Link(loginName, CPageParams(PAGE_DETAILS, &user), "", rootLevel);

	return loginName;
}

bool CARInside::ValidateGroup(const string& appRefName, int permissionId)
{
	if(permissionId >= 0)
	{
		return true;
	}
	else
	{
		CARRole role(permissionId, appRefName);

		if (role.Exists())
		{
			return true;
		}
	}

	return false;
}

string CARInside::LinkToGroup(const string& appRefName, int permissionId, int rootLevel)
{	
	if(permissionId >= 0)
	{
		CARGroup group(-1, permissionId);
		if (group.Exists())
		{
			return CWebUtil::Link(group.GetName(), CPageParams(PAGE_DETAILS, &group), "group.gif", rootLevel);
		}		
	}
	else
	{
		if(!appRefName.empty())
		{
			CARRole role(permissionId, appRefName);

			if(role.Exists())
			{
				return CWebUtil::Link(role.GetName(), CPageParams(PAGE_DETAILS, &role), "doc.gif", rootLevel);
			}
		}
	}

	stringstream strmTmp;
	strmTmp << permissionId;
	return strmTmp.str();
}

string CARInside::LinkToAlRef(const CRefItem& refItem, int rootLevel)
{
	CARActiveLink al(refItem.GetObjectId());
	if (!al.Exists())
		return EmptyValue;

	stringstream strmTmp;
	strmTmp << al.GetURL(rootLevel) << " (" << al.GetOrder() << ")";
	return strmTmp.str();
}

string CARInside::LinkToAl(const string& alName, int fromRootLevel)
{
	CARActiveLink al(alName);
	if (!al.Exists())
		return alName;

	return al.GetURL(fromRootLevel);
}

string CARInside::LinkToAl(int alInsideId, int rootLevel)
{
	CARActiveLink al(alInsideId);

	if (al.Exists())
		return al.GetURL(rootLevel);

	return EmptyValue;
}

string CARInside::LinkToFilterRef(int filterInsideId, int rootLevel)
{	
	CARFilter flt(filterInsideId);
	if (flt.Exists())
	{
		return LinkToFilterRef(&flt, rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToFilterRef(const CRefItem& refItem, int rootLevel)
{
	CARFilter flt(refItem.GetObjectId());
	if (flt.Exists())
	{
		return LinkToFilterRef(&flt, rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToFilterRef(CARFilter* filter, int rootLevel)
{
	stringstream strmTmp;
	strmTmp.str("");

	if (filter != NULL)
		strmTmp << filter->GetURL(rootLevel) << " (" << filter->GetOrder() << ")";

	return strmTmp.str();
}

string CARInside::LinkToFilter(string filterName, int fromRootLevel)
{
	if (filterName.empty()) return filterName;

	int fltInsideId = filterList.Find(filterName.c_str());
	if (fltInsideId > -1)
	{
		CARFilter flt(fltInsideId);
		return flt.GetURL(fromRootLevel);
	}
	return filterName;
}

string CARInside::LinkToMenu(string menuName, int fromRootLevel)
{
	if (menuName.compare("$NULL$")==0)
	{
		return menuName;
	}

	CARCharMenu menu(menuName);
	if (menu.Exists())
	{
		return menu.GetURL(fromRootLevel);
	}

	//Menu has not been found
	//If the menu is missing, we just return the name of it. maybe link to the
	//"missing menus" page (must be implemented first) of CDocValidator later.
	return "<span class=\"fieldNotFound\">" + menuName + "</span>";
}

string CARInside::LinkToMenu(const CRefItem& refItem, int rootLevel)
{
	CARCharMenu menu(refItem.GetObjectId());
	if (menu.Exists())
	{
		return menu.GetURL(rootLevel);
	}

	return EmptyValue;
}

string CARInside::LinkToEscalation(const string& escalationName, int fromRootLevel)
{
	CAREscalation escal(escalationName);
	if (escal.Exists())
	{
		return escal.GetURL(fromRootLevel);
	}
	return escalationName;
}

string CARInside::LinkToEscalation(const CRefItem& refItem, int fromRootLevel)
{
	CAREscalation escal(refItem.GetObjectId());
	if (escal.Exists())
	{
		return escal.GetURL(fromRootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToContainer(string containerName, int fromRootLevel)
{
	CARContainer cnt(containerName);
	if (cnt.Exists())
	{
		return cnt.GetURL(fromRootLevel);
	}
	return containerName;
}

string CARInside::LinkToContainer(const CRefItem& refItem, int rootLevel)
{
	CARContainer cnt(refItem.GetObjectId());
	if (cnt.Exists())
	{
		return cnt.GetURL(rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToServerInfo(string srvName, int rootLevel)
{		
	string result;

	if(strcmp(srvName.c_str(), "")==0)
	{
		return CWebUtil::Link(appConfig.serverName, CPageParams(PAGE_SERVER_INFO), "", rootLevel);
	}
	else if(strcmp(srvName.c_str(), AR_CURRENT_SERVER_TAG)==0 || strcmp(srvName.c_str(), AR_CURRENT_SCREEN_TAG)==0)
	{
		return CWebUtil::Link(appConfig.serverName, CPageParams(PAGE_SERVER_INFO), "", rootLevel);
	}
	else
	{
		return CWebUtil::Link(srvName, CPageParams(PAGE_SERVER_INFO), "", rootLevel);
	}


	return result;
}

string CARInside::LinkToObjByRefItem(const CRefItem& refItem, int rootLevel)
{
	string result = EmptyValue;

	switch(refItem.GetObjectType())
	{
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK: 
		{
			result = this->LinkToAlRef(refItem, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_FILTER:
		{
			result = this->LinkToFilterRef(refItem, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_SCHEMA:
		{
			result = this->LinkToSchema(refItem.GetObjectName(), rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_FIELD:
		{
			result = this->LinkToField(refItem.GetObjectId(), refItem.GetSubObjectId(), rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_VUI:
		{
			result = this->LinkToVui(refItem.GetObjectId(), refItem.GetSubObjectId(), rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_ESCALATION:
		{
			result = this->LinkToEscalation(refItem, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_CHAR_MENU:
		{
			result = this->LinkToMenu(refItem, rootLevel);
		}
		break;		
	case AR_STRUCT_ITEM_XML_CONTAINER:
		{
			result = this->LinkToContainer(refItem, rootLevel);
		}
		break;
//#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
//	case AR_STRUCT_ITEM_XML_IMAGE:
//		{
//			result = this->LinkToImage(objName, rootLevel);
//		}
//		break;
//#endif
	}

	return result;
}

string CARInside::XmlObjEnabled(CARServerObject *obj)
{
	switch(obj->GetServerObjectTypeXML())
	{
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK: 
		return CAREnum::ObjectEnable(static_cast<CARActiveLink*>(obj)->GetEnabled());
	case AR_STRUCT_ITEM_XML_FILTER:
		return CAREnum::ObjectEnable(static_cast<CARFilter*>(obj)->GetEnabled());
	case AR_STRUCT_ITEM_XML_ESCALATION:
		return CAREnum::ObjectEnable(static_cast<CAREscalation*>(obj)->GetEnabled());
	default:
		return "";
	}
}

string CARInside::XmlObjEnabled(int arsStructItemType, string objName)
{
	switch(arsStructItemType)
	{
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK: 
		{
			CARActiveLink al(objName);
			if (al.Exists())
			{
				return CAREnum::ObjectEnable(al.GetEnabled());
			}
		}
		break;
	case AR_STRUCT_ITEM_XML_FILTER:
		{
			int fltInsideId = filterList.Find(objName.c_str());
			if (fltInsideId > -1)
			{
				return CAREnum::ObjectEnable(filterList.FilterGetEnabled(fltInsideId));
			}
		}
		break;		
	case AR_STRUCT_ITEM_XML_ESCALATION:
		{
			int escInsideId = escalationList.Find(objName.c_str());
			if (escInsideId > -1)
			{
				return CAREnum::ObjectEnable(escalationList.EscalationGetEnabled(escInsideId));
			}
		}
		break;	
	}

	return "";
}

void CARInside::AddFieldReference(int schemaId, int fieldId, const CRefItem& ref)
{
	if (ref.GetMessageId() == -1) return; // if no message id is specified, dont create a reference

	CARSchema schema(schemaId);
	CARField fld(schemaId, fieldId);

	if (fld.Exists())
	{
		if (!fld.ReferenceExists(ref))
			fld.AddReference(ref);
	}
	else if (schema.Exists())
	{
		schema.AddMissingFieldReference(fieldId, ref);
	}
	// TODO: create a list of missing schemas
	//else
	//{
	//	LOG << "Missing Schema!" << endl;
	//}
}

void CARInside::AddMenuReference(const string& menuName, const CRefItem &ref)
{
	if (menuName == "$NULL$") return;

	CARCharMenu menu(menuName);

	if (menu.Exists())
	{
		// if the menu was found add the referece to it
		if (!menu.ReferenceExists(ref))
			menu.AddReference(ref);
	}
	else
	{
		// if the menu is missing, add the name to the missing menus map and append the reference to it
		if (missingMenuReferences.find(menuName) != missingMenuReferences.end())
		{
			missingMenuReferences.insert(pair<string, vector<CRefItem> >(menuName, vector<CRefItem>()));
		}

		CARCharMenu::ReferenceList::iterator curIt = missingMenuReferences[menuName].begin();
		CARCharMenu::ReferenceList::iterator endIt = missingMenuReferences[menuName].end();

		for (; curIt != endIt; ++curIt)
		{
			if (*curIt == ref)
				return;
		}

		missingMenuReferences[menuName].push_back(ref);
	}
}

string CARInside::TextFindFields(string inText, string fieldSeparator, int schemaInsideId, int rootLevel, bool findKeywords, const CRefItem *refItem)
{	
	try
	{
		if(inText.size() == 0)
			return "";

		CARSchema schema(schemaInsideId);
		if (!schema.Exists())
			return inText;

		string dummySeparator = "##"; //dummy placeholder to avoid infinite replacements

		stringstream strmTmp;

		string::size_type curPos = 0;
		string::size_type startPos = 0;
		string::size_type maxLen = inText.length();
		string::size_type fieldIdPos = 0;

		char fieldId[20];
		char *enumId;    // points to the enum part of status history within fieldId later
		char *usrOrTime; // points to the "user or time" part of status history within fieldId later

		while ((startPos = inText.find(fieldSeparator.at(0),curPos)) != std::string::npos)
		{
			++startPos;
			strmTmp << inText.substr(curPos,startPos - curPos);
			curPos = startPos;

			// reset
			fieldIdPos = 0;	
			fieldId[0] = 0;
			enumId = NULL;
			usrOrTime = NULL;

			for (curPos = startPos; curPos < maxLen; ++curPos)
			{
				char currChar = inText.at(curPos);
				if (currChar == '-' && fieldIdPos != 0)  
					break; // - is only allowed at the beginning
				if (currChar >= '0' && currChar <= '9' || currChar == '-' || currChar == '.')
				{
					if (fieldIdPos + 1 == 20)
						break;	// max length .. that cant be a field

					if (currChar != '.' && fieldIdPos > 1 && fieldId[fieldIdPos-1] == '.')
					{
						fieldId[fieldIdPos-1] = 0;
						if (enumId == NULL)
							enumId = &fieldId[fieldIdPos];
						else if (usrOrTime == NULL)
							usrOrTime = &fieldId[fieldIdPos];
						else
							break; // uhh ohh
					}
					// copy it over
					fieldId[fieldIdPos++] = currChar;
					continue;
				}
				if (currChar == fieldSeparator.at(0))
				{
					// end found
					fieldId[fieldIdPos] = 0;

					if (fieldId[0] == 0)
						break;	// two $$ .. someone must be dreaming about more money

					int iFieldId = atoi(fieldId);
					if (iFieldId > 0)
					{
						CARField field(schemaInsideId, iFieldId);

						if (!field.Exists())
							break;

						// now link to the field
						strmTmp << field.GetURL(rootLevel);
						if (refItem != NULL)
							AddFieldReference(schemaInsideId, iFieldId, *refItem);

						// special handling for status history
						if (iFieldId == 15)
						{
							if (enumId == NULL || usrOrTime == NULL) break;

							// resolve user or time attribute
							int iUsrOrTime = atoi(usrOrTime);
							const char* usrOrTimeStr = usrOrTime;
							switch (iUsrOrTime)
							{
							case AR_STAT_HISTORY_USER:
								usrOrTimeStr = "USER";
								break;
							case AR_STAT_HISTORY_TIME:
								usrOrTimeStr = "TIME";
								break;
							}

							// handle status history
							CARField fieldStatus(schemaInsideId,7);	// get status field
							string enumValue;
							if (fieldStatus.Exists())
							{
								int iEnumId = atoi(enumId);
								enumValue = GetFieldEnumValue(schemaInsideId, fieldStatus.GetInsideId(), iEnumId);
							}

							strmTmp << ".";
							if (enumValue.empty())
								strmTmp << enumId;
							else
								strmTmp << enumValue;
							strmTmp << "." << usrOrTimeStr;
						}
						strmTmp << "$";
						++curPos; // skip the $ so it isnt found again
					}
					else if (fieldId[0] == '-' && iFieldId <= 0)
					{
						// keyword handling
						int iKeyword = abs(iFieldId);
						strmTmp << CAREnum::Keyword(iKeyword) << "$";
						++curPos; // skip the $ so it isnt found again
					}
					break;
				}
				break;
			}
			if (curPos < startPos)
				strmTmp << inText.substr(curPos,startPos - curPos);
		}
		if (curPos < maxLen)
			strmTmp << inText.substr(curPos,maxLen - curPos);

		//write the current string back to inText, then clear the stream for use with the application code
		inText = strmTmp.str();
		strmTmp.clear();
		strmTmp.str("");

		// check for special run-process commands
		bool specialFound = false;
		string::size_type cmdStartPos = inText.find("Application-");
		if (cmdStartPos != string::npos)
		{
			string::size_type nextSearchPos = cmdStartPos + 12; // add length("Application-")
			specialFound = true;

			if (inText.compare(nextSearchPos, 17, "Copy-Field-Value ") == 0)
			{
				strmTmp << processTwoFields("Application-Copy-Field-Value", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 13, "Delete-Entry ") == 0)
			{
				strmTmp << processForm("Application-Delete-Entry", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 12, "Format-Qual ") == 0)
			{
				strmTmp << processForm("Application-Format-Qual", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 19, "Format-Qual-Filter ") == 0)
			{
				strmTmp << processForm("Application-Format-Qual-Filter", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 15, "Format-Qual-ID ") == 0)
			{
				strmTmp << processForm("Application-Format-Qual-ID", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 14, "Format-Qual-L ") == 0)
			{
				strmTmp << processForm("Application-Format-Qual-L", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 18, "Get-Approval-Join ") == 0)
			{
				strmTmp << processForm("Application-Get-Approval-Join", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 19, "Get-Approval-Join2 ") == 0)
			{
				strmTmp << processForm("Application-Get-Approval-Join2", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 16, "Get-DetSig-Join ") == 0)
			{
				strmTmp << processForm("Application-Get-DetSig-Join", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 17, "Get-DetSig-Join2 ") == 0)
			{
				strmTmp << processForm("Application-Get-DetSig-Join2", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 15, "Get-Form-Alias ") == 0)
			{
				strmTmp << processForm("Application-Get-Form-Alias", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 17, "Get-Locale-VuiID ") == 0)
			{
				strmTmp << processForm("Application-Get-Locale-VuiID", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 25, "Get-Next-Recurrence-Time ") == 0)
			{
				strmTmp << processForm("Application-Get-Next-Recurrence-Time", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 17, "Map-Ids-To-Names ") == 0)
			{
				strmTmp << processForm("Application-Map-Ids-To-Names", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 19, "Map-Ids-To-Names-L ") == 0)
			{
				strmTmp << processForm("Application-Map-Ids-To-Names-L", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 17, "Map-Names-To-Ids ") == 0)
			{
				strmTmp << processForm("Application-Map-Names-To-Ids", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 19, "Map-Names-To-Ids-L ") == 0)
			{
				strmTmp << processForm("Application-Map-Names-To-Ids-L", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 11, "Parse-Qual ") == 0)
			{
				strmTmp << processForm("Application-Parse-Qual", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 18, "Parse-Qual-Filter ") == 0)
			{
				strmTmp << processForm("Application-Parse-Qual-Filter", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 13, "Parse-Qual-L ") == 0)
			{
				strmTmp << processForm("Application-Parse-Qual-L", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 20, "Parse-Qual-SField-L ") == 0)
			{
				strmTmp << processForm("Application-Parse-Qual-SField-L", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 17, "Parse-Val-SField ") == 0)
			{
				strmTmp << processForm("Application-Parse-Val-SField", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 19, "Query-Delete-Entry ") == 0)
			{
				strmTmp << processForm("Application-Query-Delete-Entry", inText, schemaInsideId, rootLevel, refItem);
			}
			else
			{
				specialFound = false;
			}
		}
		
		cmdStartPos = inText.find("PERFORM-ACTION-");
		if (cmdStartPos != string::npos)
		{
			string::size_type nextSearchPos = cmdStartPos + 15; // add length("PERFORM-ACTION-")
			specialFound = true;

			if (inText.compare(nextSearchPos, 12, "ACTIVE-LINK ") == 0)
			{
				strmTmp << processSecondParameter("PERFORM-ACTION-ACTIVE-LINK", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 15, "ADD-ATTACHMENT ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-ADD-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 18, "DELETE-ATTACHMENT ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-DELETE-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 16, "GET-FIELD-LABEL ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-GET-FIELD-LABEL", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 15, "GET-PREFERENCE ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-GET-PREFERENCE", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 28, "NAV-FIELD-SET-SELECTED-ITEM ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-NAV-FIELD-SET-SELECTED-ITEM", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 16, "OPEN-ATTACHMENT ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-OPEN-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 16, "SAVE-ATTACHMENT ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-SAVE-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 15, "SET-PREFERENCE ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-SET-PREFERENCE", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 12, "TABLE-CLEAR ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-CLEAR", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 23, "TABLE-CLEAR-ROWCHANGED ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-CLEAR-ROWCHANGED", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 18, "TABLE-DESELECTALL ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-DESELECTALL", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 26, "TABLE-GET-SELECTED-COLUMN ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-GET-SELECTED-COLUMN", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 23, "TABLE-IS-LEAF-SELECTED ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-IS-LEAF-SELECTED", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 17, "TABLE-NEXT-CHUNK ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-NEXT-CHUNK", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 17, "TABLE-PREV-CHUNK ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-PREV-CHUNK", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 14, "TABLE-REFRESH ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-REFRESH", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 13, "TABLE-REPORT ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-REPORT", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 18, "TABLE-SELECT-NODE ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-SELECT-NODE", inText, schemaInsideId, rootLevel, refItem);
			}
			else if (inText.compare(nextSearchPos, 16, "TABLE-SELECTALL ") == 0)
			{
				strmTmp << processOneField("PERFORM-ACTION-TABLE-SELECTALL", inText, schemaInsideId, rootLevel, refItem);
			}
			else
			{
				specialFound = false;
			}
		}

		return (specialFound ? strmTmp.str() : inText);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in TextFindField: " << e.what() << endl;
	}

	return inText;
}

// TODO: remove unused function
string CARInside::TextFindKeywords(string inText, string fieldSeparator)
{	

	try
	{
		for(unsigned int nKeyword=0; nKeyword< AR_KEYWORD_NO; nKeyword++)
		{
			stringstream strmKeyword;
			strmKeyword << fieldSeparator << "-" << nKeyword << fieldSeparator;

			stringstream strmTxtKeyword;
			strmTxtKeyword << fieldSeparator << CAREnum::Keyword(nKeyword) << fieldSeparator;		

			inText = CUtil::StrReplace(strmKeyword.str(), strmTxtKeyword.str(), inText);	
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in TextFindKeywords:" << e.what() << endl;
	}

	return inText;
}


string CARInside::XMLFindFields(string inText, int schemaInsideId, int rootLevel, const CRefItem *refItem)
{	
	try
	{
		if(inText.empty())
			return "";

		CARSchema schema(schemaInsideId);

		unsigned int fieldCount = schema.GetFields()->GetCount();
		for(unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
		{
			CARField field(schemaInsideId, 0, fieldIndex);

			stringstream strmTmp;
			strmTmp << "arFieldId=&quot;" << field.GetFieldId() << "&quot;";			

			stringstream fieldLink;
			fieldLink << "arFieldId=&quot;" << field.GetURL(rootLevel) << "&quot;";					

			unsigned int nLengthOrg = (unsigned int)inText.length();

			string fField = strmTmp.str();
			inText = CUtil::StrReplace(fField, fieldLink.str(), inText);

			//if the string is longer because we have added a link (long string) we add a field reference
			if(inText.length() > nLengthOrg) 
			{
				AddFieldReference(schema.GetInsideId(), field.GetInsideId(), *refItem);
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in XMLFindField: " << e.what() << endl;
	}

	return inText;
}

string CARInside::ServerObjectHistory(CARServerObject *obj, int rootLevel)
{		
	stringstream strm;
	strm.str("");

	try
	{
		stringstream historyLog;
		if(obj->GetChangeDiary() != NULL)
		{				
			ARDiaryList diaryList; ARZeroMemory(&diaryList);
			if(ARDecodeDiary(&this->arControl, const_cast<char*>(obj->GetChangeDiary()), &diaryList, &this->arStatus)==AR_RETURN_OK)
			{
				if(diaryList.numItems > 0)
				{
					for(unsigned int j=0; j<diaryList.numItems; j++)
					{
						historyLog << CUtil::DateTimeToHTMLString(diaryList.diaryList[j].timeVal) << " " << this->LinkToUser(diaryList.diaryList[j].user, rootLevel) << "<br/>" << endl;
						historyLog << diaryList.diaryList[j].value << "<br/><br/>" << endl;
					}
				}
				else
				{
					historyLog << EmptyValue << endl;
				}
			}
			FreeARDiaryList(&diaryList, false);				
		}

		CTable tbl("history", "TblObjectHistory");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Value");

		tbl.description = CWebUtil::ImageTag("doc.gif", rootLevel) + "Change History";

		//Owner
		CTableRow tblRow("");
		tblRow.AddCellList(CTableCell("Owner"), CTableCell(this->LinkToUser(obj->GetOwner(), rootLevel)));
		tbl.AddRow(tblRow);

		//Last changed		
		stringstream strmLastChanged;
		strmLastChanged.str("");
		strmLastChanged << CUtil::DateTimeToHTMLString(obj->GetTimestamp()) << " " << this->LinkToUser(obj->GetLastChanged(), rootLevel) << endl;

		tblRow.AddCellList(CTableCell("Last changed"), CTableCell(strmLastChanged.str()));
		tbl.AddRow(tblRow);


		//History
		tblRow.AddCellList(CTableCell("History Log"), CTableCell(historyLog.str()));
		tbl.AddRow(tblRow);

		//Helptext
		string tmpHelptext;
		if(obj->GetHelpText() != NULL)
		{
			tmpHelptext = CUtil::StrReplace("\n", "<br/>", obj->GetHelpText());
		}
		else
		{
		  tmpHelptext = EmptyValue;
		}

		tblRow.AddCellList(CTableCell("Helptext"), CTableCell(tmpHelptext));
		tbl.AddRow(tblRow);

		strm << tbl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION writing server object history: " << e.what() << endl;
	}	

	return strm.str();
}

void CARInside::DoWork(int nMode)
{
	CWindowsUtil winUtil(appConfig);

	// first step is to create directory structure and resources (images, css and js)
	Prepare();
	winUtil.Load();

	// now load the object either from server or from file
	LoadServerObjects(nMode);

	// now create navigation page based on supported server features
	{ CNavigationPage navPage(appConfig); navPage.Write(); }

	// build needed reference tables
	CScanMain::BuildReferences();

	// write documentation
	Documentation();
}

void CARInside::ParseVersionString(string version)
{
	char ver[40];
	int part = 0;

	vMajor = 0;
	vMinor = 0;
	vRevision = 0;

	unsigned int verPos=0;
	unsigned int verStart=0;
	for (; verPos < version.size(); ++verPos)
	{
		char c = version.at(verPos);
		if (c >= '0' && c <= '9' ) 
		{
			ver[verPos] = c;
			continue;
		}
		if (verPos > verStart)
		{
			ver[verPos] = 0;
			int num = atoi(&ver[verStart]);
			switch (part)
			{
			case 0:
				vMajor = num;
				break;
			case 1:
				vMinor = num;
				break;
			case 2:
				vRevision = num;
				break;
			}
			++part;
			verStart = verPos + 1;
		}
		if (c != '.') break;
	}
	ver[verPos] = 0;
}

void CARInside::ParseVersionString(int xmlVersion)
{
	if (xmlVersion <= arXmlVersion) return;

	if (xmlVersion >= AR_XML_VERSION_750)
	{
		arServerVersion = "7.5.00";
		vMajor = 7; vMinor = 5; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_710)
	{
		arServerVersion = "7.1.00";
		vMajor = 7; vMinor = 1; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_700)
	{
		arServerVersion = "7.0.00";
		vMajor = 7; vMinor = 0; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_710)
	{
		arServerVersion = "7.1.00";
		vMajor = 7; vMinor = 1; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_710)
	{
		arServerVersion = "7.1.00";
		vMajor = 7; vMinor = 1; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_600)
	{
		// 6.0 and 6.3 use the same export version number. To show keywords
		// and other things (in case of a 6.3 export file) correctly, the
		// version is set to 6.3
		arServerVersion = "6.03.00";
		vMajor = 6; vMinor = 3; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_510)
	{
		arServerVersion = "5.1.00";
		vMajor = 5; vMinor = 1; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_500)
	{
		arServerVersion = "5.0.00";
		vMajor = 5; vMinor = 0; vRevision = 0;
	}
	else if (xmlVersion >= AR_XML_VERSION_450)
	{
		arServerVersion = "4.5.00";
		vMajor = 4; vMinor = 5; vRevision = 0;
	}
}

int CARInside::CompareServerVersion(int major, int minor, int revision)
{
	if (vMajor == major)
	{
		if (minor > -1)
		{
			if (vMinor == minor)
			{
				if (revision > -1)
				{
					if (vRevision == revision) return 0;
					if (vRevision < revision) return -1;
					if (vRevision > revision) return 1;
				}
				return 0;
			}
			if (vMinor < minor) return -1;
			if (vMinor > minor) return 1;
		}
		return 0;
	}
	if (vMajor < major) return -1;
	/*if (vMajor > major)*/ return 1;
}

string CARInside::processOneField(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem)
{
	stringstream strmTmp;
	int fieldId = 0;
	size_t length = command.length()+1;
	size_t pos = inText.find(command);

	//put the command into the stream
	strmTmp << inText.substr(0,(length+pos));
	//get everything after the command
	string tmp = inText.substr(length+pos);
	//get position of next space
	pos = tmp.find(" ");

	//set the fieldID = to the one we found

	if (pos != std::string::npos)
		fieldId = atoi(tmp.substr(0,pos).c_str());
	else
		fieldId = atoi(tmp.c_str());

	if (fieldId != std::string::npos && fieldId > 0)
		strmTmp << refFieldID(fieldId, schemaInsideId, rootLevel, refItem);

	if (pos != std::string::npos)
	{
		tmp = tmp.substr(pos,tmp.length());
		strmTmp << tmp;
	}

	return strmTmp.str();
}
string CARInside::processTwoFields(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem)
{
	stringstream strmTmp;
	int fieldId = 0;
	size_t length = command.length()+1;
	size_t pos = inText.find(command);

	//put the command into the stream
	strmTmp << inText.substr(0,(length+pos));
	//get everything after the command
	string tmp = inText.substr(length+pos);
	if (tmp.find("$") == 0 && tmp.find("$") != std::string::npos)
	{
		tmp = tmp.substr(1);
		pos = tmp.find("$")+1;
		strmTmp << "$" << tmp.substr(0,pos);
	}
	else
	{
		pos = tmp.find(" ");
		fieldId = atoi(tmp.substr(0,pos).c_str());
		strmTmp << refFieldID(fieldId, schemaInsideId, rootLevel, refItem);
	}

	//put a space between the field ID's
	strmTmp << " ";

	//get position of next space
	pos = tmp.find(" ");
	//get everything after the space
	tmp = tmp.substr(pos+1);

	if (tmp.find("$") == 0 && tmp.find("$") != std::string::npos)
	{
		strmTmp << tmp;
	}
	else
	{
		fieldId = atoi(tmp.c_str());
		strmTmp << refFieldID(fieldId, schemaInsideId, rootLevel, refItem);
	}

	return strmTmp.str();
}
string CARInside::processForm(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem)
{
	stringstream strmTmp;
	string form = "";
	size_t length = command.length()+1;
	size_t pos = inText.find(command);

	//put the command into the stream
	strmTmp << inText.substr(0,(length+pos));
	//get everything after the command
	string tmp = inText.substr(length+pos);
	//ok, we are at the first parameter, need to first find if it starts with a "
	if (tmp.substr(0,1) == "\"")
	{
		strmTmp << "\"";
		pos = tmp.find("\"",1);
		form = tmp.substr(1,pos-1);
	}
	else
	{
		//get position of next space
		pos = tmp.find(" ");
		form = tmp.substr(0,pos);
	}
	//generate link for form
	CARSchema schema(form);
	if (schema.Exists())
		strmTmp << schema.GetURL(rootLevel, false);
	else
		strmTmp << form;
		//AddReferenceItem(refItem);

	//write anything past the form to output
	strmTmp << tmp.substr(pos);

	return strmTmp.str();
}
string CARInside::processSecondParameter(const string& command, const string& inText, int schemaInsideId, int rootLevel, const CRefItem *refItem)
{
	stringstream strmTmp;
	int fieldId = 0;
	size_t length = command.length()+1;
	size_t pos = inText.find(command);

	//put the command into the stream
	strmTmp << inText.substr(0,(length+pos));
	//get everything after the command
	string tmp = inText.substr(length+pos);
	//get position of next space
	pos = tmp.find(" ");
	//set the fieldID = to the one we found
	strmTmp << tmp.substr(0,pos);
	//put a space in there
	strmTmp << " ";
	//get position of next space
	pos = tmp.find(" ");
	//get everything after the space
	if (pos != std::string::npos)
	{
		tmp = tmp.substr(pos);
		//set the fieldID = to the one we found
		fieldId = atoi(tmp.c_str());
		strmTmp << refFieldID(fieldId, schemaInsideId, rootLevel, refItem) << endl;
	}

	return strmTmp.str();
}
bool CARInside::getPos(const string& inText, const string& findText)
{
	size_t pos = inText.find(findText);
	return (pos != std::string::npos);
}

string CARInside::refFieldID(int iFieldId, int schemaInsideId, int rootLevel, const CRefItem *refItem)
{
	stringstream strmTmp;

	//add the reference
	try {
		CARField field(schemaInsideId, iFieldId);
		if (field.Exists())
		{
			strmTmp << field.GetURL(rootLevel);
		}
		else
			strmTmp << iFieldId;

		// add reference
		AddFieldReference(schemaInsideId, iFieldId, *refItem);
	}
	catch (...) {
		cout << "Exception in refFieldID" << endl;
	}

	return strmTmp.str();
}

#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
int CARInside::LoadImages()
{
	imageList.LoadFromServer();
	imageList.Sort();
	return imageList.GetCount();
}

string CARInside::LinkToImage(unsigned int imageIndex, int rootLevel)
{
	CARImage image(imageIndex);
	if (image.Exists())
	{
		return image.GetURL(rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToImage(const string &imageName, int rootLevel)
{
	int imageIndex = imageList.FindImage(imageName.c_str());
	if (imageIndex < 0)
	{
		return "<span class=\"fieldNotFound\">" + imageName + "</span>";
	}
	else
	{
		return LinkToImage(imageIndex, rootLevel);
	}
}
#endif // AR_CURRENT_API_VERSION >= AR_API_VERSION_750

bool CARInside::WriteHTAccess()
{
	stringstream strm;
	strm << this->appConfig.targetFolder << "/" << ".htaccess";

	if (FileExists(strm.str()))
		return true;		// if file is already there, it should be configured correctly

	try
	{
		string fileName = strm.str();
		LOG << "Save file '" << fileName;

		ofstream fout(fileName.c_str(), ios::out);
		fout << "RemoveType .gz" << endl << "AddEncoding gzip .gz";
		fout.close();
	}
	catch (exception &e)
	{
		stringstream erStrm;
		erStrm << "Error saving file '" << strm.str() << "' to disk. Error: " << e.what();
		throw(AppException(erStrm.str(), "undefined", "FileIo"));
	}
	return true;
}

void CARInside::SetupOverlaySupport()
{
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_764
	if (CompareServerVersion(7,6,4) >= 0 && appConfig.bOverlaySupport)
	{
		ARValueStruct value;
		value.dataType = AR_DATA_TYPE_CHAR;
		value.u.charVal = AR_OVERLAY_CLIENT_MODE_FULL;
		if (ARSetSessionConfiguration(&arControl, AR_SESS_CONTROL_PROP_API_OVERLAYGROUP, &value, &arStatus) != AR_RETURN_OK)
			cerr << "SetSessionConfiguration failed: " << GetARStatusError(&arStatus);
	}
#endif
}

string CARInside::PlaceOverlaidNotice(const CARServerObject& obj, int rootLevel)
{
	stringstream tmp;

	tmp << "<div id=\"ovlNote\">" << endl;
	tmp << "This object is currently not executed/used because it is overlaid by " << obj.GetURL(rootLevel, false) << endl;
	tmp << "</div>";

	return tmp.str();
}
