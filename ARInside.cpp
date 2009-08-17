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

#include "doc\docmain.h"
#include "doc\docuserdetails.h"
#include "doc\docgroupdetails.h"
#include "doc\docschemadetails.h"
#include "doc\docaldetails.h"
#include "doc\docfilterdetails.h"
#include "doc\docescalationdetails.h"
#include "doc\doccharmenudetails.h"
#include "doc\docfielddetails.h"
#include "doc\docvuidetails.h"
#include "doc\docwebservicedetails.h"
#include "doc\docpacklistdetails.h"
#include "doc\docapplicationdetails.h"
#include "doc\docalguidedetails.h"
#include "doc\docfilterguidedetails.h"
#include "doc\doccontainerhelper.h"
#include "doc\docanalyzer.h"
#include "doc\docvalidator.h"
#include "doc\docroledetails.h"
#include "doc\docsummaryinfo.h"

#include "output\table.h"
#include "output\webutil.h"

#ifdef _DEBUG
#include "svnrev.h"
#endif

/////////
// version information block
#define VERSION "3.0"
#if _DEBUG
	#define VERSION_STR VERSION "r" SVN_REV_STR
#else
	#define VERSION_STR VERSION
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
	this->alList.clear();
	this->containerList.clear();
	this->menuList.clear();
	this->userList.clear();
	this->groupList.clear();
	this->roleList.clear();
	this->serverInfoList.clear();
	this->globalFieldList.clear();
	this->blackList.clear();
	this->listFieldRefItem.clear();
	this->listMenuRefItem.clear();
	this->listFieldNotFound.clear();

	this->nDurationLoad = 0;
	this->nDurationDocumentation = 0;

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

	if(this->appConfig.bUseUtf8)
		strcpy(arControl.localeInfo.charSet, "UTF-8");

	strncpy(arControl.user, user.c_str(), AR_MAX_NAME_SIZE);
	arControl.user[AR_MAX_NAME_SIZE]='\0';
	strncpy(arControl.password, pw.c_str(), AR_MAX_NAME_SIZE);
	arControl.password[AR_MAX_NAME_SIZE]='\0';
	strncpy(arControl.server, server.c_str(), AR_MAX_SERVER_SIZE);
	arControl.server[AR_MAX_SERVER_SIZE]='\0';

	int nResult = ARInitialization(&this->arControl,&this->arStatus);

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
			ARNameList nameList;
			nResult = ARGetListSchema(&this->arControl,
				0,
				AR_LIST_SCHEMA_ALL | AR_HIDDEN_INCREMENT,
				NULL,
				NULL,
				NULL,
				&nameList,
				&this->arStatus);

			if(nameList.numItems == 0)
			{							
				throw(AppException(GetARStatusError(), "undefined", "ARSystem"));
			}
			else
			{
				CARServerInfo serverInfo(this->arControl, this->arStatus);
				this->srvHostName = serverInfo.GetValue(AR_SERVER_INFO_HOSTNAME);
				this->srvFullHostName = serverInfo.GetValue(AR_SERVER_INFO_FULL_HOSTNAME);
				cout << "User '" << this->arControl.user <<"' connected to server " << srvFullHostName << endl;

				LoadBlackList();
			}

			FreeARNameList(&nameList, false);
			FreeARStatusList(&this->arStatus, false);
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

