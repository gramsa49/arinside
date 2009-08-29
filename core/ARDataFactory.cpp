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

#include "StdAfx.h"
#include ".\ardatafactory.h"
#include "..\arinside.h"

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

		AREntryListFieldList fields;
		AREntryListFieldValueList values;

		fields.numItems = 9;
		fields.fieldsList = (AREntryListFieldStruct*)new AREntryListFieldStruct[fields.numItems];
		memset(fields.fieldsList,0,sizeof(AREntryListFieldStruct)*fields.numItems);

		fields.fieldsList[0].fieldId = 1;     //RequestId
		fields.fieldsList[1].fieldId = 105;   //GroupName
		fields.fieldsList[2].fieldId = 106;   //GroupId
		fields.fieldsList[3].fieldId = 107;   //GroupType
		fields.fieldsList[4].fieldId = 8;     //LongGroupName
		fields.fieldsList[5].fieldId = 2;     //CreatedBy
		fields.fieldsList[6].fieldId = 3;     //Created
		fields.fieldsList[7].fieldId = 5;     //ModifiedBy
		fields.fieldsList[8].fieldId = 6;     //Modified

		for (unsigned int k=0; k<fields.numItems; ++k) { fields.fieldsList[k].columnWidth=1; fields.fieldsList[k].separator[0]='|'; }

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			if (ARGetListEntryWithFields(
				this->pControl,
				schemaName,
				&qualifier,
				&fields,
				NULL,0,0,0,  // sort, first, max, locale
				&values,
				0,this->pStatus) == AR_RETURN_OK)
			{
				for (unsigned int row=0; row<values.numItems; ++row)
				{
					if (values.entryList[row].entryValues->fieldValueList[0].value.u.charVal == NULL) continue;
					
					CARGroup grp(values.entryList[row].entryValues->fieldValueList[0].value.u.charVal);
					grp.groupName = values.entryList[row].entryValues->fieldValueList[1].value.u.charVal;
					grp.name = grp.groupName;
					grp.comparableName = CUtil::String2Comp(grp.groupName);
					grp.groupId   = values.entryList[row].entryValues->fieldValueList[2].value.u.intVal;
					grp.insideId  = values.entryList[row].entryValues->fieldValueList[2].value.u.intVal;
					grp.groupType	= values.entryList[row].entryValues->fieldValueList[3].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[4].value.u.charVal != NULL)
						grp.longGroupName = values.entryList[row].entryValues->fieldValueList[4].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[5].value.u.charVal != NULL)
						grp.createdBy = values.entryList[row].entryValues->fieldValueList[5].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[6].value.u.timeVal != NULL)
						grp.created = values.entryList[row].entryValues->fieldValueList[6].value.u.timeVal;

					if(values.entryList[row].entryValues->fieldValueList[7].value.u.charVal != NULL)
						grp.modifiedBy = values.entryList[row].entryValues->fieldValueList[7].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[8].value.u.timeVal != NULL)
						grp.modified = values.entryList[row].entryValues->fieldValueList[8].value.u.timeVal;

					LOG << "Group '" << grp.groupName <<"' [OK]" << endl;

					listResult.push_back(grp);
				}
				FreeAREntryListFieldValueList(&values,false);
			}
		}

		delete[] fields.fieldsList;
		delete[] qualString;
	}
	catch(...)
	{		
		throw(AppException("Error loading groups.", "undefined", "DataFactory"));
	}
}

