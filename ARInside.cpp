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

/////////
// the following file is generated via a pre-build step using "svnrev_template.h" as template. 
// The $WCREV$ keyword of the template is replaced with the revision number.
#include "svnrev.h"

/////////
// version information block
#define VERSION "3.0.1"
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
	this->userList.clear();
	this->groupList.clear();
	this->roleList.clear();
	this->serverInfoList.clear();
	this->globalFieldList.clear();
	this->listFieldRefItem.clear();
	this->listMenuRefItem.clear();
	this->listFieldNotFound.clear();

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
}

CARInside::~CARInside(void)
{
	DestroyFileNamingStrategy();
}

CARInside* CARInside::GetInstance()
{
	return CARInside::pInsideInstance;
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

		if(nResult == AR_RETURN_OK)
		{
			ARBoolean isAdmin, isSubadmin, hasCustomize;
			nResult = ARVerifyUser(&this->arControl, &isAdmin, &isSubadmin, &hasCustomize, &this->arStatus);
			
			if (nResult != AR_RETURN_OK)
			{
				throw(AppException(GetARStatusError(), "undefined", "ARSystem"));
			}
			FreeARStatusList(&this->arStatus, false);

			CARServerInfo serverInfo(this->arControl, this->arStatus);
			this->srvHostName = serverInfo.GetValue(AR_SERVER_INFO_HOSTNAME);
			this->srvFullHostName = serverInfo.GetValue(AR_SERVER_INFO_FULL_HOSTNAME);
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
	}

	delete docMain;
}