void CARInside::LoadBlackList(void)
{
	try
	{
		if(appConfig.blackList.size() > 0)
		{
			ARReferenceTypeList		refTypes;
			refTypes.refType = (int *) malloc(sizeof(unsigned int) * 1);
			refTypes.numItems = 1;
			refTypes.refType[0] = ARREF_ALL;

			cout << "Loading blacklist from packinglist '" << appConfig.blackList << "'" << endl;

			CARContainer obj(appConfig.blackList, 0);

			if( ARGetContainer(&this->arControl,
				(char*)appConfig.blackList.c_str(),
				&refTypes,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL,
				&obj.references,
				NULL,
				NULL,
				NULL,					
				NULL,
				NULL,
				NULL,
				&this->arStatus) == AR_RETURN_OK)
			{
				for(unsigned int i=0; i< obj.references.numItems; i ++)
				{
					if(obj.references.referenceList[i].type >= 2 && obj.references.referenceList[i].type <= 6)
					{
						CBlackListItem *blackListItem = new CBlackListItem(obj.references.referenceList[i].type, obj.references.referenceList[i].reference.u.name);

						this->blackList.insert(blackList.end(), *blackListItem);
						LOG << "Added " << CAREnum::ContainerRefType(obj.references.referenceList[i].type) << ": '" << obj.references.referenceList[i].reference.u.name << "' to BlackList" << endl;
					}
				}
			}		
			else
				cerr << "Failed loading the blacklist: " << GetARStatusError();
		}
	} 
	catch (...)
	{ 
		cerr << "Failed loading the blacklist." << endl; 
	}
}