// TODO: function currently unused
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
	return t1.comparableName.compare(t2.comparableName) < 0;
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

		AREntryListFieldList fields;
		AREntryListFieldValueList values;

		fields.numItems = 12;
		fields.fieldsList = (AREntryListFieldStruct*)new AREntryListFieldStruct[fields.numItems];
		memset(fields.fieldsList,0,sizeof(AREntryListFieldStruct)*fields.numItems);

		fields.fieldsList[0].fieldId = 1;     //RequestId
		fields.fieldsList[1].fieldId = 101;   //LoginName
		fields.fieldsList[2].fieldId = 103;   //Email
		fields.fieldsList[3].fieldId = 104;   //GroupList
		fields.fieldsList[4].fieldId = 8;     //FullName
		fields.fieldsList[5].fieldId = 108;   //DefNorify
		fields.fieldsList[6].fieldId = 109;   //LicType
		fields.fieldsList[7].fieldId = 110;   //FtLicType
		fields.fieldsList[8].fieldId = 2;     //CreatedBy
		fields.fieldsList[9].fieldId = 3;     //Created
		fields.fieldsList[10].fieldId = 5;    //ModifiedBy
		fields.fieldsList[11].fieldId = 6;    //Modified
		
		for (unsigned int k=0; k<fields.numItems; ++k) { fields.fieldsList[k].columnWidth=1; fields.fieldsList[k].separator[0]='|'; }

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			if (ARGetListEntryWithFields(
				this->pControl,
				schemaName,
				&qualifier,
				&fields,
				NULL,0,0,0, // sort, first, max, locale,
				&values,
				0,this->pStatus) == AR_RETURN_OK)
			{
				for (unsigned int row=0; row<values.numItems; ++row)
				{
					if (values.entryList[row].entryValues->fieldValueList[0].value.u.charVal == NULL) continue;

					CARUser arUser(values.entryList[row].entryValues->fieldValueList[0].value.u.charVal);
					arUser.insideId = row;

					if(values.entryList[row].entryValues->fieldValueList[1].value.u.charVal != NULL)
					{
						arUser.loginName = values.entryList[row].entryValues->fieldValueList[1].value.u.charVal;
						arUser.name = arUser.loginName;
						arUser.comparableName = CUtil::String2Comp(arUser.loginName);
					}

					if(values.entryList[row].entryValues->fieldValueList[2].value.u.charVal != NULL)
						arUser.email = values.entryList[row].entryValues->fieldValueList[2].value.u.charVal;

					arUser.groupList.clear();
					if(values.entryList[row].entryValues->fieldValueList[3].value.u.charVal != NULL)
					{
						string tmpGroupList = values.entryList[row].entryValues->fieldValueList[3].value.u.charVal;
						CUtil::SplitString(tmpGroupList, arUser.groupList);
					}

					if(values.entryList[row].entryValues->fieldValueList[4].value.u.charVal != NULL)
						arUser.fullName = values.entryList[row].entryValues->fieldValueList[4].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[5].value.u.intVal != NULL)
						arUser.defNotify = values.entryList[row].entryValues->fieldValueList[5].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[6].value.u.intVal != NULL)			
						arUser.licenseType = values.entryList[row].entryValues->fieldValueList[6].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[7].value.u.intVal != NULL)		
						arUser.ftLicenseType = values.entryList[row].entryValues->fieldValueList[7].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[8].value.u.charVal != NULL)
						arUser.createdBy = values.entryList[row].entryValues->fieldValueList[8].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal != NULL)
						arUser.created = values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal;

					if(values.entryList[row].entryValues->fieldValueList[10].value.u.charVal != NULL)
						arUser.modifiedBy = values.entryList[row].entryValues->fieldValueList[10].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[11].value.u.timeVal != NULL)
						arUser.modified = values.entryList[row].entryValues->fieldValueList[11].value.u.timeVal;

					LOG << "User '" << arUser.loginName <<"' [OK]" << endl;
					
					listResult.push_back(arUser);
				}
				FreeAREntryListFieldValueList(&values,false);
			}
		}

		delete[] fields.fieldsList;
		delete[] qualString;
	}
	catch(...)
	{		
		throw(AppException("Error loading users.", "undefined", "DataFactory"));
	}
}

