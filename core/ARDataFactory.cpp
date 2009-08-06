
/****************************************************************************** 
 * 
 *  file:  ARDataFactory.cpp
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

#include "StdAfx.h"
#include ".\ardatafactory.h"
#include "..\arinside.h"

#define LOG if(verboseMode) cout

CARDataFactory::CARDataFactory(ARControlStruct &control, ARStatusList &status)
{
	this->pControl = &control;
	this->pStatus = &status;
}

CARDataFactory::~CARDataFactory(void)
{	
	FreeARStatusList(pStatus, false);
}

void CARDataFactory::GetListGroup(AppConfig &appConfig, list<CARGroup> &listResult)
{	
	try
	{
		ARQualifierStruct	qualifier;

		ARNameType schemaName;
		strncpy(schemaName, appConfig.groupForm.c_str(), AR_MAX_NAME_SIZE);

		char *qualString;
		qualString = new char[appConfig.groupQuery.size() + 1];
		strcpy(qualString, appConfig.groupQuery.c_str());

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			unsigned int numMatches;
			AREntryListList		entryList;	
			if(ARGetListEntry(this->pControl,
					schemaName,
					&qualifier,
					NULL,
					NULL,
					AR_START_WITH_FIRST_ENTRY,
					appConfig.maxRetrieve,
					FALSE,
					&entryList,
					&numMatches,
					this->pStatus) == AR_RETURN_OK)
			{
				if(entryList.entryList != NULL)
				{
					for(unsigned int i=0; i< entryList.numItems; i++)
					{
						if(entryList.entryList[i].entryId.numItems >0)
						{							
							CARGroup grp = this->LoadGroup(schemaName, entryList.entryList[i].entryId.entryIdList[0]);
							if(grp.groupName != "")
							{
								listResult.push_back(grp);
							}							
						}
					}
				}
			}
			FreeAREntryListList(&entryList, false);
		}

		delete[] qualString;
	}
	catch(...)
	{		
		throw(AppException("Error loading groups.", "undefined", "DataFactory"));
	}
}

CARGroup CARDataFactory::LoadGroup(ARNameType &schemaName, string requestId)
{
	CARGroup *grp = new CARGroup(requestId);
	try
	{
		ARInternalIdList idList;
		ARFieldValueList fieldList;
		
		idList.numItems = 9;
		idList.internalIdList = (ARInternalId *) malloc (sizeof(ARInternalId)* idList.numItems);			
		idList.internalIdList[0] = 1;		//RequestId
		idList.internalIdList[1] = 105;		//GroupName
		idList.internalIdList[2] = 106;		//GroupId
		idList.internalIdList[3] = 107;		//GroupType
		idList.internalIdList[4] = 8;		//LongGroupName
		idList.internalIdList[5] = 2;		//CreatedBy
		idList.internalIdList[6] = 3;		//Created
		idList.internalIdList[7] = 5;		//ModifiedBy
		idList.internalIdList[8] = 6;		//Modified
				
		AREntryIdList entryId;
		entryId.numItems=1;
		entryId.entryIdList=(AREntryIdType *)malloc(sizeof(AREntryIdType));
		strcpy(entryId.entryIdList[0], requestId.c_str());

		if(ARGetEntry(this->pControl,
			schemaName,
			&entryId,
			&idList,
			&fieldList,
			this->pStatus) == AR_RETURN_OK)
		{      
			grp->groupName = fieldList.fieldValueList[1].value.u.charVal;
			grp->name = grp->groupName;

			grp->groupId   = fieldList.fieldValueList[2].value.u.intVal;
			grp->insideId  = fieldList.fieldValueList[2].value.u.intVal;
			grp->groupType	= fieldList.fieldValueList[3].value.u.intVal;

			if(fieldList.fieldValueList[4].value.u.charVal != NULL)
				grp->longGroupName = fieldList.fieldValueList[4].value.u.charVal;

			if(fieldList.fieldValueList[5].value.u.charVal != NULL)
				grp->createdBy = fieldList.fieldValueList[5].value.u.charVal;

			if(fieldList.fieldValueList[6].value.u.timeVal != NULL)
				grp->created = fieldList.fieldValueList[6].value.u.timeVal;

			if(fieldList.fieldValueList[7].value.u.charVal != NULL)
				grp->modifiedBy = fieldList.fieldValueList[7].value.u.charVal;

			if(fieldList.fieldValueList[8].value.u.timeVal != NULL)
				grp->modified = fieldList.fieldValueList[8].value.u.timeVal;

			LOG << "Group '" << grp->groupName <<"' [OK]" << endl;		
		}
		else
		{
			LOG << "Group '" << requestId <<"' [ERROR]" << endl;		
		}


		delete[] entryId.entryIdList;
		delete idList.internalIdList;	
		FreeARFieldValueList(&fieldList,  false);
	}
	catch(...)
	{		
		cout << "WARNING error loading group: " << requestId << endl;
	}
		
	return *grp;
}

bool CARDataFactory::SortGroupsByName(const CARGroup& t1, const CARGroup& t2 )
{
	string tmpStr1 = CUtil::String2Comp(t1.groupName.c_str());
	string tmpStr2 = CUtil::String2Comp(t2.groupName.c_str());
	return ( strcmp(tmpStr1.c_str(), tmpStr2.c_str()) < 0);
}

void CARDataFactory::Sort(list<CARGroup> &listResult)
{	
	listResult.sort(SortGroupsByName);
}

void CARDataFactory::GetListUser(AppConfig &appConfig, list<CARUser> &listResult)
{
	try
	{
		ARQualifierStruct	qualifier;

		ARNameType schemaName;
		strncpy(schemaName, appConfig.userForm.c_str(), AR_MAX_NAME_SIZE);

		char *qualString;
		qualString = new char[appConfig.userQuery.size() + 1];
		strcpy(qualString, appConfig.userQuery.c_str());

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			unsigned int numMatches;
			AREntryListList		entryList;	
			if(ARGetListEntry(this->pControl,
					schemaName,
					&qualifier,
					NULL,
					NULL,
					AR_START_WITH_FIRST_ENTRY,
					appConfig.maxRetrieve,
					FALSE,
					&entryList,
					&numMatches,
					this->pStatus) == AR_RETURN_OK)
			{
				if(entryList.entryList != NULL)
				{
					for(unsigned int i=0; i< entryList.numItems; i++)
					{
						if(entryList.entryList[i].entryId.numItems == 1)
						{
							CARUser arUser = this->LoadUser(schemaName, entryList.entryList[i].entryId.entryIdList[0], i);
							if(arUser.loginName != "")
							{
								listResult.push_back(arUser);
							}
						}
					}
				}
			}
			FreeAREntryListList(&entryList, false);

		}

		delete[] qualString;
	}
	catch(...)
	{		
		throw(AppException("Error loading users.", "undefined", "DataFactory"));
	}
}

CARUser CARDataFactory::LoadUser(ARNameType &schemaName, string requestId, int insideId)
{
	CARUser *arUser = new CARUser(requestId);
	try
	{
		ARInternalIdList idList;
		ARFieldValueList fieldList;

		idList.numItems = 12;
		idList.internalIdList = (ARInternalId *) malloc (sizeof(ARInternalId)* idList.numItems);			
		
		idList.internalIdList[0] = 1;		//RequestId
		idList.internalIdList[1] = 101;		//LoginName
		idList.internalIdList[2] = 103;		//Email
		idList.internalIdList[3] = 104;		//GroupList
		idList.internalIdList[4] = 8;		//FullName
		idList.internalIdList[5] = 108;		//DefNorify
		idList.internalIdList[6] = 109;		//LicType
		idList.internalIdList[7] = 110;		//FtLicType
		idList.internalIdList[8] = 2;		//CreatedBy
		idList.internalIdList[9] = 3;		//Created
		idList.internalIdList[10] = 5;		//ModifiedBy
		idList.internalIdList[11] = 6;		//Modified	
				
		AREntryIdList entryId;
		entryId.numItems=1;
		entryId.entryIdList=(AREntryIdType *)malloc(sizeof(AREntryIdType));
		strcpy(entryId.entryIdList[0], requestId.c_str());

		if( ARGetEntry(this->pControl,
			schemaName,
			&entryId,
			&idList,
			&fieldList,
			this->pStatus) ==  AR_RETURN_OK)
		{				
			arUser->insideId = insideId;

			if(fieldList.fieldValueList[1].value.u.charVal != NULL)
			{
				arUser->loginName = fieldList.fieldValueList[1].value.u.charVal;
				arUser->name = arUser->loginName;
			}

			if(fieldList.fieldValueList[2].value.u.charVal != NULL)
				arUser->email = fieldList.fieldValueList[2].value.u.charVal;

			arUser->groupList.clear();
			if(fieldList.fieldValueList[3].value.u.charVal != NULL)
			{
				string tmpGroupList = fieldList.fieldValueList[3].value.u.charVal;
//				CUtil::SplitString(tmpGroupList, ";", arUser->groupList, false);

				CUtil::SplitString(tmpGroupList, arUser->groupList);

			}

			if(fieldList.fieldValueList[4].value.u.charVal != NULL)
				arUser->fullName = fieldList.fieldValueList[4].value.u.charVal;

			if(fieldList.fieldValueList[5].value.u.intVal != NULL)
				arUser->defNotify = fieldList.fieldValueList[5].value.u.intVal;
			
			if(fieldList.fieldValueList[6].value.u.intVal != NULL)			
				arUser->licenseType = fieldList.fieldValueList[6].value.u.intVal;

			if(fieldList.fieldValueList[7].value.u.intVal != NULL)		
				arUser->ftLicenseType = fieldList.fieldValueList[7].value.u.intVal;

			if(fieldList.fieldValueList[8].value.u.charVal != NULL)
				arUser->createdBy = fieldList.fieldValueList[8].value.u.charVal;

			if(fieldList.fieldValueList[9].value.u.timeVal != NULL)
				arUser->created = fieldList.fieldValueList[9].value.u.timeVal;

			if(fieldList.fieldValueList[10].value.u.charVal != NULL)
				arUser->modifiedBy = fieldList.fieldValueList[10].value.u.charVal;

			if(fieldList.fieldValueList[11].value.u.timeVal != NULL)
				arUser->modified = fieldList.fieldValueList[11].value.u.timeVal;

			LOG << "User '" << arUser->loginName <<"' [OK]" << endl;
		}
		else
		{
			LOG << "User '" << requestId <<"' [ERROR]" << endl;
		}
	    
		delete[] entryId.entryIdList;
		delete idList.internalIdList;	
		FreeARFieldValueList(&fieldList,  false);
		FreeARStatusList(this->pStatus, false);
	}
	catch(...)
	{		
		cout << "WARNING error loading user: " << requestId << endl;
	}

	return *arUser;
}

bool CARDataFactory::SortUsersByName(const CARUser& t1, const CARUser& t2 )
{
	string tmpStr1 = CUtil::String2Comp(t1.loginName.c_str());
	string tmpStr2 = CUtil::String2Comp(t2.loginName.c_str());
	return ( strcmp(tmpStr1.c_str(), tmpStr2.c_str()) < 0);
}

void CARDataFactory::Sort(list<CARUser> &listResult)
{	
	listResult.sort(SortUsersByName);
}

void CARDataFactory::GetListRoles(AppConfig &appConfig, list<CARRole> &listResult)
{	
	try
	{
		ARQualifierStruct	qualifier;

		ARNameType schemaName;
		strncpy(schemaName, appConfig.roleForm.c_str(), AR_MAX_NAME_SIZE);

		char *qualString;
		qualString = new char[appConfig.roleQuery.size() + 1];
		strcpy(qualString, appConfig.roleQuery.c_str());

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			unsigned int numMatches;
			AREntryListList		entryList;	
			if(ARGetListEntry(this->pControl,
					schemaName,
					&qualifier,
					NULL,
					NULL,
					AR_START_WITH_FIRST_ENTRY,
					appConfig.maxRetrieve,
					FALSE,
					&entryList,
					&numMatches,
					this->pStatus) == AR_RETURN_OK)
			{
				if(entryList.entryList != NULL)
				{
					for(unsigned int i=0; i< entryList.numItems; i++)
					{
						if(entryList.entryList[i].entryId.numItems >0)
						{							
							CARRole role = this->LoadRole(schemaName, entryList.entryList[i].entryId.entryIdList[0], i);
							if(role.roleName != "")
							{
								listResult.push_back(role);
							}							
						}
					}
				}
			}
			FreeAREntryListList(&entryList, false);
		}

		delete[] qualString;
	}
	catch(...)
	{		
		throw(AppException("Error loading roles.", "undefined", "DataFactory"));
	}
}

CARRole CARDataFactory::LoadRole(ARNameType &schemaName, string requestId, int insideId)
{	
	CARRole *role = new CARRole(requestId);

	try
	{
		ARInternalIdList idList;
		ARFieldValueList fieldList;
		
		idList.numItems = 10;
		idList.internalIdList = (ARInternalId *) malloc (sizeof(ARInternalId)* idList.numItems);			
		idList.internalIdList[0] = 1;		//RequestId
		idList.internalIdList[1] = 1700;	//ApplicationName
		idList.internalIdList[2] = 1701;	//RoleName
		idList.internalIdList[3] = 1702;	//RoleID
		idList.internalIdList[4] = 2001;	//GroupName Test
		idList.internalIdList[5] = 2002;	//GroupName Production
		idList.internalIdList[6] = 2;		//CreatedBy
		idList.internalIdList[7] = 3;		//Created
		idList.internalIdList[8] = 5;		//ModifiedBy
		idList.internalIdList[9] = 6;		//Modified
				
		AREntryIdList entryId;
		entryId.numItems=1;
		entryId.entryIdList=(AREntryIdType *)malloc(sizeof(AREntryIdType));
		strcpy(entryId.entryIdList[0], requestId.c_str());

		if(ARGetEntry(this->pControl,
			schemaName,
			&entryId,
			&idList,
			&fieldList,
			this->pStatus) == AR_RETURN_OK)
		{   
			role->insideId = insideId;

			if(fieldList.fieldValueList[1].value.u.charVal != NULL)
				role->applicationName = fieldList.fieldValueList[1].value.u.charVal;
			
			if(fieldList.fieldValueList[2].value.u.charVal != NULL)
			{
				role->roleName = fieldList.fieldValueList[2].value.u.charVal;
				role->name = role->roleName;
			}

			if(fieldList.fieldValueList[3].value.u.charVal != NULL)
				role->roleId = fieldList.fieldValueList[3].value.u.intVal;
			
			if(fieldList.fieldValueList[4].value.u.charVal != NULL)
			{
				string tmp = fieldList.fieldValueList[4].value.u.charVal;

				vector<string> tmpGroupList;
				tmpGroupList.clear();
				
//				CUtil::SplitString(tmp, ";", tmpGroupList, false);
				CUtil::SplitString(tmp, tmpGroupList);

				role->testGroupId = atoi(tmpGroupList[0].c_str());
			}

			if(fieldList.fieldValueList[5].value.u.charVal != NULL)
			{
				string tmp = fieldList.fieldValueList[5].value.u.charVal;

				vector<string> tmpGroupList;
				tmpGroupList.clear();

//				CUtil::SplitString(tmp, ";", tmpGroupList, false);
				CUtil::SplitString(tmp, tmpGroupList);


				role->productionGroupId = atoi(tmpGroupList[0].c_str());
			}

			if(fieldList.fieldValueList[6].value.u.charVal != NULL)
				role->createdBy = fieldList.fieldValueList[6].value.u.charVal;

			if(fieldList.fieldValueList[7].value.u.timeVal != NULL)
				role->created = fieldList.fieldValueList[7].value.u.timeVal;

			if(fieldList.fieldValueList[8].value.u.charVal != NULL)
				role->modifiedBy = fieldList.fieldValueList[8].value.u.charVal;

			if(fieldList.fieldValueList[9].value.u.timeVal != NULL)
				role->modified = fieldList.fieldValueList[9].value.u.timeVal;

			LOG << "Role '" << role->roleName <<"' [OK]" << endl;		
		}
		else
		{
			LOG << "Role '" << requestId <<"' [ERROR]" << endl;		
		}

		delete[] entryId.entryIdList;
		delete idList.internalIdList;	
		FreeARFieldValueList(&fieldList,  false);
	}
	catch(...)
	{		
		cout << "WARNING error loading role: " << requestId << endl;
	}
		
	return *role;
}

bool CARDataFactory::SortRolesByName(const CARRole& t1, const CARRole& t2 )
{
	string tmpStr1 = CUtil::String2Comp(t1.roleName.c_str());
	string tmpStr2 = CUtil::String2Comp(t2.roleName.c_str());
	return ( strcmp(tmpStr1.c_str(), tmpStr2.c_str()) < 0);
}

void CARDataFactory::Sort(list<CARRole> &listResult)
{	
	listResult.sort(SortRolesByName);
}
