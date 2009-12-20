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

/////////
// the following file is generated via a pre-build step using "svnrev_template.h" as template. 
// The $WCREV$ keyword of the template is replaced with the revision number.
#include "svnrev.h"

/////////
// version information block
#define VERSION "3.0.0"
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
	this->schemaList.clear();
	this->filterList.clear();
	this->escalList.clear();
	this->containerList.clear();
	this->menuList.clear();
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

	if (CARInside::pInsideInstance == NULL) 
		CARInside::pInsideInstance = this;
}

CARInside::~CARInside(void)
{
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
	FreeARStatusList(&this->arStatus, false);
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
	CWindowsUtil wUtil(this->appConfig);

	CDocMain *docMain = new CDocMain(*this);

	if( docMain->Index() == 1)
	{
		wUtil.CreateSubDirectory("active_link");
		wUtil.CreateSubDirectory("active_link_guide");
		wUtil.CreateSubDirectory("application");
		wUtil.CreateSubDirectory("escalation");
		wUtil.CreateSubDirectory("filter");
		wUtil.CreateSubDirectory("filter_guide");
		wUtil.CreateSubDirectory("group");
		wUtil.CreateSubDirectory("image");
		wUtil.CreateSubDirectory("img");
		wUtil.CreateSubDirectory("menu");
		wUtil.CreateSubDirectory("other");
		wUtil.CreateSubDirectory("packing_list");
		wUtil.CreateSubDirectory("schema");
		wUtil.CreateSubDirectory("template");
		wUtil.CreateSubDirectory("user");
		wUtil.CreateSubDirectory("role");
		wUtil.CreateSubDirectory("webservice");
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

		/*
		ARStructItemList objectsToParse;
		objectsToParse.numItems = 1;
		objectsToParse.structItemList = (ARStructItemStruct *) malloc (sizeof(ARStructItemStruct)* objectsToParse.numItems);		
		objectsToParse.structItemList[0].type = AR_STRUCT_ITEM_XML_SCHEMA;
		strcpy(objectsToParse.structItemList[0].name, "xxTest");
		*/

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
			unsigned int arInsideIdSchema = 0;
			unsigned int arInsideIdAl = 0;
			unsigned int arInsideIdFilter = 0;
			unsigned int arInsideIdEscal = 0;
			unsigned int arInsideIdCont = 0;
			unsigned int arInsideIdMenu = 0;
			unsigned int arInsideIdImage = 0;

			unsigned int imagesCount = 0;
			unsigned int activelinkCount = 0;
			for (unsigned int i=0; i < parsedObjects.numItems; ++i)
			{
				switch (parsedObjects.structItemList[i].type)
				{
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
					++activelinkCount;
					break;
#if AR_CURRENT_API_VERSION >= AR_API_VERSION_750
				case AR_STRUCT_ITEM_XML_IMAGE:
					++imagesCount; 
					break;
#endif
				}
			}

			if (activelinkCount > 0) alList.Reserve(activelinkCount);
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
						CARFilter *obj = new CARFilter(parsedObjects.structItemList[i].name, arInsideIdFilter);

						if( ARGetFilterFromXML(&this->arControl, 
							&parsedStream,
							parsedObjects.structItemList[i].name,
							NULL, 
							&obj->order,
							&obj->schemaList,								
							&obj->opSet,
							&obj->enable,								
							&obj->query,
							&obj->actionList,
							&obj->elseList,
							obj->owner,
							obj->lastChanged,
							&obj->timestamp,								
							&obj->helptext,
							&obj->changeDiary,
							&obj->objPropList,
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
							&obj->errorOptions,
							obj->errorFilterName,
#endif
							&obj->xmlDocVersion,
							&this->arStatus) == AR_RETURN_OK)
						{
							ParseVersionString(obj->xmlDocVersion);

							this->filterList.insert(this->filterList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdFilter << ") [OK]" << endl;
							arInsideIdFilter++;
						}
						else
							cerr << GetARStatusError();

						delete obj;
						FreeARStatusList(&this->arStatus, false);
					}
					break;
				case AR_STRUCT_ITEM_XML_SCHEMA:
					{
						LOG << "Loading Form: " << parsedObjects.structItemList[i].name; 
						CARSchema *schema = new CARSchema(parsedObjects.structItemList[i].name, arInsideIdSchema);

						ARFieldInfoList fieldInfoList;
						ARVuiInfoList vuiInfoList;

						if( ARGetSchemaFromXML(&this->arControl, 
							&parsedStream,
							parsedObjects.structItemList[i].name,
							NULL, 
							&schema->schema,
							&schema->groupList,
							&schema->admingrpList,
							&schema->getListFields,
							&schema->sortList,
							&schema->indexList,
							&schema->archiveInfo,
							&schema->auditInfo,
							schema->defaultVui,
							NULL,
							NULL,
							NULL,
							&fieldInfoList,
							&vuiInfoList,								
							schema->owner,
							schema->lastChanged,
							&schema->timestamp,								
							&schema->helptext,
							&schema->changeDiary,
							&schema->objPropList,
							&schema->xmlDocVersion,
							&this->arStatus) == AR_RETURN_OK)
						{										
							ParseVersionString(schema->xmlDocVersion);

							//Fields
							for(unsigned int nField = 0; nField < fieldInfoList.numItems; nField++)
							{									
								ARFieldInfoStruct tmpField = fieldInfoList.fieldList[nField];
								LOG << "Loading Field: " << tmpField.fieldName;	

								CARField *field = new CARField(tmpField.fieldId);

								field->schemaInsideId = arInsideIdSchema;
								field->changeDiary = tmpField.changeDiary;
								field->createMode = tmpField.createMode;
								field->dataType = tmpField.dataType;
								field->defaultVal = tmpField.defaultVal;
								field->dInstanceList = tmpField.dInstanceList;
								field->fieldId = tmpField.fieldId;
								field->fieldMap = tmpField.fieldMap;
								strncpy(field->fieldName,tmpField.fieldName, AR_MAX_NAME_SIZE);
								field->helptext = tmpField.helpText;
								strncpy(field->lastChanged,tmpField.lastChanged, AR_MAX_NAME_SIZE);								
								field->name = tmpField.fieldName;
								field->option = tmpField.option;
								strncpy(field->owner,tmpField.owner, AR_MAX_NAME_SIZE);
								field->permissions = tmpField.permList;
								field->timestamp = tmpField.timestamp;
								field->xmlDocVersion = schema->xmlDocVersion;
								field->limit = tmpField.limit;

								schema->fieldList.insert(schema->fieldList.end(), *field);

								//Check if this is a global field and add it to the globalfieldlist
								if(field->fieldId >= 1000000 &&  field->fieldId <= 1999999)
								{
									CARGlobalField *globalField = new CARGlobalField(schema->GetInsideId(), field->GetInsideId(), field->fieldId);
									this->globalFieldList.insert(this->globalFieldList.end(), *globalField);
									delete globalField;
								}	

								delete field;
								LOG << " [OK]" << endl;												
							}

							//VuiList
							for(unsigned int nViewCnt=0; nViewCnt < vuiInfoList.numItems; nViewCnt++)
							{
								ARVuiInfoStruct tmpVui = vuiInfoList.vuiList[nViewCnt];
								CARVui *vui = new CARVui(tmpVui.vuiId);

								vui->vuiType = tmpVui.vuiType;
								vui->objPropList = tmpVui.props;
								vui->name = tmpVui.vuiName;								
								vui->changeDiary = tmpVui.changeDiary;
								vui->helptext = tmpVui.helpText;
								strncpy(vui->lastChanged,tmpVui.lastChanged, AR_MAX_NAME_SIZE);
								strncpy(vui->owner,tmpVui.owner, AR_MAX_NAME_SIZE);
								vui->timestamp = tmpVui.timestamp;
								vui->xmlDocVersion = schema->xmlDocVersion;
								vui->schemaInsideId = arInsideIdSchema;
								schema->vuiList.insert(schema->vuiList.end(), *vui);

								delete vui;
							}

							this->schemaList.insert(this->schemaList.end(), *schema);

							LOG << " (InsideID: " << arInsideIdSchema << ") [OK]" << endl;
							arInsideIdSchema++;
						}
						else
							cerr << GetARStatusError();

						delete schema;
						//FreeARFieldInfoList(&fieldInfoList, false);
						//FreeARVuiInfoList(&vuiInfoList, false);
						FreeARStatusList(&this->arStatus, false);

					}
					break;					
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
					{
						LOG << "Loading ActiveLink: " << parsedObjects.structItemList[i].name; 

						int objInsideId = alList.AddActiveLinkFromXML(parsedStream, parsedObjects.structItemList[i].name, &xmlDocVersion);

						if (objInsideId > -1)
						{
							ParseVersionString(xmlDocVersion);
							LOG << " (InsideID: " << arInsideIdAl << ") [OK]" << endl;
						}
					}
					break;
				case AR_STRUCT_ITEM_XML_CHAR_MENU:
					{
						LOG << "Loading CharMenu: " << parsedObjects.structItemList[i].name; 
						CARCharMenu *obj = new CARCharMenu(parsedObjects.structItemList[i].name, arInsideIdMenu);

						if( ARGetMenuFromXML(&this->arControl, 
							&parsedStream,
							parsedObjects.structItemList[i].name,
							NULL,							
							&obj->refreshCode,
							&obj->menuDefn,
							obj->owner,
							obj->lastChanged,
							&obj->timestamp,								
							&obj->helptext,
							&obj->changeDiary,
							&obj->objPropList,
							&obj->xmlDocVersion,
							&this->arStatus) == AR_RETURN_OK)
						{
							ParseVersionString(obj->xmlDocVersion);

							this->menuList.insert(this->menuList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdMenu << ") [OK]" << endl;
							arInsideIdMenu++;								
						}
						else
							cerr << GetARStatusError();

						delete obj;
						FreeARStatusList(&this->arStatus, false);
					}
					break;
				case AR_STRUCT_ITEM_XML_ESCALATION:
					{
						LOG << "Loading Escalation: " << parsedObjects.structItemList[i].name; 
						CAREscalation *obj = new CAREscalation(parsedObjects.structItemList[i].name, arInsideIdEscal);

						if( ARGetEscalationFromXML(&this->arControl, 
							&parsedStream,
							parsedObjects.structItemList[i].name,
							NULL,							
							&obj->escalationTm,
							&obj->schemaList,								
							&obj->enable,                               								
							&obj->query,								
							&obj->actionList,
							&obj->elseList,
							obj->owner,
							obj->lastChanged,
							&obj->timestamp,								
							&obj->helptext,
							&obj->changeDiary,
							&obj->objPropList,
							&obj->xmlDocVersion,
							&this->arStatus) == AR_RETURN_OK)
						{
							ParseVersionString(obj->xmlDocVersion);

							this->escalList.insert(this->escalList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdEscal << ") [OK]" << endl;
							arInsideIdEscal++;								
						}
						else
							cerr << GetARStatusError();

						delete obj;
						FreeARStatusList(&this->arStatus, false);
					}
					break;
				case AR_STRUCT_ITEM_XML_CONTAINER:
					{
						LOG << "Loading Container: " << parsedObjects.structItemList[i].name; 
						CARContainer *obj = new CARContainer(parsedObjects.structItemList[i].name, arInsideIdCont);

						if( ARGetContainerFromXML(&this->arControl, 
							&parsedStream,
							parsedObjects.structItemList[i].name,
							NULL,							
							&obj->groupList,
							&obj->admingrpList,
							&obj->ownerObjList,
							&obj->label,
							&obj->description,
							&obj->type,
							&obj->references,
							obj->owner,
							obj->lastChanged,
							&obj->timestamp,								
							&obj->helptext,
							&obj->changeDiary,
							&obj->objPropList,
							&obj->xmlDocVersion,
							&this->arStatus) == AR_RETURN_OK)
						{
							ParseVersionString(obj->xmlDocVersion);

							this->containerList.insert(this->containerList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdCont << ") [OK]" << endl;
							arInsideIdCont++;								
						}
						else
							cerr << GetARStatusError();

						delete obj;
						FreeARStatusList(&this->arStatus, false);
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
			
			alList.Sort();
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

	//Regular forms
	nResult = LoadForms(AR_LIST_SCHEMA_REGULAR, insideId);
	cout << nResult << " Regular Forms loaded" << endl;

	//Dialogs
	nResult = LoadForms(AR_LIST_SCHEMA_DIALOG, insideId);
	cout << nResult << " Dialog Forms loaded" << endl;

	//Join
	nResult = LoadForms(AR_LIST_SCHEMA_JOIN, insideId);
	cout << nResult << " Join Forms loaded" << endl;

	//Vendor
	nResult = LoadForms(AR_LIST_SCHEMA_VENDOR, insideId);
	cout << nResult << " Vendor Forms loaded" << endl;

	//View
	nResult = LoadForms(AR_LIST_SCHEMA_VIEW, insideId);
	cout << nResult << " View Forms loaded" << endl << endl;
}

int CARInside::LoadForms(int nType, int &schemaInsideId)
{
	int nCnt =0;

	try
	{
		ARNameList nameList;
		ARNameList aliasList;

		if(ARGetListSchemaWithAlias(&this->arControl, 0, AR_HIDDEN_INCREMENT | nType, NULL, NULL, NULL, NULL, &nameList, &aliasList, &this->arStatus) == AR_RETURN_OK)
		{
			for(unsigned int i=0; i< nameList.numItems; i++)
			{
				if(!this->InBlacklist(ARREF_SCHEMA, nameList.nameList[i]))
				{
					LOG << "Loading Form: " << nameList.nameList[i] << endl;
					CARSchema *schema = new CARSchema(nameList.nameList[i], schemaInsideId);

					if(aliasList.nameList[i] != NULL)
						schema->alias = aliasList.nameList[i];

					if(ARGetSchema(&this->arControl,
						nameList.nameList[i],
						&schema->schema,
						NULL,
						&schema->groupList,
						&schema->admingrpList,
						&schema->getListFields,
						&schema->sortList,
						&schema->indexList,
						&schema->archiveInfo,
						&schema->auditInfo,
						schema->defaultVui,
						&schema->helptext,
						&schema->timestamp,											
						schema->owner,
						schema->lastChanged,
						&schema->changeDiary,
						&schema->objPropList,
						&this->arStatus) == AR_RETURN_OK)
					{
						//Load form views
						ARInternalIdList idList;

						schema->vuiList.clear();
						if(ARGetListVUI(&this->arControl, nameList.nameList[i], 0, &idList, &this->arStatus) == AR_RETURN_OK)
						{
							for(unsigned int nViewCnt=0; nViewCnt < idList.numItems; nViewCnt++)
							{
								CARVui *vui = new CARVui(idList.internalIdList[nViewCnt]);
								if(ARGetVUI(&this->arControl, 
									nameList.nameList[i],
									idList.internalIdList[nViewCnt],
									vui->vuiName,
									vui->locale,
									&vui->vuiType,
									&vui->objPropList,
									&vui->helptext,
									&vui->timestamp,											
									vui->owner,
									vui->lastChanged,
									&vui->changeDiary,
									&this->arStatus) == AR_RETURN_OK)
								{
									LOG << "Loading Form: " << nameList.nameList[i] << " Vui: " << vui->vuiName << endl;
									vui->schemaInsideId = schemaInsideId;
									vui->name = vui->vuiName;
									schema->vuiList.insert(schema->vuiList.end(), *vui);
								}
								else
									LOG << " [ERROR]" << endl;

								delete vui;
								FreeARStatusList(&this->arStatus, false);
							}
						}
						FreeARInternalIdList(&idList, false);

						//Load fields
						schema->fieldList.clear();

						ARBooleanList existList;
						ARInternalIdList fieldId2;
						ARNameList fieldName;
						ARFieldMappingList fieldMap;
						ARUnsignedIntList dataType;
						ARUnsignedIntList option;
						ARUnsignedIntList createMode;
						ARUnsignedIntList fieldOption;
						ARValueList defaultVal;
						ARPermissionListList permissions;
						ARFieldLimitList limit;
						ARDisplayInstanceListList dInstanceList;
						ARTextStringList helpText;
						ARTimestampList timestamp;
						ARAccessNameList owner;
						ARAccessNameList lastChanged;
						ARTextStringList changeDiary;

						if(ARGetMultipleFields (&this->arControl,
							nameList.nameList[i],
							NULL,
							&existList, 
							&fieldId2,
							&fieldName, 
							&fieldMap, 
							&dataType,
							&option, 
							&createMode, 
							&fieldOption, 
							&defaultVal,
							&permissions, 
							&limit,
							&dInstanceList, 
							&helpText,
							&timestamp, 
							&owner,
							&lastChanged, 
							&changeDiary, 
							&this->arStatus)==AR_RETURN_OK)
						{
							for(unsigned int nFieldCnt=0; nFieldCnt< fieldId2.numItems; nFieldCnt++)
							{
								CARField *field = new CARField(fieldId2.internalIdList[nFieldCnt]);

								field->schemaInsideId = schemaInsideId;
								field->fieldId = fieldId2.internalIdList[nFieldCnt];
								field->name = fieldName.nameList[nFieldCnt];
								field->fieldMap = fieldMap.mappingList[nFieldCnt];
								field->dataType = dataType.intList[nFieldCnt];
								field->option = option.intList[nFieldCnt];
								field->createMode = createMode.intList[nFieldCnt];
								field->defaultVal = defaultVal.valueList[nFieldCnt];
								field->permissions = permissions.permissionList[nFieldCnt];
								field->limit = limit.fieldLimitList[nFieldCnt];
								field->dInstanceList = dInstanceList.dInstanceList[nFieldCnt];
								field->helptext = helpText.stringList[nFieldCnt];
								field->timestamp = timestamp.timestampList[nFieldCnt];
								strncpy(field->owner ,owner.nameList[nFieldCnt], AR_MAX_NAME_SIZE);
								strncpy(field->lastChanged, lastChanged.nameList[nFieldCnt], AR_MAX_NAME_SIZE);
								field->changeDiary = changeDiary.stringList[nFieldCnt];

								schema->fieldList.insert(schema->fieldList.end(), *field);

								//Check if this is a global field and add it to the globalfieldlist
								if(field->fieldId >= 1000000 &&  field->fieldId <= 1999999)
								{
									CARGlobalField *globalField = new CARGlobalField(schemaInsideId, field->GetInsideId(), field->fieldId);
									this->globalFieldList.insert(this->globalFieldList.end(), *globalField);
									delete globalField;
								}
								delete field;
							}

							this->Sort(schema->fieldList);
						}


						//Add form to list
						this->schemaList.push_back(*schema);

						LOG << nameList.nameList[i] << " (InsideID: " << schemaInsideId << ") [OK]" << endl;
						schemaInsideId++;

						nCnt++;
					}			
					else
						LOG << " [ERROR]" << endl;

					delete schema;
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARNameList(&aliasList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(schemaList);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Schema: " << e.what() << endl;
		GetARStatusError();
	}

	return nCnt;
}


int CARInside::LoadContainer(void)
{
	int insideId=0;

	try
	{
		ARContainerInfoList conList;

		ARReferenceTypeList		refTypes;
		refTypes.refType = (int *) malloc(sizeof(unsigned int) * 1);
		refTypes.numItems = 1;
		refTypes.refType[0] = ARREF_ALL;

		if(ARGetListContainer(&this->arControl, 0, ARCON_ALL, AR_HIDDEN_INCREMENT, NULL, NULL, &conList, &this->arStatus) == AR_RETURN_OK)
		{
			this->containerList.clear();	
			for(unsigned int i=0; i< conList.numItems; i++)
			{
				if(!this->InBlacklist(ARREF_CONTAINER, conList.conInfoList[i].name))
				{
					LOG << "Loading " << CAREnum::ContainerType(conList.conInfoList[i].type) << ": " << conList.conInfoList[i].name; 
					CARContainer *obj = new CARContainer(conList.conInfoList[i].name, insideId);

					if( ARGetContainer(&this->arControl,
						conList.conInfoList[i].name,
						&refTypes,
						&obj->groupList,
						&obj->admingrpList,
						&obj->ownerObjList,
						&obj->label,
						&obj->description,
						&obj->type,
						&obj->references,
						&obj->helptext,
						obj->owner,
						&obj->timestamp,						
						obj->lastChanged,
						&obj->changeDiary,
						&obj->objPropList,
						&this->arStatus) == AR_RETURN_OK)
					{
						this->containerList.insert(this->containerList.end(), *obj);

						LOG << " (InsideID: " << insideId << ") [OK]" << endl;						
						insideId++;

						FreeARStatusList(&this->arStatus, false);
					}		
					else
						cerr << GetARStatusError();

					delete obj;
				}
			}
		}

		delete (refTypes.refType);
		FreeARContainerInfoList(&conList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(containerList);		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Container: " << e.what() << endl;
		GetARStatusError();
	}

	return insideId;
}

int CARInside::LoadCharMenus(void)
{
	int insideId=0;

	try
	{
		ARNameList nameList;

		if(ARGetListCharMenu(&this->arControl, 0, NULL, NULL, NULL, &nameList, &this->arStatus) == AR_RETURN_OK)
		{
			this->menuList.clear();	
			for(unsigned int i=0; i< nameList.numItems; i++)
			{
				if(!this->InBlacklist(ARREF_CHAR_MENU, nameList.nameList[i]))
				{
					LOG << "Loading Menu: " << nameList.nameList[i]; 
					CARCharMenu *obj = new CARCharMenu(nameList.nameList[i], insideId);

					if( ARGetCharMenu(&this->arControl, 
						nameList.nameList[i], 
						&obj->refreshCode,
						&obj->menuDefn,
						&obj->helptext,
						&obj->timestamp,
						obj->owner,
						obj->lastChanged,
						&obj->changeDiary,
						&obj->objPropList,
						&this->arStatus) == AR_RETURN_OK)
					{
						this->menuList.insert(this->menuList.end(), *obj);

						LOG << " (InsideID: " << insideId << ") [OK]" << endl;						
						insideId++;

						FreeARStatusList(&this->arStatus, false);
					}
					else
						cerr << GetARStatusError();

					delete obj;
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(menuList);		
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Menus: " << e.what() << endl;
		GetARStatusError();
	}

	return insideId;
}

int CARInside::LoadEscalations(void)
{
	int insideId=0;

	try
	{
		ARNameList nameList;

		if(ARGetListEscalation(&this->arControl, NULL, 0, NULL, &nameList, &this->arStatus) == AR_RETURN_OK)
		{
			this->escalList.clear();	
			for(unsigned int i=0; i< nameList.numItems; i++)
			{
				if(!this->InBlacklist(ARREF_ESCALATION, nameList.nameList[i]))
				{
					LOG << "Loading Escalation: " << nameList.nameList[i]; 
					CAREscalation *obj = new CAREscalation(nameList.nameList[i], insideId);

					if( ARGetEscalation(&this->arControl, 
						nameList.nameList[i], 
						&obj->escalationTm,
						&obj->schemaList,
						&obj->enable,
						&obj->query,
						&obj->actionList,
						&obj->elseList,
						&obj->helptext,
						&obj->timestamp,
						obj->owner,
						obj->lastChanged,
						&obj->changeDiary,
						&obj->objPropList,
						&this->arStatus) == AR_RETURN_OK)
					{
						this->escalList.insert(this->escalList.end(), *obj);

						LOG << " (InsideID: " << insideId << ") [OK]" << endl;						
						insideId++;

						FreeARStatusList(&this->arStatus, false);
					}	
					else
						cerr << GetARStatusError();

					delete obj;
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(escalList);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Escalations: " << e.what() << endl;
		GetARStatusError();
	}

	return insideId;
}

int CARInside::LoadFilters(void)
{
	int insideId=0;

	try
	{
		ARNameList nameList;

		if(ARGetListFilter(&this->arControl, NULL, 0, NULL, &nameList, &this->arStatus) == AR_RETURN_OK)
		{
			this->filterList.clear();	
			for(unsigned int i=0; i< nameList.numItems; i++)
			{
				if(!this->InBlacklist(ARREF_FILTER, nameList.nameList[i]))
				{
					LOG << "Loading Filter: " << nameList.nameList[i]; 
					CARFilter *obj = new CARFilter(nameList.nameList[i], insideId);

					if( ARGetFilter(&this->arControl, 
						nameList.nameList[i], 
						&obj->order,
						&obj->schemaList,
						&obj->opSet,
						&obj->enable,
						&obj->query,
						&obj->actionList,
						&obj->elseList,
						&obj->helptext,
						&obj->timestamp,
						obj->owner,
						obj->lastChanged,
						&obj->changeDiary,
						&obj->objPropList,
#if AR_CURRENT_API_VERSION > 12 // Version 7.1 and higher
						&obj->errorOptions,
						obj->errorFilterName,
#endif
						&this->arStatus) == AR_RETURN_OK)
					{
						this->filterList.insert(this->filterList.end(), *obj);

						LOG << " (InsideID: " << insideId << ") [OK]" << endl;						
						insideId++;

						FreeARStatusList(&this->arStatus, false);
					}	
					else
						cerr << GetARStatusError();

					delete obj;
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(filterList);
	}
	catch(exception& e)
	{
		cout << "EXCEPTION loading Filters: " << e.what() << endl;
		GetARStatusError();
	}

	return insideId;
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
		GetARStatusError();
	}
	return alList.GetCount();
}

void CARInside::Sort(list<CARSchema> &listResult)
{
	listResult.sort(SortByName);
}

void CARInside::Sort(list<CARFilter> &listResult)
{
	listResult.sort(SortByName);
}

void CARInside::Sort(list<CAREscalation> &listResult)
{
	listResult.sort(SortByName);
}

void CARInside::Sort(list<CARActiveLink> &listResult)
{
	listResult.sort(SortByName);
}

void CARInside::Sort(list<CARContainer> &listResult)
{
	listResult.sort(SortByName);
}

void CARInside::Sort(list<CARCharMenu> &listResult)
{
	listResult.sort(SortByName);
}

void CARInside::Sort(list<CARField> &listResult)
{
	listResult.sort(SortByName);
}

bool CARInside::SortByName(const CARServerObject& t1, const CARServerObject& t2 )
{
	string tmpStr1 = CUtil::String2Comp(t1.GetName());
	string tmpStr2 = CUtil::String2Comp(t2.GetName());
	return ( tmpStr1.compare(tmpStr2) < 0 );
}

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
	CDocMain *docMain = new CDocMain(*this);

	//Server information
	docMain->ServerInfoList();

	//ContainerList
	docMain->ContainerList(ARCON_GUIDE, "index", "ContainerList (ActiveLinkGuide)", "*");
	docMain->ContainerList(ARCON_APP, "index", "ContainerList (Application)", "*");
	docMain->ContainerList(ARCON_PACK, "index", "ContainerList (PackingList)", "*");
	docMain->ContainerList(ARCON_FILTER_GUIDE, "index", "ContainerList (FilterGuide)", "*");
	docMain->ContainerList(ARCON_WEBSERVICE, "index", "ContainerList (Webservice)", "*");	
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ContainerList(ARCON_GUIDE, ObjListFilename(std::string(1, strValue.at(i))), "GuideList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_APP, ObjListFilename(std::string(1, strValue.at(i))), "ApplicationList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_PACK, ObjListFilename(std::string(1, strValue.at(i))), "PackinglistList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_FILTER_GUIDE, ObjListFilename(std::string(1, strValue.at(i))), "FilterGuideList", std::string(1, strValue.at(i)));
		docMain->ContainerList(ARCON_WEBSERVICE, ObjListFilename(std::string(1, strValue.at(i))), "WebserviceList", std::string(1, strValue.at(i)));	
	}

	//Application Details
	int nTmpCnt = 1;
	//Create documentation here to fill objects applicationName reference information	
	list<CARContainer>::iterator listIter;	
	cout << "Starting Container Documentation" << endl;
	for ( listIter = this->containerList.begin(); listIter != this->containerList.end(); listIter++ )
	{
		CARContainer *cont = &(*listIter);

		switch(cont->type)
		{
		case ARCON_APP:
			{
				LOG << "Application [" << nTmpCnt << "-" << containerList.size() << "] '" << cont->name << "': ";
				CDocApplicationDetails appDetails(*this, *cont);
				appDetails.Documentation();
			}
			break;
		default:
			{
				LOG << "Container [" << nTmpCnt << "-" << containerList.size() << "] '" << cont->name << "' [OK]" << endl;
			}
			break;
		}

		nTmpCnt++;
	}

	nTmpCnt = 1;
	for ( listIter = this->containerList.begin(); listIter != this->containerList.end(); listIter++ )
	{
		CARContainer *cont = &(*listIter);
		switch(cont->type)
		{
		case ARCON_WEBSERVICE:
			{
				LOG << "Webservice [" << nTmpCnt << "-" << containerList.size() << "] '" << cont->name << "': ";
				CDocWebserviceDetails wsDetails(*this, *cont);
				wsDetails.Documentation();
			}
			break;
		case ARCON_GUIDE:
			{
				LOG << "ActiveLinkGuide [" << nTmpCnt << "-" << containerList.size() << "] '" << cont->name << "': ";
				CDocAlGuideDetails guideDetails(*this, *cont);
				guideDetails.Documentation();
			}
			break;
		case ARCON_FILTER_GUIDE:
			{
				LOG << "FilterGuide [" << nTmpCnt << "-" << containerList.size() << "] '" << cont->name << "': ";
				CDocFilterGuideDetails fltGuideDetails(*this, *cont);
				fltGuideDetails.Documentation();
			}
			break;
		case ARCON_PACK:
			{
				LOG << "PackingList [" << nTmpCnt << "-" << containerList.size() << "] '" << cont->name << "': ";
				CDocPacklistDetails packDetails(*this, *cont);
				packDetails.Documentation();
			}
			break;
		case ARCON_APP:
			{
				LOG << "Application [" << nTmpCnt << "-" << containerList.size() << "] '" << cont->name << "' [OK]" << endl;
			}
			break;
		}

		nTmpCnt++;
	}

	//ActiveLink List
	docMain->ActiveLinkList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->ActiveLinkList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
	}	

	docMain->ActiveLinkActionList("index_action");

	//ActiveLink Details
	unsigned int tmpCount = alList.GetCount();
	cout << "Starting ActiveLink Documentation" << endl;
	for (unsigned int actlinkIndex = 0; actlinkIndex < tmpCount; ++actlinkIndex)
	{	
		LOG << "ActiveLink [" << actlinkIndex << "-" << nTmpCnt << "] '" << alList.ActiveLinkGetName(actlinkIndex) << "': ";
		CDocAlDetails alDetails(actlinkIndex, 2);
		alDetails.Documentation();
	}



	//Filter List
	docMain->FilterList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->FilterList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
	}	
	docMain->FilterActionList("index_action");

	//Filter Details
	nTmpCnt=1;
	list<CARFilter>::iterator filterIter;
	cout << "Starting Filter Documentation" << endl;
	for ( filterIter = this->filterList.begin(); filterIter != this->filterList.end(); filterIter++)
	{
		CARFilter *filter = &(*filterIter);

		LOG << "Filter [" << nTmpCnt << "-" << filterList.size() << "] '" << filter->name << "': ";
		CDocFilterDetails *filterDetails = new CDocFilterDetails(*this, *filter, "filter/"+filter->FileID(), 2);
		filterDetails->Documentation();
		delete filterDetails;
		nTmpCnt++;
	}



	//Escalation List
	docMain->EscalationList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->EscalationList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
	}
	docMain->EscalationActionList("index_action");

	//Escalation Details
	nTmpCnt=1;
	list<CAREscalation>::iterator escalIter;
	cout << "Starting Escalation Documentation" << endl;
	for ( escalIter = this->escalList.begin(); escalIter != this->escalList.end(); escalIter++)
	{
		CAREscalation *escal = &(*escalIter);

		LOG << "Escalation [" << nTmpCnt << "-" << escalList.size() << "] '" << escal->name << "': ";
		CDocEscalationDetails *escalDetails = new CDocEscalationDetails(*this, *escal, "escalation/"+escal->FileID(), 2);
		escalDetails->Documentation();
		delete escalDetails;		
		nTmpCnt++;
	}



	//CharMenus
	docMain->CharMenuList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->CharMenuList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
	}	

	// Char Menu Details
	nTmpCnt = 1;
	list<CARCharMenu>::iterator menuIter;	
	cout << "Starting Menu Documentation" << endl;
	for ( menuIter = this->menuList.begin(); menuIter != this->menuList.end(); menuIter++ )
	{
		CARCharMenu *menu = &(*menuIter);	

		LOG << "Menu [" << nTmpCnt << "-" << menuList.size() << "] '" << menu->name << "': ";
		string path = "menu/"+menu->FileID();
		CDocCharMenuDetails *menuDetails = new CDocCharMenuDetails(*this, *menu, path, 2);
		menuDetails->Documentation();
		delete menuDetails;
		nTmpCnt++;
	}



	//Schema List
	docMain->SchemaList(AR_SCHEMA_NONE, "index", "Formlist (All)", "*");
	docMain->SchemaList(AR_SCHEMA_REGULAR, "index_regular", "Formlist (Regular)", "*");
	docMain->SchemaList(AR_SCHEMA_JOIN, "index_join", "Formlist (Join)", "*");
	docMain->SchemaList(AR_SCHEMA_VIEW, "index_view", "Formlist (View)", "*");
	docMain->SchemaList(AR_SCHEMA_DIALOG, "index_dialog", "Formlist (Dialog)", "*");
	docMain->SchemaList(AR_SCHEMA_VENDOR, "index_vendor", "Formlist (Vendor)", "*");	
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->SchemaList(AR_SCHEMA_NONE, ObjListFilename(std::string(1, strValue.at(i))), "Formlist", std::string(1, strValue.at(i)));
	}

	//Schema and field Details
	nTmpCnt=1;
	list<CARSchema>::iterator schemaIter;	
	cout << "Starting Schema Documentation" << endl;
	for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
	{
		int rootLevel = 2;

		CARSchema *schema = &(*schemaIter);		
		string path="schema/"+schema->FileID();

		LOG << "Schema [" << nTmpCnt << "-" << schemaList.size() << "] '" << schema->name << "': ";
		CDocSchemaDetails *schemaDetails = new CDocSchemaDetails(*this, *schema, path, rootLevel);
		schemaDetails->Documentation();
		delete schemaDetails;


		//VuiDetails
		LOG << "VuiDetails Schema '" << schema->name << "'" << endl;
		list<CARVui>::iterator vuiIter;			
		for( vuiIter = schema->vuiList.begin(); vuiIter != schema->vuiList.end(); vuiIter++)
		{
			CARVui *vui = &(*vuiIter);

			LOG << "SchemaView '" << vui->name << "': ";
			CDocVuiDetails *vuiDetails = new CDocVuiDetails(*this, *schema, *vui, path, rootLevel);
			vuiDetails->Documentation();
			delete vuiDetails;
		}

		//FieldDetails
		LOG << "FieldDetails Schema '" << schema->name << "'" << endl;		
		list<CARField>::iterator fieldIter;
		for ( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++ )
		{	
			CARField *field = &(*fieldIter);			
			CDocFieldDetails *fieldDetails = new CDocFieldDetails(*this, *schema, *field, path, rootLevel);
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

		CDocImageDetails imgDetails(imgIndex, 2);
		imgDetails.Documentation();
	}
#endif

	//GlobalFieldList
	docMain->GlobalFieldList();	

	//MessageList
	docMain->MessageList();	

	//Analyzer
	CDocAnalyzer *analyzer = new CDocAnalyzer(*this, "other");
	analyzer->Documentation();
	delete analyzer;

	//Validation
	CDocValidator *validator = new CDocValidator(*this, "other");
	validator->Main();
	delete validator;

	//Group List
	docMain->GroupList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->GroupList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
	}	


	//Group Details
	nTmpCnt=1;
	list<CARGroup>::iterator groupIter;
	cout << "Starting Group Documentation" << endl;
	for ( groupIter = this->groupList.begin(); groupIter != this->groupList.end(); groupIter++ )
	{
		CARGroup *grp = &(*groupIter);

		LOG << "Group [" << nTmpCnt << "-" << groupList.size() << "] '" << grp->name << "': ";
		CDocGroupDetails *grpDetails = new CDocGroupDetails(*this, *grp);
		grpDetails->Documentation();
		delete grpDetails;
		nTmpCnt++;
	}


	//Role List
	docMain->RoleList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->RoleList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
	}	

	//Role Details
	nTmpCnt=1;
	list<CARRole>::iterator roleIter;
	cout << "Starting Role Documentation" << endl;
	for ( roleIter = this->roleList.begin(); roleIter != this->roleList.end(); roleIter++ )
	{
		CARRole *role = &(*roleIter);

		LOG << "Role [" << nTmpCnt << "-" << roleList.size() << "] '" << role->name << "': ";
		CDocRoleDetails *roleDetails = new CDocRoleDetails(*this, *role);
		roleDetails->Documentation();
		delete roleDetails;
		nTmpCnt++;
	}


	//Userlists
	docMain->UserList("index", "*");
	for (unsigned int i = 0; i < strValue.size(); ++i)
	{		
		docMain->UserList(ObjListFilename(std::string(1, strValue.at(i))), std::string(1, strValue.at(i)));
	}	

	//User Details	list<CARUser>::iterator userIter;
	nTmpCnt=1;
	list<CARUser>::iterator userIter;
	cout << "Starting User Documentation" << endl;
	for ( userIter = this->userList.begin(); userIter != this->userList.end(); userIter++ )
	{
		CARUser *user = &(*userIter);

		LOG << "User [" << nTmpCnt << "-" << userList.size() << "] '" << user->loginName << "': ";
		CDocUserDetails *userDetails = new CDocUserDetails(*this, *user);
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
	list<CARSchema>::iterator schemaIter;		

	for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
	{			
		CARSchema *schema = &(*schemaIter);

		if(schema->GetInsideId() == schemaInsideId)
		{
			list<CARField>::iterator fieldIter;					
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);
				if(field->GetInsideId() == fieldInsideId)
				{					
					if(field->dataType == AR_DATA_TYPE_ENUM)
					{
						switch(field->limit.u.enumLimits.listStyle)
						{
						case AR_ENUM_STYLE_REGULAR:
							return field->limit.u.enumLimits.u.regularList.nameList[enumPosition];
							break;
						case AR_ENUM_STYLE_CUSTOM:
							for (int i=0; i< (int)field->limit.u.enumLimits.u.customList.numItems; i++) { 
								if (field->limit.u.enumLimits.u.customList.enumItemList[i].itemNumber == enumPosition) 
									return field->limit.u.enumLimits.u.customList.enumItemList[i].itemName; 
							} 
							return "";
							break;
						case AR_ENUM_STYLE_QUERY:
							return "QUERY";
							break;
						}						
					}
				}
			}
		}
	}
	return EmptyValue;
}

string CARInside::LinkToField(string schemaName, int fieldInsideId, int fromRootLevel)
{	
	return LinkToField(SchemaGetInsideId(schemaName), fieldInsideId, fromRootLevel);
}

string CARInside::LinkToField(int schemaInsideId, int fieldInsideId, int fromRootLevel)
{	
	stringstream tmp;
	tmp.str("");

	list<CARSchema>::iterator schemaIter;		
	for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
	{			
		CARSchema *schema = &(*schemaIter);
		if(schema->GetInsideId() == schemaInsideId)
		{
			list<CARField>::iterator fieldIter;
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);
				if(field->GetInsideId() == fieldInsideId)
				{
					return field->GetURL(fromRootLevel);
				}
			}
		}
	}

	//Field has not been found
	if(fieldInsideId > 0) //OpenWindow uses 0 what is no valid field
	{
		CFieldRefItem *refItemNotFound = new CFieldRefItem();
		refItemNotFound->arsStructItemType = AR_STRUCT_ITEM_XML_FIELD;
		refItemNotFound->fromName = "";						
		refItemNotFound->schemaInsideId = schemaInsideId;
		refItemNotFound->fieldInsideId = fieldInsideId;
		refItemNotFound->description = "Field does not exist";	
		this->listFieldNotFound.push_back(*refItemNotFound);
		delete refItemNotFound;
	}

	tmp << "<span class=\"fieldNotFound\">" << fieldInsideId << "</span>";

	return tmp.str();
}

string CARInside::LinkToMenuField(int schemaInsideId, int fieldInsideId, int fromRootLevel)
{	
	stringstream tmp;
	tmp.str("");

	list<CARSchema>::iterator schemaIter;			
	for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
	{			
		CARSchema *schema = &(*schemaIter);
		if(schema->GetInsideId() == schemaInsideId)
		{
			list<CARField>::iterator fieldIter;					
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);
				if(field->GetInsideId() == fieldInsideId)
				{
					tmp << CWebUtil::RootPath(fromRootLevel) << "schema/" << schemaInsideId << "/" << CWebUtil::DocName(field->FileID());

					return CWebUtil::Link(field->name, tmp.str(), "", fromRootLevel);
				}
			}
		}
	}

	tmp << fieldInsideId;

	return tmp.str();
}

string CARInside::LinkToSchemaTypeList(int schemaType, int rootLevel)
{
	string strFileUrl = "";
	switch (schemaType)
	{		
	case AR_SCHEMA_REGULAR: strFileUrl = CWebUtil::DocName("index_regular"); break;
	case AR_SCHEMA_JOIN: strFileUrl = CWebUtil::DocName("index_join"); break;
	case AR_SCHEMA_VIEW: strFileUrl = CWebUtil::DocName("index_view"); break;
	case AR_SCHEMA_DIALOG: strFileUrl = CWebUtil::DocName("index_dialog"); break;
	case AR_SCHEMA_VENDOR: strFileUrl = CWebUtil::DocName("index_vendor"); break;
	default: strFileUrl = CWebUtil::DocName("index");
	}

	return CWebUtil::Link(CAREnum::SchemaType(schemaType), "../"+strFileUrl, "", rootLevel);
}

string CARInside::LinkToSchemaIndex(string indexName, int schemaInsideId, int fromRootLevel)
{
	stringstream tmp;
	tmp << CWebUtil::RootPath(fromRootLevel) << "schema/" << schemaInsideId << "/" << CWebUtil::DocName("form_index_list");	
	return CWebUtil::Link(indexName, tmp.str(), "", fromRootLevel, true);
}

string CARInside::LinkToSchema(int insideId, int fromRootLevel)
{
	list<CARSchema>::iterator schemaIter;
	for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
	{			
		CARSchema *schema = &(*schemaIter);
		if(insideId == schema->GetInsideId())
		{
			return schema->GetURL(fromRootLevel);
		}
	}
	return EmptyValue;
}

string CARInside::LinkToSchema(string schemaName, int fromRootLevel)
{
	list<CARSchema>::iterator schemaIter;
	for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
	{			
		CARSchema *schema = &(*schemaIter);
		if(strcmp(schemaName.c_str(), schema->name.c_str())==0)
		{
			return schema->GetURL(fromRootLevel);
		}
	}
	return schemaName;
}

int CARInside::SchemaGetInsideId(string searchObjName)
{
	list<CARSchema>::iterator iter;			
	list<CARSchema>::iterator endIt = schemaList.end();
	for ( iter = schemaList.begin(); iter != endIt; ++iter )
	{			
		CARSchema *obj = &(*iter);
		if(obj->name.compare(searchObjName)==0)
		{
			return obj->GetInsideId();
		}
	}
	return -1;
}

int CARInside::AlGetInsideId(string searchObjName)
{
	return alList.Find(searchObjName.c_str());
}

int CARInside::FilterGetInsideId(string searchObjName)
{
	list<CARFilter>::iterator iter;			
	for ( iter = filterList.begin(); iter != filterList.end(); iter++ )
	{			
		CARFilter *obj = &(*iter);
		if(strcmp(obj->name.c_str(), searchObjName.c_str())==0)
		{
			return obj->GetInsideId();
		}
	}
	return -1;
}

int CARInside::ContainerGetInsideId(string searchObjName)
{
	list<CARContainer>::iterator iter;			
	for ( iter = containerList.begin(); iter != containerList.end(); iter++ )
	{			
		CARContainer *obj = &(*iter);
		if(strcmp(obj->name.c_str(), searchObjName.c_str())==0)
		{
			return obj->GetInsideId();
		}
	}
	return -1;
}

int CARInside::MenuGetInsideId(string searchObjName)
{
	list<CARCharMenu>::iterator iter;			
	for ( iter = menuList.begin(); iter != menuList.end(); iter++ )
	{			
		CARCharMenu *obj = &(*iter);
		if(strcmp(obj->name.c_str(), searchObjName.c_str())==0)
		{
			return obj->GetInsideId();
		}
	}
	return -1;
}

int CARInside::EscalationGetInsideId(string searchObjName)
{
	list<CAREscalation>::iterator iter;
	for ( iter = escalList.begin(); iter != escalList.end(); iter++ )
	{			
		CAREscalation *obj = &(*iter);
		if(strcmp(obj->name.c_str(), searchObjName.c_str())==0)
		{
			return obj->GetInsideId();
		}
	}
	return -1;
}

string CARInside::LinkToUser(string loginName, int rootLevel)
{	
	list<CARUser>::iterator userIter;			
	for ( userIter = userList.begin(); userIter != userList.end(); userIter++ )
	{	
		CARUser *user = &(*userIter);
		if(strcmp(loginName.c_str(), user->name.c_str())==0)
		{
			return CWebUtil::Link(loginName, CWebUtil::RootPath(rootLevel)+"user/"+CWebUtil::DocName(user->FileID()), "", rootLevel);
		}
	}
	return loginName;
}

bool CARInside::ValidateGroup(string appRefName, int permissionId)
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
				return CWebUtil::Link(group->groupName, CWebUtil::RootPath(rootLevel)+"group/"+CWebUtil::DocName(group->FileID()), "group.gif", rootLevel);
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
					return CWebUtil::Link(role->roleName, CWebUtil::RootPath(rootLevel)+"role/"+CWebUtil::DocName(role->FileID()), "doc.gif", rootLevel);
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
	list<CARFilter>::iterator filterIter;
	list<CARFilter>::iterator endIt = this->filterList.end();
	for ( filterIter = this->filterList.begin(); filterIter != endIt; ++filterIter)
	{	
		CARFilter *filter = &(*filterIter);
		if(filter->GetInsideId() == filterInsideId)
			return LinkToFilterRef(filter, rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToFilterRef(string fltName, int rootLevel)
{	
	list<CARFilter>::iterator filterIter;
	list<CARFilter>::iterator endIt = this->filterList.end();
	for (filterIter = this->filterList.begin(); filterIter != endIt; ++filterIter)
	{	
		CARFilter *filter = &(*filterIter);
		if(filter->name.compare(fltName)==0)
			return LinkToFilterRef(filter, rootLevel);
	}
	return EmptyValue;
}

string CARInside::LinkToFilterRef(CARFilter* filter, int rootLevel)
{
	stringstream strmTmp;
	strmTmp.str("");

	if (filter != NULL)
		strmTmp << filter->GetURL(rootLevel) << " (" << filter->order << ")";

	return strmTmp.str();
}

string CARInside::LinkToFilter(string filterName, int fromRootLevel)
{
	list<CARFilter>::iterator filterIter;
	list<CARFilter>::iterator endIt = this->filterList.end();
	for ( filterIter = this->filterList.begin(); filterIter != endIt; ++filterIter )
	{	
		CARFilter *filter = &(*filterIter);
		if(strcmp(filterName.c_str(), filter->name.c_str())==0)
		{
			return filter->GetURL(fromRootLevel);
		}
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

	list<CARCharMenu>::iterator menuIter;
	list<CARCharMenu>::iterator endIt = this->menuList.end();
	for ( menuIter = this->menuList.begin(); menuIter != endIt; ++menuIter )
	{	
		CARCharMenu *menu = &(*menuIter);
		if(menuName.compare(menu->name) == 0)
		{
			if (bFound) { *bFound = true; }
			return menu->GetURL(fromRootLevel);
		}
	}

	//Menu has not been found
	if (bFound) { *bFound = false; }

	//if the menu is missing, we just return the name of it. maybe link to the
	//"missing menus" page (must be implemented first) of CDocValidator later.
	return "<span class=\"fieldNotFound\">" + menuName + "</span>";
}

string CARInside::LinkToEscalation(string escalationName, int fromRootLevel)
{
	list<CAREscalation>::iterator escalIter;
	list<CAREscalation>::iterator endIt = this->escalList.end();
	for ( escalIter = this->escalList.begin(); escalIter != endIt; ++escalIter )
	{	
		CAREscalation *escal = &(*escalIter);
		if(escalationName.compare(escal->name) == 0)
		{
			return escal->GetURL(fromRootLevel);			
		}
	}
	return escalationName;
}

string CARInside::LinkToContainer(string containerName, int fromRootLevel)
{
	list<CARContainer>::iterator contIter;
	list<CARContainer>::iterator endIt = this->containerList.end();
	for ( contIter = this->containerList.begin(); contIter != endIt; ++contIter )
	{	
		CARContainer *container = &(*contIter);
		if(containerName.compare(container->name) == 0)
		{			
			return container->GetURL(fromRootLevel);
		}
	}
	return containerName;
}

string CARInside::LinkToServerInfo(string srvName, int rootLevel)
{		
	string result;

	if(strcmp(srvName.c_str(), "")==0)
	{
		return CWebUtil::Link(appConfig.serverName, CWebUtil::RootPath(rootLevel)+"other/"+CWebUtil::DocName("server"), "", rootLevel);
	}
	else if(strcmp(srvName.c_str(), AR_CURRENT_SERVER_TAG)==0 || strcmp(srvName.c_str(), AR_CURRENT_SCREEN_TAG)==0)
	{
		return CWebUtil::Link(appConfig.serverName, CWebUtil::RootPath(rootLevel)+"other/"+CWebUtil::DocName("server"), "", rootLevel);
	}
	else
	{
		return CWebUtil::Link(srvName, CWebUtil::RootPath(rootLevel)+"other/"+CWebUtil::DocName("server"), "", rootLevel);
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
		return CAREnum::ObjectEnable(static_cast<CARFilter*>(obj)->enable);
	case AR_STRUCT_ITEM_XML_ESCALATION:
		return CAREnum::ObjectEnable(static_cast<CAREscalation*>(obj)->enable);
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
			list<CARFilter>::iterator filterIter;			
			for ( filterIter = this->filterList.begin(); filterIter != this->filterList.end(); ++filterIter)
			{	
				CARFilter *filter = &(*filterIter);
				if(objName.compare(filter->name) == 0)
				{
					return CAREnum::ObjectEnable(filter->enable);
				}
			}
		}
		break;		
	case AR_STRUCT_ITEM_XML_ESCALATION:
		{
			list<CAREscalation>::iterator escalIter;			
			for ( escalIter = this->escalList.begin(); escalIter != this->escalList.end(); ++escalIter)
			{	
				CAREscalation *escal = &(*escalIter);
				if(objName.compare(escal->name)==0)
				{
					return CAREnum::ObjectEnable(escal->enable);
				}
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

		list<CARSchema>::iterator schemaIter = schemaList.begin();
		list<CARSchema>::iterator endIt = schemaList.end();
		for ( ; schemaIter != endIt; ++schemaIter )
		{			
			CARSchema *schema = &(*schemaIter);

			list<CARField>::iterator fieldIter = schema->fieldList.begin();
			list<CARField>::iterator endIt = schema->fieldList.end();
			for( ; fieldIter != endIt; ++fieldIter)
			{
				CARField *field = &(*fieldIter);
				CustomFieldReferences(*schema, *field);
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
void CARInside::CustomFieldReferences(CARSchema &schema, CARField &obj)
{
	try
	{
		int rootLevel = 2;

		stringstream strm;
		strm.str("");

		switch(obj.limit.dataType)
		{		
		case AR_DATA_TYPE_COLUMN: //Find column references
			{
				ARColumnLimitsStruct fLimit = obj.limit.u.columnLimits;

				//To create a link to the datafield we first must find the target schema of the table
				string tableSchema;                    // the name of table source form
				CARSchema* tableSourceSchema = NULL;   // the obj of table source form (if found)

				CARField* tableField = this->FindField(&schema, fLimit.parent);
				if (tableField != NULL && tableField->dataType == AR_DATA_TYPE_TABLE && tableField->limit.dataType == AR_DATA_TYPE_TABLE)
				{
					tableSchema = tableField->limit.u.tableLimits.schema;

					if (!tableSchema.empty() && tableSchema[0] == '$')
						tableSchema = tableField->limit.u.tableLimits.sampleSchema;

					if (tableSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
						tableSchema = schema.name;

					tableSourceSchema = this->FindSchema(tableSchema);

					if (tableSourceSchema != NULL)
					{
						stringstream tmpDesc;
						tmpDesc << "Column in Table " + LinkToField(schema.GetInsideId(), tableField->fieldId, rootLevel) << " of Form " << LinkToSchema(tableField->schemaInsideId, rootLevel);
						CFieldRefItem refItem(AR_STRUCT_ITEM_XML_FIELD, schema.GetName(), tmpDesc.str(), fLimit.dataField, tableSourceSchema->GetInsideId());
						refItem.fromFieldId = obj.fieldId;
						this->AddReferenceItem(&refItem);
					}
				}
			}
			break;
		case AR_DATA_TYPE_TABLE:
			{
				ARTableLimitsStruct fLimit = obj.limit.u.tableLimits;

				string tableSchema;                    // the name of table source form
				CARSchema* tableSourceSchema = NULL;   // the obj of table source form (if found)

				tableSchema = obj.limit.u.tableLimits.schema;

				if (!tableSchema.empty() && tableSchema[0] == '$')
					tableSchema = obj.limit.u.tableLimits.sampleSchema;

				if (tableSchema.compare(AR_CURRENT_SCHEMA_TAG) == 0)
					tableSchema = schema.name;

				tableSourceSchema = this->FindSchema(tableSchema);

				if (tableSourceSchema != NULL && fLimit.qualifier.operation != AR_COND_OP_NONE)
				{		
					stringstream strmQuery;
					CFieldRefItem refItem(AR_STRUCT_ITEM_XML_FIELD, schema.name, "Table Qualification", 0, 0);
					refItem.fromFieldId = obj.fieldId;

					CARQualification arQual(*this);
					int pFormId = schema.GetInsideId();
					int sFormId = tableSourceSchema->GetInsideId();

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

		CARSchema *schema = FindSchema(schemaInsideId);
		if (schema == NULL)
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
						CARSchema *schema = FindSchema(schemaInsideId);
						CARField *field = FindField(schema,iFieldId);
						CARField *fieldStatus = NULL;

						if (field == NULL)
							break;

						// now link to the field
						strmTmp << field->GetURL(rootLevel);
						refItem->fieldInsideId = field->fieldId;
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
							fieldStatus = FindField(schema,7);	// get status field
							if (fieldStatus != NULL)
							{
								int iEnumId = atoi(enumId);
								strmTmp << "." << GetFieldEnumValue(schemaInsideId, fieldStatus->GetInsideId(), iEnumId) << "." << usrOrTimeStr;
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
		if(inText.size() == 0)
			return "";

		list<CARSchema>::iterator schemaIter;				
		for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
		{			
			CARSchema *schema = &(*schemaIter);
			if(schema->GetInsideId() == schemaInsideId)
			{
				list<CARField>::iterator fieldIter;		
				CARField *field;

				for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
				{
					field = &(*fieldIter);

					stringstream strmTmp;
					strmTmp << "arFieldId=&quot;" << field->fieldId << "&quot;";			

					stringstream fieldLink;
					fieldLink << "arFieldId=&quot;" << field->GetURL(rootLevel) << "&quot;";					

					unsigned int nLengthOrg = (unsigned int)inText.length();

					string fField = strmTmp.str();
					inText = CUtil::StrReplace(fField, fieldLink.str(), inText);

					//if the string is longer because we have added a link (long string) we add a field reference
					if(inText.length() > nLengthOrg) 
					{						
						refItem->fieldInsideId = field->fieldId;
						AddReferenceItem(refItem);
					}
				}
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

		list<CARFilter>::iterator filterIter;
		for ( filterIter = filterList.begin(); filterIter != filterList.end(); filterIter++)
		{
			CARFilter *filter = &(*filterIter);
			if (filter->errorOptions == AR_FILTER_ERRHANDLER_ENABLE)
			{
				ErrorCallMap::iterator item = errorCalls.find(filter->errorFilterName);
				if (item == errorCalls.end())
				{
					ErrCallPair newItem(filter->errorFilterName, list<string>());
					newItem.second.insert(newItem.second.end(), filter->name);
					errorCalls.insert(newItem);
				}
				else
				{
					item->second.insert(item->second.end(), filter->name);
				}
			}
		}

		if (errorCalls.size() > 0)
		{
			for ( filterIter = filterList.begin(); filterIter != filterList.end(); filterIter++)
			{
				CARFilter *filter = &(*filterIter);
				ErrorCallMap::iterator item = errorCalls.find(filter->name);
				if (item != errorCalls.end())
				{
					for ( list<string>::iterator caller = item->second.begin(); caller != item->second.end(); caller++)
					{
						filter->errorCallers.insert(filter->errorCallers.end(), *caller);
					}
				}
			}
		}

		cout << endl;
	}
	catch (...)
	{
		cerr << "EXCEPTION SearchFilterReferences" << endl;
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

CARSchema* CARInside::FindSchema(int schemaInsideId)
{
	list<CARSchema>::iterator schemaIter;		
	for (schemaIter = schemaList.begin(); schemaIter != schemaList.end(); ++schemaIter)
	{			
		CARSchema *schema = &(*schemaIter);
		if(schema->GetInsideId() == schemaInsideId)
		{
			return schema;
		}
	}
	return NULL;
}

CARSchema* CARInside::FindSchema(std::string schemaName)
{
	list<CARSchema>::iterator schemaIter;		
	for (schemaIter = schemaList.begin(); schemaIter != schemaList.end(); ++schemaIter)
	{			
		CARSchema *schema = &(*schemaIter);
		if(schema->name.compare(schemaName)==0)
		{
			return schema;
		}
	}
	return NULL;
}

CARField* CARInside::FindField(CARSchema* schema, int fieldId)
{
	if (schema == NULL) return NULL;

	list<CARField>::iterator fieldIter = schema->fieldList.begin();
	list<CARField>::iterator endIt = schema->fieldList.end();

	for(; fieldIter != endIt; ++fieldIter)
	{
		CARField *field = &(*fieldIter);
		if(field->fieldId == fieldId)
		{
			return field;
		}
	}
	return NULL;
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
	CARSchema *schema = FindSchema(form);
	if (schema != NULL)
		strmTmp << schema->GetURL(rootLevel, false);
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
	strmTmp.str("");

	//add the reference
	try {
		CARSchema *schema = FindSchema(schemaInsideId);
		if (schema != NULL)
		{
			CARField *field = FindField(schema,iFieldId);
			if (field != NULL)
			{
				CARField *fieldStatus = NULL;

				strmTmp << field->GetURL(rootLevel);
				refItem->fieldInsideId = field->fieldId;
				AddReferenceItem(refItem);
			}
			else
				strmTmp << iFieldId;
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