bool CARInside::InBlacklist(int refType, string objName)
{
	if(refType == ARREF_CONTAINER && strcmp(objName.c_str(), (char*)appConfig.blackList.c_str())==0)
		return true;

	list<CBlackListItem>::iterator bItemIter;
	CBlackListItem *bItem;

	for ( bItemIter = blackList.begin(); bItemIter != blackList.end(); bItemIter++ )
	{
		bItem = &(*bItemIter);

		if(bItem->refType == refType && strcmp(objName.c_str(), bItem->name.c_str())==0)
		{
			return true;
		}	
	}
	return false;
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
			const char* typeStr = returnTypes[min(this->arStatus.statusList[i].messageType,maxTypes)];
			strm << "[" << typeStr << this->arStatus.statusList[i].messageNum << "] ";
			strm << this->arStatus.statusList[i].messageText << endl;
			if (this->arStatus.statusList[i].appendedText != NULL) 
				strm << "  " << this->arStatus.statusList[i].appendedText << endl;
		}
	}

	FreeARStatusList(&this->arStatus, false);
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
		fName << targetFolder << "\\valid.txt";

		ofstream fout( fName.str().c_str(), ios::out);
		fout << "arinside" << endl;
		fout.close();

		nResult = remove(fName.str().c_str());
	}
	catch(...)
	{
		cout << "EXCEPTION ValidateTargetDir '" << targetFolder << "'" << endl;
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
	catch(...)
	{
		cout << "EXCEPTION An error occured validating the target path" << endl;
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
}

bool CARInside::FieldreferenceExists(int schemaInsideId, int fieldInsideId, CFieldRefItem &refItem)
{
	list<CFieldRefItem>::iterator iter;
	CFieldRefItem *item;

	for ( iter = this->listFieldRefItem.begin(); iter != this->listFieldRefItem.end(); iter++ )
	{	
		item = &(*iter);
		if(	item->fieldInsideId == fieldInsideId
			&& item->schemaInsideId == schemaInsideId
			&& item->arsStructItemType == refItem.arsStructItemType
			&& strcmp(item->fromName.c_str(), refItem.fromName.c_str()) == 0
			&& strcmp(item->description.c_str(), refItem.description.c_str()) == 0)

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

		if(ARParseXMLDocument(&this->arControl, 
			&xmlInputDoc,
			NULL,
			&parsedStream,
			&parsedObjects,
			NULL,
			&this->arStatus) == AR_RETURN_OK)
		{			
			unsigned int arInsideIdSchema = 0;
			unsigned int arInsideIdAl = 0;
			unsigned int arInsideIdFilter = 0;
			unsigned int arInsideIdEscal = 0;
			unsigned int arInsideIdCont = 0;
			unsigned int arInsideIdMenu = 0;

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
							&obj->errorOptions,
							obj->errorFilterName,
							&obj->xmlDocVersion,
							&this->arStatus) == AR_RETURN_OK)
						{
							this->filterList.insert(this->filterList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdFilter << ") [OK]" << endl;
							arInsideIdFilter++;
						}
						else
							cerr << GetARStatusError();

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
									CARGlobalField *globalField = new CARGlobalField(schema->insideId, field->insideId, field->fieldId);
									this->globalFieldList.insert(this->globalFieldList.end(), *globalField);
								}	

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
							}

							this->schemaList.insert(this->schemaList.end(), *schema);

							LOG << " (InsideID: " << arInsideIdSchema << ") [OK]" << endl;
							arInsideIdSchema++;
						}
						else
							cerr << GetARStatusError();

						//FreeARFieldInfoList(&fieldInfoList, false);
						//FreeARVuiInfoList(&vuiInfoList, false);
						FreeARStatusList(&this->arStatus, false);

					}
					break;					
				case AR_STRUCT_ITEM_XML_ACTIVE_LINK:
					{
						LOG << "Loading ActiveLink: " << parsedObjects.structItemList[i].name; 
						CARActiveLink *obj = new CARActiveLink(parsedObjects.structItemList[i].name, arInsideIdAl);

						if( ARGetActiveLinkFromXML(&this->arControl, 
							&parsedStream,
							parsedObjects.structItemList[i].name,
							NULL, 
							&obj->order,
							&obj->schemaList,
							&obj->groupList,
							&obj->executeMask,
							&obj->controlField,
							&obj->focusField,
							&obj->enable,
							&obj->query,
							&obj->actionList,
							&obj->elseList,
							NULL,
							obj->owner,
							obj->lastChanged,
							&obj->timestamp,								
							&obj->helptext,
							&obj->changeDiary,
							&obj->objPropList,
							&obj->xmlDocVersion,
							NULL,NULL,  // as of version 7.5 this two parameters should be NULL; reserverd for future use
							&this->arStatus) == AR_RETURN_OK)
						{
							this->alList.insert(this->alList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdAl << ") [OK]" << endl;
							arInsideIdAl++;								
						}
						else
							cerr << GetARStatusError();

						FreeARStatusList(&this->arStatus, false);
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
							this->menuList.insert(this->menuList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdMenu << ") [OK]" << endl;
							arInsideIdMenu++;								
						}
						else
							cerr << GetARStatusError();

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
							this->escalList.insert(this->escalList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdEscal << ") [OK]" << endl;
							arInsideIdEscal++;								
						}
						else
							cerr << GetARStatusError();

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
							this->containerList.insert(this->containerList.end(), *obj);

							LOG << " (InsideID: " << arInsideIdCont << ") [OK]" << endl;
							arInsideIdCont++;								
						}
						else
							cerr << GetARStatusError();

						FreeARStatusList(&this->arStatus, false);
					}
					break;
				}										
			}		

		}
		else
		{
			cout << "An error occured parsing the xml document '" << appConfig.objListXML << "'" << endl;
			cout << GetARStatusError();
		}

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

	//LoadServerInfoList	
	if(appConfig.bLoadServerInfoList)
	{
		cout << "Start loading Server Informations:" << endl;
		CARServerInfo serverInfo(this->arControl, this->arStatus);
		serverInfo.GetList(serverInfoList);
		cout << (unsigned int)serverInfoList.size() << " server settings loaded" << endl;
	}
	else
		cout << endl << "Loading Server Informations [SKIPPED]" << endl;

	//LoadUserList
	if(appConfig.bLoadUserList)
	{
		cout << endl << "Start laoding Users:" << endl;		

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
									CARGlobalField *globalField = new CARGlobalField(schemaInsideId, field->insideId, field->fieldId);
									this->globalFieldList.insert(this->globalFieldList.end(), *globalField);
								}
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
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARNameList(&aliasList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(schemaList);
	}
	catch(...)
	{
		cout << "EXCEPTION loading Schema " << endl;
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
				}
			}
		}

		delete (refTypes.refType);
		FreeARContainerInfoList(&conList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(containerList);		
	}
	catch(...)
	{
		cout << "EXCEPTION loading Container " << endl;
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
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(menuList);		
	}
	catch(...)
	{
		cout << "EXCEPTION loading Menus " << endl;
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
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(escalList);
	}
	catch(...)
	{
		cout << "EXCEPTION loading Escalations " << endl;
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
						&obj->errorOptions,
						obj->errorFilterName,
						&this->arStatus) == AR_RETURN_OK)
					{
						this->filterList.insert(this->filterList.end(), *obj);

						LOG << " (InsideID: " << insideId << ") [OK]" << endl;						
						insideId++;

						FreeARStatusList(&this->arStatus, false);
					}	
					else
						cerr << GetARStatusError();
				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(filterList);
	}
	catch(...)
	{
		cout << "EXCEPTION loading Filters " << endl;
		GetARStatusError();
	}

	return insideId;
}