// TODO: function currently unused
CARUser CARDataFactory::LoadUser(ARNameType &schemaName, string requestId, int insideId)
{
	CARInside* pInside = CARInside::GetInstance();
	CARUser *arUser = new CARUser(requestId);
	try
	{
		ARInternalIdList idList;
		ARFieldValueList fieldList;

		idList.numItems = 12;
		idList.internalIdList = (ARInternalId *) new ARInternalId[idList.numItems];

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
			cerr << "Retrieving User '" << requestId <<"' failed!" << endl;
			cerr << pInside->GetARStatusError(this->pStatus);
		}

		delete[] entryId.entryIdList;
		delete[] idList.internalIdList;	
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
	return t1.comparableName.compare(t2.comparableName) < 0;
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

		AREntryListFieldList fields;
		AREntryListFieldValueList values;

		fields.numItems = 10;
		fields.fieldsList = new AREntryListFieldStruct[fields.numItems];
		memset(fields.fieldsList,0,sizeof(AREntryListFieldStruct)*fields.numItems);

		fields.fieldsList[0].fieldId = 1;		//RequestId
		fields.fieldsList[1].fieldId = 1700;	//ApplicationName
		fields.fieldsList[2].fieldId = 1701;	//RoleName
		fields.fieldsList[3].fieldId = 1702;	//RoleID
		fields.fieldsList[4].fieldId = 2001;	//GroupName Test
		fields.fieldsList[5].fieldId = 2002;	//GroupName Production
		fields.fieldsList[6].fieldId = 2;		//CreatedBy
		fields.fieldsList[7].fieldId = 3;		//Created
		fields.fieldsList[8].fieldId = 5;		//ModifiedBy
		fields.fieldsList[9].fieldId = 6;		//Modified

		for (unsigned int k=0; k<fields.numItems; ++k) { fields.fieldsList[k].columnWidth=1; fields.fieldsList[k].separator[0]='|'; }

		if(ARLoadARQualifierStruct(this->pControl, 
			schemaName,
			NULL, 
			qualString, 
			&qualifier, 
			this->pStatus) == AR_RETURN_OK)
		{
			if (ARGetListEntryWithFields(
				this->pControl,
				schemaName,
				&qualifier,
				&fields,
				NULL,0,0,0,
				&values,
				0,this->pStatus) == AR_RETURN_OK)
			{
				for (unsigned int row=0; row<values.numItems; ++row)
				{
					if (values.entryList[row].entryValues->fieldValueList[0].value.u.charVal == NULL) continue;

					CARRole role(values.entryList[row].entryValues->fieldValueList[0].value.u.charVal);
					role.insideId = row;

					if(values.entryList[row].entryValues->fieldValueList[1].value.u.charVal != NULL)
						role.applicationName = values.entryList[row].entryValues->fieldValueList[1].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[2].value.u.charVal != NULL)
					{
						role.roleName = values.entryList[row].entryValues->fieldValueList[2].value.u.charVal;
						role.name = role.roleName;
						role.comparableRoleName = role.roleName;
					}

					if(values.entryList[row].entryValues->fieldValueList[3].value.u.charVal != NULL)
						role.roleId = values.entryList[row].entryValues->fieldValueList[3].value.u.intVal;

					if(values.entryList[row].entryValues->fieldValueList[4].value.u.charVal != NULL)
					{
						string tmp = values.entryList[row].entryValues->fieldValueList[4].value.u.charVal;

						vector<string> tmpGroupList;
						tmpGroupList.clear();

						//				CUtil::SplitString(tmp, ";", tmpGroupList, false);
						CUtil::SplitString(tmp, tmpGroupList);

						role.testGroupId = atoi(tmpGroupList[0].c_str());
					}

					if(values.entryList[row].entryValues->fieldValueList[5].value.u.charVal != NULL)
					{
						string tmp = values.entryList[row].entryValues->fieldValueList[5].value.u.charVal;

						vector<string> tmpGroupList;
						tmpGroupList.clear();

						//				CUtil::SplitString(tmp, ";", tmpGroupList, false);
						CUtil::SplitString(tmp, tmpGroupList);


						role.productionGroupId = atoi(tmpGroupList[0].c_str());
					}

					if(values.entryList[row].entryValues->fieldValueList[6].value.u.charVal != NULL)
						role.createdBy = values.entryList[row].entryValues->fieldValueList[6].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[7].value.u.timeVal != NULL)
						role.created = values.entryList[row].entryValues->fieldValueList[7].value.u.timeVal;

					if(values.entryList[row].entryValues->fieldValueList[8].value.u.charVal != NULL)
						role.modifiedBy = values.entryList[row].entryValues->fieldValueList[8].value.u.charVal;

					if(values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal != NULL)
						role.modified = values.entryList[row].entryValues->fieldValueList[9].value.u.timeVal;

					LOG << "Role '" << role.roleName <<"' [OK]" << endl;	

					listResult.push_back(role);
				}
				FreeAREntryListFieldValueList(&values,false);
			}
		}

		delete[] fields.fieldsList;
		delete[] qualString;
	}
	catch(...)
	{		
		throw(AppException("Error loading roles.", "undefined", "DataFactory"));
	}
}

// TODO: function currently unused
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
				role->comparableRoleName = role->roleName;
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
	return t1.comparableRoleName.compare(t2.comparableRoleName) < 0;
}

void CARDataFactory::Sort(list<CARRole> &listResult)
{	
	listResult.sort(SortRolesByName);
}