bool CARInside::FieldreferenceExists(int schemaInsideId, int fieldInsideId, const CFieldRefItem &refItem)
{
	list<CFieldRefItem>::iterator iter;
	list<CFieldRefItem>::iterator endIt = this->listFieldRefItem.end();
	CFieldRefItem *item;

	for ( iter = this->listFieldRefItem.begin(); iter != endIt; ++iter)
	{	
		item = &(*iter);
		if(	item->fieldInsideId == fieldInsideId
			&& item->schemaInsideId == schemaInsideId
			&& item->arsStructItemType == refItem.arsStructItemType
			&& item->fromName.compare(refItem.fromName) == 0
			&& item->description.compare(refItem.description) == 0)
		{
			return true;
		}
	}	
	return false;
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

	CARServerInfo srvInfo(this->arControl, this->arStatus);
	arServerVersion = srvInfo.GetValue(AR_SERVER_INFO_VERSION);
	cout << "server version: " << arServerVersion << endl;
	ParseVersionString(arServerVersion);

	//LoadServerInfoList	
	if(appConfig.bLoadServerInfoList)
	{
		cout << "Start loading Server Information:" << endl;
		CARServerInfo serverInfo(this->arControl, this->arStatus);
		serverInfo.GetList(serverInfoList);
		cout << (unsigned int)serverInfoList.size() << " server settings loaded" << endl;
	}
	else
		cout << endl << "Loading Server Informations [SKIPPED]" << endl;

	//LoadUserList
	if(appConfig.bLoadUserList)
	{
		cout << endl << "Start loading Users:" << endl;		

		CARDataFactory *dataFactory = new CARDataFactory(this->arControl, this->arStatus);
		dataFactory->GetListUser(this->appConfig, userList);

		dataFactory->Sort(userList);
		cout << (unsigned int)userList.size() << " Users loaded" << endl;

		delete dataFactory;
	}
	else
		cout << endl << "Loading Users [SKIPPED]" << endl;


	//LoadGroupList
	if(appConfig.bLoadGroupList)
	{
		cout << endl << "Start loading Groups:" << endl;		

		CARDataFactory *dataFactory = new CARDataFactory(this->arControl, this->arStatus);
		dataFactory->GetListGroup(this->appConfig, groupList);

		dataFactory->Sort(groupList);
		cout << (unsigned int)groupList.size() << " Groups loaded" << endl;
		delete dataFactory;
	}
	else
		cout << endl << "Loading Groups [SKIPPED]" << endl;

	//LoadRoleList
	if(appConfig.bLoadRoleList)
	{
		cout << endl << "Start loading Roles:" << endl;		

		CARDataFactory *dataFactory = new CARDataFactory(this->arControl, this->arStatus);
		dataFactory->GetListRoles(this->appConfig, roleList);

		dataFactory->Sort(roleList);
		cout << (unsigned int)roleList.size() << " Roles loaded" << endl;
		delete dataFactory;
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
		GetARStatusError();
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
		GetARStatusError();
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

void CARInside::Sort(list<CARContainer> &listResult)
{
	listResult.sort(SortByName);
}

void CARInside::Sort(list<CARCharMenu> &listResult)
{
	listResult.sort(SortByName);
}

bool CARInside::SortByName(const CARServerObject& t1, const CARServerObject& t2 )
{
	string tmpStr1 = CUtil::String2Comp(t1.GetName());
	string tmpStr2 = CUtil::String2Comp(t2.GetName());
	return ( tmpStr1.compare(tmpStr2) < 0 );
}

// TODO: this method becomes obsolete when IFileNaming is finished
string CARInside::ObjListFilename(string firstChar)
{
	string fName = "index_";
	if(firstChar != "#")
		fName += firstChar;
	else
		fName += "other";

	return fName;
}

void CARInside::Documentation(void)
{	
	CAppTimer mTimer;
	mTimer.StartTimer();

	string strValue = "abcdefghijklmnopqrstuvwxyz0123456789#";
	CDocMain *docMain = new CDocMain();

	//Server information
	docMain->ServerInfoList();

	//ContainerList
	docMain->ContainerList(ARCON_GUIDE, "ContainerList (ActiveLinkGuide)", "*");
	docMain->ContainerList(ARCON_APP, "ContainerList (Application)", "*");
	docMain->ContainerList(ARCON_PACK, "ContainerList (PackingList)", "*");
	docMain->ContainerList(ARCON_FILTER_GUIDE, "ContainerList (FilterGuide)", "*");
	docMain->ContainerList(ARCON_WEBSERVICE, "ContainerList (Webservice)", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ContainerList(ARCON_GUIDE, "GuideList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_APP, "ApplicationList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_PACK, "PackinglistList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_FILTER_GUIDE, "FilterGuideList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_WEBSERVICE, "WebserviceList", std::string(1, strValue.at(i)));
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

	//ActiveLink List
	docMain->ActiveLinkList("*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ActiveLinkList(std::string(1, strValue.at(i)));
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



	//Filter List
	docMain->FilterList("*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->FilterList(std::string(1, strValue.at(i)));
	}	
	docMain->FilterActionList();

	//Filter Details
	tmpCount = filterList.GetCount();
	cout << "Starting Filter Documentation" << endl;
	for (unsigned int filterIndex = 0; filterIndex < tmpCount; ++filterIndex)
	{
		LOG << "Filter [" << filterIndex << "-" << tmpCount << "] '" << filterList.FilterGetName(filterIndex) << "': ";
		CDocFilterDetails filterDetails(filterIndex);
		filterDetails.Documentation();
	}



	//Escalation List
	docMain->EscalationList("*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->EscalationList(std::string(1, strValue.at(i)));
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



	//CharMenus
	docMain->CharMenuList("*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->CharMenuList(std::string(1, strValue.at(i)));
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


	//Schema List
	docMain->SchemaList(AR_SCHEMA_NONE,    CPageParams(PAGE_OVERVIEW, AR_STRUCT_ITEM_XML_SCHEMA)      , "Formlist (All)"    , "*");
	docMain->SchemaList(AR_SCHEMA_REGULAR, CPageParams(PAGE_SCHEMA_REGULAR, AR_STRUCT_ITEM_XML_SCHEMA), "Formlist (Regular)", "*");
	docMain->SchemaList(AR_SCHEMA_JOIN,    CPageParams(PAGE_SCHEMA_JOIN, AR_STRUCT_ITEM_XML_SCHEMA)   , "Formlist (Join)"   , "*");
	docMain->SchemaList(AR_SCHEMA_VIEW,    CPageParams(PAGE_SCHEMA_VIEW, AR_STRUCT_ITEM_XML_SCHEMA)   , "Formlist (View)"   , "*");
	docMain->SchemaList(AR_SCHEMA_DIALOG,  CPageParams(PAGE_SCHEMA_DIALOG, AR_STRUCT_ITEM_XML_SCHEMA) , "Formlist (Dialog)" , "*");
	docMain->SchemaList(AR_SCHEMA_VENDOR,  CPageParams(PAGE_SCHEMA_VENDOR, AR_STRUCT_ITEM_XML_SCHEMA) , "Formlist (Vendor)" , "*");	
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->SchemaList(AR_SCHEMA_NONE, CPageParams(strValue.at(i), AR_STRUCT_ITEM_XML_SCHEMA), "Formlist", std::string(1, strValue.at(i)));
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
	// Image
	docMain->ImageList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ImageList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
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

	//Analyzer
	CDocAnalyzer *analyzer = new CDocAnalyzer();
	analyzer->Documentation();
	delete analyzer;

	//Validation
	CDocValidator *validator = new CDocValidator();
	validator->Main();
	delete validator;

	//Group List
	docMain->GroupList("*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->GroupList(std::string(1, strValue.at(i)));
	}	


	//Group Details
	nTmpCnt=1;
	list<CARGroup>::iterator groupIter;
	cout << "Starting Group Documentation" << endl;
	for ( groupIter = this->groupList.begin(); groupIter != this->groupList.end(); groupIter++ )
	{
		CARGroup *grp = &(*groupIter);

		LOG << "Group [" << nTmpCnt << "-" << groupList.size() << "] '" << grp->name << "': ";
		CDocGroupDetails *grpDetails = new CDocGroupDetails(*grp);
		grpDetails->Documentation();
		delete grpDetails;
		nTmpCnt++;
	}


	//Role List
	docMain->RoleList("*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->RoleList(std::string(1, strValue.at(i)));
	}	

	//Role Details
	nTmpCnt=1;
	list<CARRole>::iterator roleIter;
	cout << "Starting Role Documentation" << endl;
	for ( roleIter = this->roleList.begin(); roleIter != this->roleList.end(); roleIter++ )
	{
		CARRole *role = &(*roleIter);

		LOG << "Role [" << nTmpCnt << "-" << roleList.size() << "] '" << role->name << "': ";
		CDocRoleDetails *roleDetails = new CDocRoleDetails(*role);
		roleDetails->Documentation();
		delete roleDetails;
		nTmpCnt++;
	}


	//Userlists
	docMain->UserList("*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->UserList(std::string(1, strValue.at(i)));
	}	

	//User Details	list<CARUser>::iterator userIter;
	nTmpCnt=1;
	list<CARUser>::iterator userIter;
	cout << "Starting User Documentation" << endl;
	for ( userIter = this->userList.begin(); userIter != this->userList.end(); userIter++ )
	{
		CARUser *user = &(*userIter);

		LOG << "User [" << nTmpCnt << "-" << userList.size() << "] '" << user->loginName << "': ";
		CDocUserDetails *userDetails = new CDocUserDetails(*user);
		userDetails->Documentation();
		delete userDetails;
		nTmpCnt++;
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
				return limits.u.enumLimits.u.regularList.nameList[enumPosition];
				break;
			case AR_ENUM_STYLE_CUSTOM:
				for (unsigned int i=0; i < limits.u.enumLimits.u.customList.numItems; i++) { 
					if (limits.u.enumLimits.u.customList.enumItemList[i].itemNumber == enumPosition) 
						return limits.u.enumLimits.u.customList.enumItemList[i].itemName; 
				} 
				return "";
			case AR_ENUM_STYLE_QUERY:
				return "QUERY";
			}						
		}
	}
	return EmptyValue;
}

string CARInside::LinkToField(string schemaName, int fieldInsideId, int fromRootLevel)
{	
	return LinkToField(SchemaGetInsideId(schemaName), fieldInsideId, fromRootLevel);
}

string CARInside::LinkToField(int schemaInsideId, int fieldInsideId, int fromRootLevel, bool needValidField)
{	
	CARField field(schemaInsideId, fieldInsideId);
	if (field.Exists())
	{
		return field.GetURL(fromRootLevel);
	}

	//Field has not been found
	stringstream tmp;

	if(needValidField && fieldInsideId > 0 && schemaInsideId > -1) //OpenWindow uses 0 what is no valid field
	{
		CFieldRefItem *refItemNotFound = new CFieldRefItem();
		refItemNotFound->arsStructItemType = AR_STRUCT_ITEM_XML_FIELD;
		refItemNotFound->fromName = "";						
		refItemNotFound->schemaInsideId = schemaInsideId;
		refItemNotFound->fieldInsideId = fieldInsideId;
		refItemNotFound->description = "Field does not exist";	
		this->listFieldNotFound.push_back(*refItemNotFound);
		delete refItemNotFound;

		tmp << "<span class=\"fieldNotFound\">" << fieldInsideId << "</span>";
	}
	else
	{
		tmp << fieldInsideId;
	}

	return tmp.str();
}

// TODO: maybe change callers to LinkToField and remove this function completely
string CARInside::LinkToMenuField(int schemaInsideId, int fieldInsideId, int fromRootLevel)
{
	return LinkToField(schemaInsideId, fieldInsideId, fromRootLevel, false);
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

string CARInside::LinkToSchema(string schemaName, int fromRootLevel)
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
	list<CARUser>::iterator userIter;			
	for ( userIter = userList.begin(); userIter != userList.end(); userIter++ )
	{	
		CARUser *user = &(*userIter);
		if(loginName == user->name)
		{
			return CWebUtil::Link(loginName, CPageParams(PAGE_DETAILS, user), "", rootLevel);
		}
	}
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
		list<CARRole>::iterator roleIter;
		for(roleIter = this->roleList.begin(); roleIter != this->roleList.end(); roleIter++)
		{
			CARRole *role = &(*roleIter);
			if(role->roleId == permissionId && strcmp(role->applicationName.c_str(), appRefName.c_str())==0)
			{
				return true;
			}
		}
	}

	return false;
}

string CARInside::LinkToGroup(string appRefName, int permissionId, int rootLevel)
{	
	stringstream strmTmp;
	strmTmp.str("");
	strmTmp << permissionId;

	if(permissionId >= 0)
	{
		list<CARGroup>::iterator grpIter;			
		for ( grpIter = this->groupList.begin(); grpIter != this->groupList.end(); grpIter++ )
		{	
			CARGroup *group = &(*grpIter);		
			if(group->groupId == permissionId)
			{
				return CWebUtil::Link(group->groupName, CPageParams(PAGE_DETAILS, group), "group.gif", rootLevel);
			}		
		}
	}
	else
	{
		if(appRefName.c_str() != NULL && strcmp(appRefName.c_str(), "") != 0)
		{
			list<CARRole>::iterator roleIter;
			for(roleIter = this->roleList.begin(); roleIter != this->roleList.end(); roleIter++)
			{
				CARRole *role = &(*roleIter);
				if(role->roleId == permissionId && strcmp(role->applicationName.c_str(), appRefName.c_str())==0)
				{
					return CWebUtil::Link(role->roleName, CPageParams(PAGE_DETAILS, role), "doc.gif", rootLevel);
				}
			}
		}
	}

	return strmTmp.str();
}

string CARInside::LinkToAlRef(int alInsideId, int rootLevel)
{
	if (alInsideId < 0) return EmptyValue;

	CARActiveLink al(alInsideId);
	return LinkToAlRef(al, rootLevel);
}

string CARInside::LinkToAlRef(const string &alName, int rootLevel)
{
	if (alName.empty()) return EmptyValue;

	int alInsideId = alList.Find(alName.c_str());
	if (alInsideId > -1)
		return LinkToAlRef(alInsideId, rootLevel);

	return EmptyValue;
}

string CARInside::LinkToAlRef(CARActiveLink &al, int rootLevel)
{
	stringstream strmTmp;
	strmTmp.str("");

	strmTmp << al.GetURL(rootLevel) << " (" << al.GetOrder() << ")";
	return strmTmp.str();
}

string CARInside::LinkToAl(string alName, int fromRootLevel)
{
	if (alName.empty()) return alName;

	int alInsideId = alList.Find(alName.c_str());
	if (alInsideId > -1)
	{
		CARActiveLink al(alInsideId);
		return al.GetURL(fromRootLevel);
	}

	return alName;
}

string CARInside::LinkToAl(int alInsideId, int rootLevel)
{
	if (alInsideId > -1)
	{
		CARActiveLink al(alInsideId);
		return al.GetURL(rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToFilterRef(int filterInsideId, int rootLevel)
{	
	if (filterInsideId > -1)
	{
		CARFilter flt(filterInsideId);
		return LinkToFilterRef(&flt, rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToFilterRef(string fltName, int rootLevel)
{
	if (fltName.empty()) return fltName;

	int fltInsideId = filterList.Find(fltName.c_str());
	if (fltInsideId > -1)
	{
		CARFilter flt(fltInsideId);
		return LinkToFilterRef(&flt, rootLevel);
	}
	return fltName;
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

string CARInside::LinkToMenu(string menuName, int fromRootLevel, bool* bFound)
{
	if (menuName.compare("$NULL$")==0)
	{
		if (bFound) *bFound = true;
		return menuName;
	}

	CARCharMenu menu(menuName);
	if (menu.Exists())
	{
		if (bFound) { *bFound = true; }
		return menu.GetURL(fromRootLevel);
	}

	//Menu has not been found
	if (bFound) { *bFound = false; }

	//if the menu is missing, we just return the name of it. maybe link to the
	//"missing menus" page (must be implemented first) of CDocValidator later.
	return "<span class=\"fieldNotFound\">" + menuName + "</span>";
}

string CARInside::LinkToEscalation(string escalationName, int fromRootLevel)
{
	int objInsideId = escalationList.Find(escalationName.c_str());

	if (objInsideId > -1)
	{
		CAREscalation escal(objInsideId);
		return escal.GetURL(fromRootLevel);
	}
	return escalationName;
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

string CARInside::LinkToXmlObjType(int arsStructItemType, const string &objName, int rootLevel)
{
	return LinkToXmlObjType(arsStructItemType, objName, -1, rootLevel);
}

string CARInside::LinkToXmlObjType(int arsStructItemType, const string &objName, int subObjId, int rootLevel)
{
	string result = EmptyValue;

	switch(arsStructItemType)
	{
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK: 
		{
			result = this->LinkToAlRef(objName, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_FILTER:
		{
			result = this->LinkToFilterRef(objName, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_SCHEMA:
		{
			result = this->LinkToSchema(objName, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_FIELD:
		{
			result = this->LinkToField(objName, subObjId, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_ESCALATION:
		{
			result = this->LinkToEscalation(objName, rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_CHAR_MENU:
		{
			result = this->LinkToMenu(objName, rootLevel);
		}
		break;		
	case AR_STRUCT_ITEM_XML_CONTAINER:
		{
			result = this->LinkToContainer(objName, rootLevel);
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
	string result = "";

	switch(arsStructItemType)
	{
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK: 
		{
			int alInsideId = alList.Find(objName.c_str());
			if (alInsideId > -1)
			{
				return CAREnum::ObjectEnable(alList.ActiveLinkGetEnabled(alInsideId));
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

	return result;
}


void CARInside::SearchCustomFieldReferences()
{
	try
	{
		cout << "Checking field references [";

		unsigned int schemaCount = schemaList.GetCount();
		for (unsigned int schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
		{			
			CARSchema schema(schemaIndex);

			unsigned int fieldCount = schema.GetFields()->GetCount();
			for (unsigned int fieldIndex = 0; fieldIndex < fieldCount; ++fieldIndex)
			{
				CARField field(schemaIndex, 0, fieldIndex);
				CustomFieldReferences(schema, field);
			}

			cout << ".";
		}

		cout << "]" << endl;
	}
	catch(exception& e)
	{
		cout << "EXCEPTION SearchCustomFieldReferences: " << e.what() << endl;
	}
}

//Find references at startup
//This function is necessary because when we would try to add the reference at runtime for example
//Columns might be enumerated at a time when the datafield has been already saved to html file
void CARInside::CustomFieldReferences(const CARSchema &schema, const CARField &obj)
{
	try
	{
		int rootLevel = 2;

		stringstream strm;
		strm.str("");

		const ARFieldLimitStruct& objLimits = obj.GetLimits();
		switch(objLimits.dataType)
		{		
		case AR_DATA_TYPE_COLUMN: //Find column references
			{
				const ARColumnLimitsStruct& fLimit = objLimits.u.columnLimits;

				//To create a link to the datafield we first must find the target schema of the table
				string tableSchema;                    // the name of table source form

				CARField tableField(schema.GetInsideId(), fLimit.parent);
				if (tableField.Exists())
				{
					const ARFieldLimitStruct& limits = tableField.GetLimits();
					if (tableField.GetDataType() == AR_DATA_TYPE_TABLE && limits.dataType == AR_DATA_TYPE_TABLE)
					{
						tableSchema = limits.u.tableLimits.schema;

						if (!tableSchema.empty() && tableSchema[0] == '$')
							tableSchema = limits.u.tableLimits.sampleSchema;

						if (tableSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
							tableSchema = schema.GetARName();

						// now get the table source form (if exists)
						CARSchema tableSourceSchema(tableSchema);

						if (tableSourceSchema.Exists())
						{
							stringstream tmpDesc;
							tmpDesc << "Column in Table " + LinkToField(schema.GetInsideId(), tableField.GetFieldId(), rootLevel) << " of Form " << LinkToSchema(tableSourceSchema.GetInsideId(), rootLevel);
							CFieldRefItem refItem(AR_STRUCT_ITEM_XML_FIELD, schema.GetName(), tmpDesc.str(), fLimit.dataField, tableSourceSchema.GetInsideId());
							refItem.fromFieldId = obj.GetFieldId();
							this->AddReferenceItem(&refItem);
						}
					}
				}
			}
			break;
		case AR_DATA_TYPE_TABLE:
			{
				const ARTableLimitsStruct fLimit = objLimits.u.tableLimits;

				string tableSchema;                    // the name of table source form

				tableSchema = fLimit.schema;

				if (!tableSchema.empty() && tableSchema[0] == '$')
					tableSchema = fLimit.sampleSchema;

				if (tableSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
					tableSchema = schema.GetARName();

						// now get the table source form (if exists)
				CARSchema tableSourceSchema(tableSchema);

				if (tableSourceSchema.Exists() && fLimit.qualifier.operation != AR_COND_OP_NONE)
				{		
					stringstream strmQuery;
					CFieldRefItem refItem(AR_STRUCT_ITEM_XML_FIELD, schema.GetARName(), "Table Qualification", 0, 0);
					refItem.fromFieldId = obj.GetFieldId();

					CARQualification arQual(*this);
					int pFormId = schema.GetInsideId();
					int sFormId = tableSourceSchema.GetInsideId();

					arQual.CheckQuery(&fLimit.qualifier, refItem, 0, pFormId, sFormId, strmQuery, rootLevel);
				}
			}
			break;
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION CustomFieldReferences: " << e.what() << endl;
	}
}

void CARInside::AddReferenceItem(CFieldRefItem *refItem)
{
	try
	{
		if(refItem->schemaInsideId > -1 && refItem->arsStructItemType != AR_STRUCT_ITEM_XML_NONE)
		{
			this->listFieldRefItem.insert(this->listFieldRefItem.end(), *refItem);
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION AddReferenceItem: " << e.what() << endl;
	}
}

void CARInside::AddMissingMenu(const CMissingMenuRefItem& refItem)
{
	try
	{
		list<CMissingMenuRefItem>::iterator mIter = listMenuNotFound.begin();
		list<CMissingMenuRefItem>::iterator endIt = listMenuNotFound.end();
		for ( ; mIter != endIt; ++mIter)
		{
			CMissingMenuRefItem current = *mIter;
			if (current == refItem)
			{
				return;
			}
		}
		listMenuNotFound.push_back(refItem);
	}
	catch (...)
	{
		cout << "EXCEPTION in AddMissingMenu" << endl;
	}
}

string CARInside::TextFindFields(string inText, string fieldSeparator, int schemaInsideId, int rootLevel, bool findKeywords, CFieldRefItem *refItem)
{	
	try
	{
		if(inText.size() == 0)
			return "";

		stringstream strmTmp;
		unsigned int curPos = 0;

		string dummySeparator = "##"; //dummy placeholder to avoid infinite replacements

		CARSchema schema(schemaInsideId);
		if (!schema.Exists())
			return inText;

		unsigned int startPos = 0;
		unsigned int maxLen = (unsigned int)inText.length();
		unsigned int fieldIdPos = 0;

		char fieldId[20];
		char *enumId;    // points to the enum part of status history within fieldId later
		char *usrOrTime; // points to the "user or time" part of status history within fieldId later

		while ((startPos = (unsigned int)inText.find(fieldSeparator.at(0),curPos)) != std::string::npos)
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
						refItem->fieldInsideId = field.GetFieldId();
						AddReferenceItem(refItem);

						// special handling for status history
						if (iFieldId == 15)
						{
							if (enumId == NULL || usrOrTime == NULL) break;

							// resolve user or time attribute
							int iUsrOrTime = atoi(usrOrTime);
							char* usrOrTimeStr = usrOrTime;
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
							if (fieldStatus.Exists())
							{
								int iEnumId = atoi(enumId);
								strmTmp << "." << GetFieldEnumValue(schemaInsideId, fieldStatus.GetInsideId(), iEnumId) << "." << usrOrTimeStr;
							}
							else
								strmTmp << "." << enumId << "." << usrOrTimeStr;
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

		//write the current string back ot inText, then clear the stream for use with the application code
		inText = strmTmp.str();
		strmTmp.clear();
		strmTmp.str("");

		//cout << refItem->fromName << endl;

		if (getPos(inText,"Application-Copy-Field-Value "))
		{
			strmTmp << processTwoFields("Application-Copy-Field-Value", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Delete-Entry "))
		{
			strmTmp << processForm("Application-Delete-Entry", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Format-Qual "))
		{
			strmTmp << processForm("Application-Format-Qual", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Format-Qual-Filter "))
		{
			strmTmp << processForm("Application-Format-Qual-Filter", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Format-Qual-ID "))
		{
			strmTmp << processForm("Application-Format-Qual-ID", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Format-Qual-L "))
		{
			strmTmp << processForm("Application-Format-Qual-L", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Get-Approval-Join "))
		{
			strmTmp << processForm("Application-Get-Approval-Join", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Get-Approval-Join2 "))
		{
			strmTmp << processForm("Application-Get-Approval-Join2", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Get-DetSig-Join "))
		{
			strmTmp << processForm("Application-Get-DetSig-Join", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Get-DetSig-Join2 "))
		{
			strmTmp << processForm("Application-Get-DetSig-Join2", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Get-Form-Alias "))
		{
			strmTmp << processForm("Application-Get-Form-Alias", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Get-Locale-VuiID "))
		{
			strmTmp << processForm("Application-Get-Locale-VuiID", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Get-Next-Recurrence-Time "))
		{
			strmTmp << processForm("Application-Get-Next-Recurrence-Time", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Map-Ids-To-Names "))
		{
			strmTmp << processForm("Application-Map-Ids-To-Names", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Map-Ids-To-Names-L "))
		{
			strmTmp << processForm("Application-Map-Ids-To-Names-L", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Map-Names-To-Ids "))
		{
			strmTmp << processForm("Application-Map-Names-To-Ids", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Map-Names-To-Ids-L "))
		{
			strmTmp << processForm("Application-Map-Names-To-Ids-L", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Parse-Qual "))
		{
			strmTmp << processForm("Application-Parse-Qual", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Parse-Qual-Filter "))
		{
			strmTmp << processForm("Application-Parse-Qual-Filter", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Parse-Qual-L "))
		{
			strmTmp << processForm("Application-Parse-Qual-L", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Parse-Qual-SField-L "))
		{
			strmTmp << processForm("Application-Parse-Qual-SField-L", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Parse-Val-SField "))
		{
			strmTmp << processForm("Application-Parse-Val-SField", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"Application-Query-Delete-Entry "))
		{
			strmTmp << processForm("Application-Query-Delete-Entry", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-ACTIVE-LINK "))
		{
			strmTmp << processSecondParameter("PERFORM-ACTION-ACTIVE-LINK", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-ADD-ATTACHMENT "))
		{
			strmTmp << processOneField("PERFORM-ACTION-ADD-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-DELETE-ATTACHMENT "))
		{
			strmTmp << processOneField("PERFORM-ACTION-DELETE-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-GET-FIELD-LABEL "))
		{
			strmTmp << processOneField("PERFORM-ACTION-GET-FIELD-LABEL", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-GET-PREFERENCE "))
		{
			strmTmp << processOneField("PERFORM-ACTION-GET-PREFERENCE", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-NAV-FIELD-SET-SELECTED-ITEM "))
		{
			strmTmp << processOneField("PERFORM-ACTION-NAV-FIELD-SET-SELECTED-ITEM", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-OPEN-ATTACHMENT "))
		{
			strmTmp << processOneField("PERFORM-ACTION-OPEN-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-SAVE-ATTACHMENT "))
		{
			strmTmp << processOneField("PERFORM-ACTION-SAVE-ATTACHMENT", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-SET-PREFERENCE "))
		{
			strmTmp << processOneField("PERFORM-ACTION-SET-PREFERENCE", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-CLEAR "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-CLEAR", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-CLEAR-ROWCHANGED "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-CLEAR-ROWCHANGED", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-DESELECTALL "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-DESELECTALL", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-GET-SELECTED-COLUMN "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-GET-SELECTED-COLUMN", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-IS-LEAF-SELECTED "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-IS-LEAF-SELECTED", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-NEXT-CHUNK "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-NEXT-CHUNK", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-PREV-CHUNK "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-PREV-CHUNK", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-REFRESH "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-REFRESH", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-REPORT "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-REPORT", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-SELECT-NODE "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-SELECT-NODE", inText, schemaInsideId, rootLevel, refItem);
		}
		else if (getPos(inText,"PERFORM-ACTION-TABLE-SELECTALL "))
		{
			strmTmp << processOneField("PERFORM-ACTION-TABLE-SELECTALL", inText, schemaInsideId, rootLevel, refItem);
		}
		else
		{
			strmTmp << inText;
		}

		return strmTmp.str();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in TextFindField: " << e.what() << endl;
	}

	return inText;
}


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


string CARInside::XMLFindFields(string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem)
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
				refItem->fieldInsideId = field.GetFieldId();
				AddReferenceItem(refItem);
			}
		}
	}
	catch(exception& e)
	{
		cout << "EXCEPTION in XMLFindField: " << e.what() << endl;
	}

	return inText;
}

string CARInside::DataObjectHistory(CARDataObject *obj, int rootLevel)
{		
	stringstream strm;
	strm.str("");

	try
	{				
		CTable tbl("history", "TblObjectHistory");
		tbl.AddColumn(20, "Description");
		tbl.AddColumn(80, "Value");

		tbl.description = CWebUtil::ImageTag("doc.gif", rootLevel) + "Change History";
		stringstream strmTmp;	

		//Created
		strmTmp << CUtil::DateTimeToHTMLString(obj->created) << " " << this->LinkToUser(obj->createdBy, rootLevel);

		CTableRow tblRow("");
		tblRow.AddCellList(CTableCell("Created"), CTableCell(strmTmp.str()));
		tbl.AddRow(tblRow);


		//Modified
		strmTmp.str("");
		strmTmp << CUtil::DateTimeToHTMLString(obj->modified) << " " << this->LinkToUser(obj->modifiedBy, rootLevel);

		tblRow.AddCellList(CTableCell("Modified"), CTableCell(strmTmp.str()));
		tbl.AddRow(tblRow);

		strm << tbl.ToXHtml();
		tbl.Clear();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION writing dataobject history: " << e.what() << endl;
	}	

	return strm.str();		
}

string CARInside::ServerObjectHistory(CARServerObject *obj, int rootLevel)
{		
	stringstream strm;
	strm.str("");

	try
	{
		list<CChangeHistoryEntry> listHistoryEntry;
		if(obj->GetChangeDiary() != NULL)
		{				
			ARDiaryList diaryList;
			if(ARDecodeDiary(&this->arControl, const_cast<char*>(obj->GetChangeDiary()), &diaryList, &this->arStatus)==AR_RETURN_OK)
			{
				if(diaryList.diaryList != NULL)
				{			
					for(unsigned int j=0; j<diaryList.numItems; j++)
					{
						CChangeHistoryEntry entry(diaryList.diaryList[j].timeVal, diaryList.diaryList[j].user, diaryList.diaryList[j].value);		
						listHistoryEntry.push_back(entry);
					}
				}
			}
			FreeARDiaryList(&diaryList, false);				
		}

		stringstream historyLog;
		if(listHistoryEntry.size() > 0)
		{
			list<CChangeHistoryEntry>::iterator listIter;		
			for ( listIter = listHistoryEntry.begin(); listIter != listHistoryEntry.end(); listIter++ )
			{	
				CChangeHistoryEntry historyEntry = (CChangeHistoryEntry) *listIter;
				historyLog << CUtil::DateTimeToHTMLString(historyEntry.ts) << " " << this->LinkToUser(historyEntry.user, rootLevel) << "<br/>" << endl;
				historyLog << historyEntry.entry << "<br/><br/>" << endl;
			}
		}
		else
		{
			historyLog << EmptyValue << endl;
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
		string tmpHelptext = EmptyValue;
		if(obj->GetHelpText() != NULL)
		{
			tmpHelptext = CUtil::StrReplace("\n", "<br/>", obj->GetHelpText());
		}

		tblRow.AddCellList(CTableCell("Helptext"), CTableCell(tmpHelptext));
		tbl.AddRow(tblRow);

		strm << tbl.ToXHtml();
		tbl.Clear();
	}
	catch(exception& e)
	{
		cout << "EXCEPTION writing server object history: " << e.what() << endl;
	}	

	return strm.str();
}

void CARInside::BuildReferences()
{
	SearchCustomFieldReferences();
	SearchFilterReferences();
}

void CARInside::SearchFilterReferences()
{
	typedef map< string,list<string> > ErrorCallMap;
	typedef pair< string,list<string> > ErrCallPair;

	ErrorCallMap errorCalls;

	try
	{
		cout << "Checking filter references";

		unsigned int filterCount = filterList.GetCount();
		for (unsigned int filterIndex = 0; filterIndex < filterCount; filterIndex++)
		{
			CARFilter filter(filterIndex);
			if (filter.GetErrorOption() == AR_FILTER_ERRHANDLER_ENABLE)
			{
				ErrorCallMap::iterator item = errorCalls.find(filter.GetErrorHandler());
				if (item == errorCalls.end())
				{
					ErrCallPair newItem(filter.GetErrorHandler(), list<string>());
					newItem.second.insert(newItem.second.end(), filter.GetName());
					errorCalls.insert(newItem);
				}
				else
				{
					item->second.insert(item->second.end(), filter.GetName());
				}
			}
		}

		if (errorCalls.size() > 0)
		{
			for (unsigned int filterIndex = 0; filterIndex < filterCount; filterIndex++)
			{
				CARFilter filter(filterIndex);
				ErrorCallMap::iterator item = errorCalls.find(filter.GetName());
				if (item != errorCalls.end())
				{
					if (!item->second.empty())
					{
						filter.ErrorCallers().reserve(item->second.size());
						list<string>::iterator callerEnd = item->second.end();
						for (list<string>::iterator caller = item->second.begin(); caller != callerEnd; ++caller)
						{
							filter.ErrorCallers().push_back(*caller);
						}
					}
				}
			}
		}

		cout << endl;
	}
	catch (exception &e)
	{
		cerr << "EXCEPTION SearchFilterReferences: " << e.what() << endl;
	}
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
	BuildReferences();

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

string CARInside::processOneField(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem)
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
string CARInside::processTwoFields(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem)
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
string CARInside::processForm(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem)
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
string CARInside::processSecondParameter(string command, string inText, int schemaInsideId, int rootLevel, CFieldRefItem *refItem)
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
bool CARInside::getPos(string inText, string findText)
{
	size_t pos = inText.find(findText);
	bool found = false;
	if (pos != std::string::npos)
		found = true;
	else
		pos = 0;

	return found;
}

string CARInside::refFieldID(int iFieldId, int schemaInsideId, int rootLevel, CFieldRefItem *refItem)
{
	stringstream strmTmp;

	//add the reference
	try {
		CARField field(schemaInsideId, iFieldId);
		if (field.Exists())
		{
			strmTmp << field.GetURL(rootLevel);
			refItem->fieldInsideId = field.GetInsideId();
			AddReferenceItem(refItem);
		}
		else
			strmTmp << iFieldId;
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
	return imageList.ImageGetURL(imageIndex, rootLevel);
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