int CARInside::LoadActiveLinks(void)
{
	int insideId=0;

	try
	{
		ARNameList nameList;

		if(ARGetListActiveLink(&this->arControl, NULL, 0, NULL, &nameList, &this->arStatus) == AR_RETURN_OK)
		{
			this->alList.clear();	
			for(unsigned int i=0; i< nameList.numItems; i++)
			{
				if(!this->InBlacklist(ARREF_ACTLINK, nameList.nameList[i]))
				{
					LOG << "Loading ActiveLink: " << nameList.nameList[i]; 
					CARActiveLink *obj = new CARActiveLink(nameList.nameList[i], insideId);

					if( ARGetActiveLink(&this->arControl, 
						nameList.nameList[i], 
						&obj->order,
						&obj->schemaList,
						&obj->groupList,
						&obj->executeMask,
						&obj->controlField,
						&obj->focusField,
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
						NULL,NULL,  // as of version 7.5 this two parameters should be NULL; reserverd for future use
						&this->arStatus) == AR_RETURN_OK)
					{
						this->alList.insert(this->alList.end(), *obj);

						LOG << " (InsideID: " << insideId << ") [OK]" << endl;						
						insideId++;

						FreeARStatusList(&this->arStatus, false);
					}		
					else
						cerr << GetARStatusError();

				}
			}
		}

		FreeARNameList(&nameList, false);
		FreeARStatusList(&this->arStatus, false);

		this->Sort(alList);
	}
	catch(...)
	{
		cout << "EXCEPTION loading ActiveLinks " << endl;
		GetARStatusError();
	}

	return insideId;
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
	string tmpStr1 = CUtil::String2Comp(t1.name.c_str());
	string tmpStr2 = CUtil::String2Comp(t2.name.c_str());
	return ( strcmp(tmpStr1.c_str(), tmpStr2.c_str()) < 0);
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
	nTmpCnt = 1;
	list<CARActiveLink>::iterator alIter;
	for ( alIter = this->alList.begin(); alIter != this->alList.end(); alIter++ )
	{	
		CARActiveLink *al = &(*alIter);

		LOG << "ActiveLink [" << nTmpCnt << "-" << alList.size() << "] '" << al->name << "': ";
		CDocAlDetails *alDetails = new CDocAlDetails(*this, *al, "active_link\\"+al->FileID(), 2);
		alDetails->Documentation();
		delete alDetails;
		nTmpCnt++;
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
	for ( filterIter = this->filterList.begin(); filterIter != this->filterList.end(); filterIter++)
	{
		CARFilter *filter = &(*filterIter);

		LOG << "Filter [" << nTmpCnt << "-" << filterList.size() << "] '" << filter->name << "': ";
		CDocFilterDetails *filterDetails = new CDocFilterDetails(*this, *filter, "filter\\"+filter->FileID(), 2);
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
	for ( escalIter = this->escalList.begin(); escalIter != this->escalList.end(); escalIter++)
	{
		CAREscalation *escal = &(*escalIter);

		LOG << "Escalation [" << nTmpCnt << "-" << escalList.size() << "] '" << escal->name << "': ";
		CDocEscalationDetails *escalDetails = new CDocEscalationDetails(*this, *escal, "escalation\\"+escal->FileID(), 2);
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
	for ( menuIter = this->menuList.begin(); menuIter != this->menuList.end(); menuIter++ )
	{
		CARCharMenu *menu = &(*menuIter);	

		LOG << "Menu [" << nTmpCnt << "-" << menuList.size() << "] '" << menu->name << "': ";
		string path = "menu\\"+menu->FileID();
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
	for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
	{
		int rootLevel = 2;

		CARSchema *schema = &(*schemaIter);		
		string path="schema\\"+schema->FileID();

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

		if(schema->insideId == schemaInsideId)
		{
			list<CARField>::iterator fieldIter;					
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);
				if(field->insideId == fieldInsideId)
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
		if(schema->insideId == schemaInsideId)
		{
			list<CARField>::iterator fieldIter;
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);
				if(field->insideId == fieldInsideId)
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
		if(schema->insideId == schemaInsideId)
		{
			list<CARField>::iterator fieldIter;					
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);
				if(field->insideId == fieldInsideId)
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
		if(insideId == schema->insideId)
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
	for ( iter = schemaList.begin(); iter != schemaList.end(); iter++ )
	{			
		CARSchema *obj = &(*iter);
		if(strcmp(obj->name.c_str(), searchObjName.c_str())==0)
		{
			return obj->insideId;
		}
	}
	return -1;
}

int CARInside::AlGetInsideId(string searchObjName)
{
	list<CARActiveLink>::iterator iter;			
	for ( iter = alList.begin(); iter != alList.end(); iter++ )
	{			
		CARActiveLink *obj = &(*iter);
		if(strcmp(obj->name.c_str(), searchObjName.c_str())==0)
		{
			return obj->insideId;
		}
	}
	return -1;
}

int CARInside::FilterGetInsideId(string searchObjName)
{
	list<CARFilter>::iterator iter;			
	for ( iter = filterList.begin(); iter != filterList.end(); iter++ )
	{			
		CARFilter *obj = &(*iter);
		if(strcmp(obj->name.c_str(), searchObjName.c_str())==0)
		{
			return obj->insideId;
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
			return obj->insideId;
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
			return obj->insideId;
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
			return obj->insideId;
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
	list<CARActiveLink>::iterator alIter;	
	for ( alIter = this->alList.begin(); alIter != this->alList.end(); alIter++ )
	{
		CARActiveLink *al = &(*alIter);
		if(al->insideId == alInsideId)
		{
			stringstream strmTmp;
			strmTmp.str("");

			strmTmp << CWebUtil::Link(al->name, CWebUtil::RootPath(rootLevel)+"active_link/"+al->FileID()+"/"+CWebUtil::DocName("index"), "active_link.gif", rootLevel) << " (" << al->order << ")";
			return strmTmp.str();
		}
	}
	return EmptyValue;
}

string CARInside::LinkToAl(string alName, int fromRootLevel)
{
	list<CARActiveLink>::iterator alIter;	
	for ( alIter = this->alList.begin(); alIter != this->alList.end(); alIter++ )
	{
		CARActiveLink *al = &(*alIter);
		if(strcmp(alName.c_str(), al->name.c_str())==0)
		{
			return al->GetURL(fromRootLevel);
		}
	}

	return alName;
}

string CARInside::LinkToFilterRef(int filterInsideId, int rootLevel)
{	
	list<CARFilter>::iterator filterIter;	
	for ( filterIter = this->filterList.begin(); filterIter != this->filterList.end(); filterIter++ )
	{	
		CARFilter *filter = &(*filterIter);
		if(filter->insideId == filterInsideId)
		{
			stringstream strmTmp;
			strmTmp.str("");

			strmTmp << CWebUtil::Link(filter->name, CWebUtil::RootPath(rootLevel)+"filter/"+filter->FileID()+"/"+CWebUtil::DocName("index"), "filter.gif", rootLevel) << " (" << filter->order << ")";
			return strmTmp.str();
		}
	}
	return EmptyValue;
}

string CARInside::LinkToFilter(string filterName, int fromRootLevel)
{
	list<CARFilter>::iterator filterIter;	
	for ( filterIter = this->filterList.begin(); filterIter != this->filterList.end(); filterIter++ )
	{	
		CARFilter *filter = &(*filterIter);
		if(strcmp(filterName.c_str(), filter->name.c_str())==0)
		{
			return filter->GetURL(fromRootLevel);
		}
	}
	return filterName;
}

string CARInside::LinkToMenu(string menuName, int fromRootLevel)
{
	list<CARCharMenu>::iterator menuIter;	
	for ( menuIter = this->menuList.begin(); menuIter != this->menuList.end(); menuIter++ )
	{	
		CARCharMenu *menu = &(*menuIter);
		if(strcmp(menuName.c_str(), menu->name.c_str())==0)
		{
			return menu->GetURL(fromRootLevel);
		}
	}
	return EmptyValue;
}

string CARInside::LinkToEscalation(string escalationName, int fromRootLevel)
{
	list<CAREscalation>::iterator escalIter;	
	for ( escalIter = this->escalList.begin(); escalIter != this->escalList.end(); escalIter++ )
	{	
		CAREscalation *escal = &(*escalIter);
		if(strcmp(escalationName.c_str(), escal->name.c_str())==0)
		{
			return escal->GetURL(fromRootLevel);			
		}
	}
	return escalationName;
}

string CARInside::LinkToContainer(string containerName, int fromRootLevel)
{
	list<CARContainer>::iterator contIter;
	for ( contIter = this->containerList.begin(); contIter != this->containerList.end(); contIter++ )
	{	
		CARContainer *container = &(*contIter);
		if(strcmp(containerName.c_str(), container->name.c_str())==0)
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

string CARInside::LinkToXmlObjType(int arsStructItemType, string objName, int rootLevel)
{
	string result = EmptyValue;

	switch(arsStructItemType)
	{
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK: 
		{
			result = this->LinkToAlRef(AlGetInsideId(objName), rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_FILTER:
		{
			result = this->LinkToFilterRef(FilterGetInsideId(objName), rootLevel);
		}
		break;
	case AR_STRUCT_ITEM_XML_SCHEMA:
		{
			result = this->LinkToSchema(objName, rootLevel);
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
	}

	return result;
}


string CARInside::XmlObjEnabled(int arsStructItemType, string objName)
{
	string result = "";

	switch(arsStructItemType)
	{
	case AR_STRUCT_ITEM_XML_ACTIVE_LINK: 
		{
			list<CARActiveLink>::iterator alIter;			
			for ( alIter = this->alList.begin(); alIter != this->alList.end(); alIter++ )
			{	
				CARActiveLink *al = &(*alIter);
				if(strcmp(objName.c_str(), al->name.c_str())==0)
				{
					result = CAREnum::ObjectEnable(al->enable);					
				}
			}
		}
		break;
	case AR_STRUCT_ITEM_XML_FILTER:
		{
			list<CARFilter>::iterator filterIter;			
			for ( filterIter = this->filterList.begin(); filterIter != this->filterList.end(); filterIter++ )
			{	
				CARFilter *filter = &(*filterIter);
				if(strcmp(objName.c_str(), filter->name.c_str())==0)
				{
					result = CAREnum::ObjectEnable(filter->enable);		
				}
			}
		}
		break;		
	case AR_STRUCT_ITEM_XML_ESCALATION:
		{
			list<CAREscalation>::iterator escalIter;			
			for ( escalIter = this->escalList.begin(); escalIter != this->escalList.end(); escalIter++ )
			{	
				CAREscalation *escal = &(*escalIter);
				if(strcmp(objName.c_str(), escal->name.c_str())==0)
				{
					result = CAREnum::ObjectEnable(escal->enable);		
				}
			}
		}
		break;	
	default:
		{
			result = "";
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

		list<CARSchema>::iterator schemaIter;
		for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
		{			
			CARSchema *schema = &(*schemaIter);

			list<CARField>::iterator fieldIter;		
			for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
			{
				CARField *field = &(*fieldIter);

				CustomFieldReferences(*schema, *field);
			}

			cout << ".";
		}

		cout << "]" << endl;
	}
	catch(...)
	{
		cout << "EXCEPTION SearchCustomFieldReferences" << endl;
	}
}

//Find references at startup
//This function is necessary because when we wouldt try to add the reference at runtime for example
//Columns might be enumerated at a time when the datafield has been already saved to html file
void CARInside::CustomFieldReferences(CARSchema &schema, CARField &obj)
{
	try
	{
		int rootLevel = 2;

		stringstream strm;
		strm.str("");

		switch(obj.dataType)
		{		
		case AR_DATA_TYPE_COLUMN: //Find column references
			{
				ARColumnLimitsStruct fLimit = obj.limit.u.columnLimits;

				//To create a link to the datafield we first must find the target schema of the table
				int tblTargetSchemaInsideId = schema.insideId;

				list<CARSchema>::iterator schemaIter;						
				for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
				{			
					CARSchema *searchSchema = &(*schemaIter);
					if(schema.insideId == searchSchema->insideId)
					{
						list<CARField>::iterator fieldIter;		
						for( fieldIter = searchSchema->fieldList.begin(); fieldIter != searchSchema->fieldList.end(); fieldIter++)
						{
							CARField *field = &(*fieldIter);
							if(field->insideId == fLimit.parent) // we found the table
							{				
								if(field->limit.dataType == AR_DATA_TYPE_TABLE)
								{
									ARTableLimitsStruct tblLimits = field->limit.u.tableLimits;

									if(!strcmp(tblLimits.schema, AR_CURRENT_SCHEMA_TAG)==0)
									{
										tblTargetSchemaInsideId = SchemaGetInsideId(tblLimits.schema);
									}
								}
							}
						}
					}
				}

				//Add a reference
				CFieldRefItem *refItem = new CFieldRefItem();
				refItem->arsStructItemType = AR_STRUCT_ITEM_XML_SCHEMA;
				refItem->description = "Datasource in Table: " + LinkToField(schema.insideId, fLimit.parent, rootLevel) + " Field: " + LinkToField(schema.insideId, obj.insideId, rootLevel);
				refItem->fromName = schema.name;
				refItem->fieldInsideId = fLimit.dataField;
				refItem->schemaInsideId = tblTargetSchemaInsideId;
				AddReferenceItem(refItem);
				delete refItem;
			}
			break;
		case AR_DATA_TYPE_TABLE:
			{
				ARTableLimitsStruct fLimit = obj.limit.u.tableLimits;

				string tmpSchema = fLimit.schema;
				if(strcmp(fLimit.schema, AR_CURRENT_SCHEMA_TAG) == 0)
				{
					tmpSchema = schema.name;
				}

				stringstream strmQuery;
				if(fLimit.qualifier.operation != NULL)
				{		
					CFieldRefItem *refItem = new CFieldRefItem();
					refItem->arsStructItemType = AR_STRUCT_ITEM_XML_SCHEMA;
					refItem->description = "Table Qualification: " +  obj.GetURL(rootLevel);
					refItem->fromName = schema.name;

					CARQualification arQual(*this);
					int pFormId = SchemaGetInsideId(schema.name.c_str());
					int sFormId = SchemaGetInsideId(tmpSchema.c_str());

					arQual.CheckQuery(&fLimit.qualifier, *refItem, 0, pFormId, sFormId, strmQuery, rootLevel);
					delete refItem;
				}
			}
			break;
		}
	}
	catch(...)
	{
		cout << "EXCEPTION CustomFieldReferences" << endl;
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
	catch(...)
	{
		cout << "EXCEPTION AddReferenceItem" << endl;
	}
}

string CARInside::TextFindFields(string inText, string fieldSeparator, int schemaInsideId, int rootLevel, bool findKeywords, CFieldRefItem *refItem)
{	

	try
	{
		if(inText.size() == 0)
			return "";

		string dummySeparator = "##"; //dummy placeholder to avoid infinite replacements

		list<CARSchema>::iterator schemaIter;				
		for ( schemaIter = schemaList.begin(); schemaIter != schemaList.end(); schemaIter++ )
		{			
			CARSchema *schema = &(*schemaIter);
			if(schema->insideId == schemaInsideId)
			{
				list<CARField>::iterator fieldIter;		
				CARField *field;

				for( fieldIter = schema->fieldList.begin(); fieldIter != schema->fieldList.end(); fieldIter++)
				{
					field = &(*fieldIter);

					stringstream strmTmp;
					strmTmp << fieldSeparator << field->fieldId << fieldSeparator;			

					stringstream fieldLink;
					fieldLink << dummySeparator << field->GetURL(rootLevel) << dummySeparator;					

					unsigned int nLengthOrg = (unsigned int)inText.length();

					string fField = strmTmp.str();
					inText = CUtil::StrReplace(fField, fieldLink.str(), inText);

					//Finally replace the dummy placeholder
					inText = CUtil::StrReplace(dummySeparator, fieldSeparator, inText);

					//if the string is longer because we have added a link (long string) we add a field reference
					if(inText.length() > nLengthOrg) 
					{						
						refItem->fieldInsideId = field->fieldId;
						AddReferenceItem(refItem);
					}
				}
			}
		}

		if(findKeywords == true)
		{
			inText = TextFindKeywords(inText, fieldSeparator);
		}
	}
	catch(...)
	{
		cout << "EXCEPTION in TextFindField" << endl;
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
	catch(...)
	{
		cout << "EXCEPTION in TextFindField" << endl;
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
			if(schema->insideId == schemaInsideId)
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
	catch(...)
	{
		cout << "EXCEPTION in XMLFindField" << endl;
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
	catch(...)
	{
		cout << "EXCEPTION writing dataobject history " << endl;
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
		if(obj->changeDiary != NULL)
		{				
			ARDiaryList diaryList;
			if(ARDecodeDiary(&this->arControl, obj->changeDiary, &diaryList, &this->arStatus)==AR_RETURN_OK)
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
		tblRow.AddCellList(CTableCell("Owner"), CTableCell(this->LinkToUser(obj->owner, rootLevel)));
		tbl.AddRow(tblRow);

		//Last changed		
		stringstream strmLastChanged;
		strmLastChanged.str("");
		strmLastChanged << CUtil::DateTimeToHTMLString(obj->timestamp) << " " << this->LinkToUser(obj->lastChanged, rootLevel) << endl;

		tblRow.AddCellList(CTableCell("Last changed"), CTableCell(strmLastChanged.str()));
		tbl.AddRow(tblRow);


		//History
		tblRow.AddCellList(CTableCell("History Log"), CTableCell(historyLog.str()));
		tbl.AddRow(tblRow);

		//Helptext
		string tmpHelptext = EmptyValue;
		if(obj->helptext != NULL)
		{
			tmpHelptext = CUtil::StrReplace("\n", "<br/>", obj->helptext);
		}

		tblRow.AddCellList(CTableCell("Helptext"), CTableCell(tmpHelptext));
		tbl.AddRow(tblRow);

		strm << tbl.ToXHtml();
		tbl.Clear();
	}
	catch(...)
	{
		cout << "EXCEPTION writing server object history " << endl;
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
	typedef map<string,list<string>> ErrorCallMap;
	typedef pair<string,list<string>> ErrCallPair;

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

	// build needed reference tables
	BuildReferences();

	// write documentation
	Documentation();
}